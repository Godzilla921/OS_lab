# include <stdio.h>
# include <stdlib.h>
# include <windows.h>
# include <time.h>
# define bufferSize 7       //缓冲区个数，实际为 bufferSize -1
# define threadProducer 2   //生产者线程为 2
# define threadConsumer 3   //消费者线程为 3
# define contentSize 12     //每个生产者要生产的产品数量
//定义缓冲区
typedef struct{
    // 定义缓冲池，每个缓冲区大小为10个的字符串 即大小为size的循环队列
    char* buffer[bufferSize];
    // 用以存取缓冲区的指针  当rear==front时缓冲区为空，当(rear+1)%size==front%size时 缓冲区为满
    int front,rear;
}Buffer;

// 定义存储线程信息的结构体
typedef struct{
    HANDLE handle;  // 存储线程的HANDLE
    DWORD id;       // 存储线程的ID
}ThreadInfo;

// 定义数据源结构体
typedef struct{
    char* src[contentSize];
    int size;
}Psrc;

DWORD WaitTime=INFINITE;          //阻塞等待时间
DWORD ProdeucerWaitTime=2000;     //生产者向缓冲区送入产品后的等待时间
DWORD ConsumerWaitTime=3000;      //消费者从缓冲区取出产品后的等待时间
// 定义信号量 
HANDLE mutex;          //生产者与消费者对于缓冲区的互斥信号量
HANDLE empty;          //缓冲区剩余数量的同步信号量
HANDLE full;           //缓冲区中产品个数的同步信号量
// 定义缓冲区 buffer
Buffer buffer;
//生产者 和 消费者进程
DWORD WINAPI Producer(LPVOID Iparam);
DWORD WINAPI Consumer(LPVOID Iparam);

void printNowTime();
void showBuffer(Buffer* buf);
//向缓冲区中添加和取出物品
void pushBuffer(char *src,Buffer *buf);
char* popBuffer(Buffer *buf);

//生产者要加入缓冲区的源数据
Psrc psrc[threadProducer]={
    {
    {"msg-0-1","msg-0-2","msg-0-3","msg-0-4","msg-0-5","msg-0-6",
    "msg-0-7","msg-0-8","msg-0-9","msg-0-10","msg-0-11","msg-0-12"},
    contentSize
    },
    {
    {"msg-1-1","msg-1-2","msg-1-3","msg-1-4","msg-1-5","msg-1-6",
    "msg-1-7","msg-1-8","msg-1-9","msg-1-10","msg-1-11","msg-1-12"},
    contentSize
    }
};
int main()
{
    // 初始化信号量
    empty = CreateSemaphore(NULL, bufferSize-1, bufferSize-1, NULL);
    full = CreateSemaphore(NULL, 0, bufferSize-1, NULL);
    mutex = CreateMutex(NULL, FALSE,NULL);
    // 初始化缓冲区
    buffer.front=buffer.rear=0;
    ThreadInfo producerInfo[threadProducer]; //生产者线程的标识符 
    ThreadInfo consumerInfo[threadConsumer]; //消费者线程的标识符
    for(int i=0;i<threadProducer;++i){
        //创建生产者线程
        producerInfo[i].handle=CreateThread(NULL, 0, Producer, &psrc[i], 0, &producerInfo[i].id);
        if(producerInfo[i].handle==NULL){ // 创建线程失败
            return -1;
        }
    }
    for(int i=0;i<threadConsumer;++i){
        //创建消费者线程
        consumerInfo[i].handle=CreateThread(NULL, 0, Consumer, NULL, 0, &consumerInfo[i].id);
        if(consumerInfo[i].handle==NULL){ // 创建线程失败
            return -1;
        }
    }

    system("pause");   //挂起主线程
    printf("Finished!!\n");
    return 0;
}

//生产者线程
DWORD WINAPI Producer(LPVOID Iparam)
{
    Psrc *psrc=(Psrc *)Iparam;
    for(int i=0;i<psrc->size;++i){
         // 申请一个空缓冲
        if(WaitForSingleObject(empty,WaitTime)==WAIT_OBJECT_0){ 
            //申请缓冲区的使用权
            if(WaitForSingleObject(mutex,WaitTime)==WAIT_OBJECT_0){
                //生产一个产品
//                ``````
                //将产品放入缓冲区
                pushBuffer(psrc->src[i],&buffer);
                //释放一个产品
                ReleaseSemaphore(full, 1, NULL);
                //释放缓冲区的使用权
                ReleaseMutex(mutex);
            }
        }
        //随机等待一段时间
        Sleep(ProdeucerWaitTime); 
    }
}
//消费者线程
DWORD WINAPI Consumer(LPVOID Iparam)
{
    while(1){
        // 申请一个产品
        if(WaitForSingleObject(full,WaitTime)==WAIT_OBJECT_0){ 
            //申请缓冲区的使用权
            if(WaitForSingleObject(mutex,WaitTime)==WAIT_OBJECT_0){
                //从缓冲区中取出一个产品
                char *str=popBuffer(&buffer);
                // 消耗产品
                // ~~~
                //释放一个空缓冲
                ReleaseSemaphore(empty, 1, NULL);
                //释放缓冲区的使用权
                ReleaseMutex(mutex);
            }
        }
        //随机等待一段时间
        Sleep(ConsumerWaitTime); 
    }
}

void pushBuffer(char *src,Buffer *buf)
{
    buf->buffer[buf->rear]=src;
    buf->rear=(buf->rear+1)%bufferSize;
    //输出缓冲区的映像
    printNowTime();
    printf("Push in buffer:\n");
    showBuffer(buf);
}
char* popBuffer(Buffer *buf)
{
    char *str=buf->buffer[buf->front];
    buf->front=(buf->front+1)%bufferSize;
    //输出缓冲区的映像
    printNowTime();
    printf("Pop out buffer:\n");
    showBuffer(buf);
    return str;
}

void printNowTime()
{
    //输出系统当前时间
    time_t nowTime=time(&nowTime);
    struct tm *timeinfo;
    timeinfo=localtime(&nowTime);
    char* str;
    printf("%04d-%02d-%02d %02d:%02d:%02d\n", timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday,
    timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
}
void showBuffer(Buffer* buf)
{
    int begin=buf->front;
    int end=buf->rear;
    int size=(end>=begin)? (end-begin):(end-begin+bufferSize);
    for(int i=0;i<size;++i){
        printf("    %s\n",buf->buffer[(buf->front+i)%bufferSize]);
    }
    if(size==bufferSize-1){
        printf("The buffer is full!\n");
    }else if(size==0){
        printf("The buffer is empty!\n");
    }
}