# include <iostream>
# include <windows.h>
# include <psapi.h>
# include <tlhelp32.h>
# include <iomanip>
# include <map>
# include <string>
using namespace std;

// 用以字节转换的长整形常量
const DWORDLONG to_KB = (DWORDLONG)1 << 10;
const DWORDLONG to_MB = (DWORDLONG)1 << 20;
const DWORDLONG to_GB = (DWORDLONG)1 << 30;
const DWORDLONG to_TB = (DWORDLONG)1 << 40;

// 函数声明部分
void showSystemInfo();
void showMemoryStatus();
void showSystemPerformence();
void showProcessInfo();
void showVirtualProcessMemory();
void showHelp();
void showSize(DWORDLONG size, int len = 8);
string WCHARtoString(LPCWSTR pwszSrc);

enum MyEnum
{
    Help, SystemInfo, MemoryStatus, SystemPerformence, ProcessInfo, VirtualProcessMemory, Quit
};
map <DWORD, string> state = {
    {MEM_COMMIT,"MEM_COMMIT"} ,
    {MEM_RESERVE,"MEM_RESERVE"} ,
    {MEM_FREE,"MEM_FREE"}
};
map <DWORD, string> type = {
    {MEM_IMAGE,"MEM_IMAGE"} ,
    {MEM_MAPPED,"MEM_MAPPED"} ,
    {MEM_PRIVATE,"MEM_PRIVATE"}
};
map <DWORD, string> protect = {
    {PAGE_EXECUTE,"PAGE_EXECUTE"} ,
    {PAGE_EXECUTE_READ,"PAGE_EXECUTE_READ"},
    {PAGE_EXECUTE_READWRITE,"PAGE_EXECUTE_READWRITE"},
    {PAGE_EXECUTE_WRITECOPY,"PAGE_EXECUTE_WRITECOPY"},
    {PAGE_NOCACHE,"PAGE_NOCACHE"},
    {PAGE_NOACCESS,"PAGE_NOACCESS"},
    {PAGE_READONLY,"PAGE_READONLY"},
    {PAGE_READWRITE,"PAGE_READWRITE"},
    {PAGE_WRITECOPY,"PAGE_WRITECOPY"},
    {PAGE_GUARD,"PAGE_GUARD"},
    {PAGE_WRITECOMBINE,"PAGE_WRITECOMBINE"}
};
map <string, MyEnum>  in = {
    {"h",Help},
    {"s",SystemInfo},
    {"m",MemoryStatus} ,
    {"b",SystemPerformence} ,
    {"p",ProcessInfo} ,
    {"v",VirtualProcessMemory} ,
    {"q",Quit}
};
int main()
{
    cout << "------------------------------------------" << endl;
    cout << "-----------欢迎来到内存监视实验-----------" << endl;
    cout << "你可以输入一些命令来查看当前系统的内存信息" << endl;
    showHelp();
    string info;
    cin >> info;
    while (info != "q") {
        switch (in[info]) {
        case Help:showHelp(); break;
        case SystemInfo:showSystemInfo(); break;
        case MemoryStatus:showMemoryStatus(); break;
        case SystemPerformence:showSystemPerformence(); break;
        case ProcessInfo: showProcessInfo(); break;
        case VirtualProcessMemory:showVirtualProcessMemory(); break;
        case Quit:break;
        default:break;
        }
        cin >> info;
    };
    cout << "试验结束！" << endl;;
    return 0;
}

// 打印当前系统的信息
void showSystemInfo() {
    cout << "---------------系统信息---------------" << endl;
    SYSTEM_INFO systemInfo;
    // 得到当前的系统信息
    ZeroMemory(&systemInfo, sizeof(systemInfo));
    GetSystemInfo(&systemInfo);
    cout << "处理器掩码: " << systemInfo.dwActiveProcessorMask << endl;
    cout << "处理器等级: " << systemInfo.wProcessorLevel << endl;
    cout << "处理器版本: " << systemInfo.wProcessorRevision << endl;
    cout << "处理器类型: " << systemInfo.dwProcessorType << endl;
    cout << "处理器个数: " << systemInfo.dwNumberOfProcessors << endl;
    cout << "处理器分页大小(KB): " << systemInfo.dwPageSize / to_KB << endl;
    cout << "虚拟内存空间的粒度(KB): " << systemInfo.dwAllocationGranularity / to_KB << endl;
    cout << "最大寻址单元: " << systemInfo.lpMaximumApplicationAddress << endl;
    cout << "最小寻址单元: " << systemInfo.lpMinimumApplicationAddress << endl;
    cout << "--------------------------------------" << endl;
}

void showMemoryStatus() {
    cout << "-----------物理与虚拟内存信息------------" << endl;
    MEMORYSTATUSEX memoryStatusEx;
    ZeroMemory(&memoryStatusEx, sizeof(memoryStatusEx));
    // 初始化结构的长度
    memoryStatusEx.dwLength = sizeof(memoryStatusEx);
    // 得到当前可用的物理和虚拟内存信息
    if (GlobalMemoryStatusEx(&memoryStatusEx)) {
        cout << "结构的长度B: " << memoryStatusEx.dwLength << endl;
        cout << "物理内存的使用率(%): " << memoryStatusEx.dwMemoryLoad << endl;
        cout << "物理内存的总量: "; showSize(memoryStatusEx.ullTotalPhys); cout << endl;
        cout << "物理内存的剩余量: "; showSize(memoryStatusEx.ullAvailPhys); cout << endl;
        cout << "系统页面文件大小: "; showSize(memoryStatusEx.ullTotalPageFile); cout << endl;
        cout << "系统可用页面文件大小: "; showSize(memoryStatusEx.ullAvailPageFile); cout << endl;
        cout << "虚拟内存的总量: "; showSize(memoryStatusEx.ullTotalVirtual); cout << endl;
        cout << "虚拟内存的剩余量: "; showSize(memoryStatusEx.ullAvailVirtual); cout << endl;
    }
    else {
        cout << "获取物理与虚拟内存失败!" << endl;
    }
    cout << "-----------------------------------------" << endl;
}

void showSystemPerformence() {
    cout << "----------------系统性能-----------------" << endl;
    PERFORMANCE_INFORMATION performanceInfo;
    ZeroMemory(&performanceInfo, sizeof(performanceInfo));
    performanceInfo.cb = sizeof(performanceInfo);
    if (GetPerformanceInfo(&performanceInfo, sizeof(performanceInfo))) {
        cout << "系统当前提交的页数: " << performanceInfo.CommitTotal << endl;
        cout << "系统可以提交的当前最大页数: " << performanceInfo.CommitLimit << endl;
        cout << "自上次系统重新启动以来处于已提交状态的最大页数: " << performanceInfo.CommitPeak << endl;
        cout << "实际物理内存量(以页为单位): " << performanceInfo.PhysicalTotal << endl;
        cout << "当前可用的物理内存量(以页为单位): " << performanceInfo.PhysicalAvailable << endl;
        cout << "以页为单位的系统缓存内存量: " << performanceInfo.SystemCache << endl;
        cout << "分页和非分页内核池中当前内存的总和(以页为单位): " << performanceInfo.KernelTotal << endl;
        cout << "分页内核池中当前位于页中的内存: " << performanceInfo.KernelPaged << endl;
        cout << "当前位于非分页内核池中的内存(以页为单位): " << performanceInfo.KernelNonpaged << endl;
        cout << "页面大小 :"; showSize(performanceInfo.PageSize); cout << endl;
        cout << "当前打开句柄数: " << performanceInfo.HandleCount << endl;
        cout << "当前进程数: " << performanceInfo.ProcessCount << endl;
        cout << "当前线程数: " << performanceInfo.ThreadCount << endl;
    }
    else {
        cout << "获取系统性能失败！" << endl;
    }
    cout << "-----------------------------------------" << endl;
}

// 打印当前系统的所有进程信息
void showProcessInfo() {
    cout << "---------------各个进程的内存使用情况---------------" << endl;
    PROCESSENTRY32 processEntry32;
    ZeroMemory(&processEntry32, sizeof(processEntry32));
    processEntry32.dwSize = sizeof(processEntry32);
    // 获得当前系统的快照
    HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (handle == INVALID_HANDLE_VALUE) {
        cout << "获取系统进程快照失败！" << endl;
        return;
    }
    // 若系统快照的第一个进程没有进入缓冲区，则获取进程失败
    if (!Process32First(handle, &processEntry32)) {
        cout << "获取进程失败！" << endl;
        return;
    }
    cout << left;
    cout << setw(10) << "PID" << "\t";
    cout << setw(10) << "cntThreads" << "\t";
    cout << setw(40) << "szExeFile" << "\t";
    cout << setw(10) << "Size" << endl;
    do {
        // 使用OpenProcess 函数获取进程的句柄
        HANDLE hd = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processEntry32.th32ProcessID);
        PROCESS_MEMORY_COUNTERS pmc;  // 存储进程的内存使用情况
        ZeroMemory(&pmc, sizeof(pmc));
        if (GetProcessMemoryInfo(hd, &pmc, sizeof(pmc))) {
            cout << left;
            cout << fixed << setprecision(4);
            cout << setw(10) << processEntry32.th32ProcessID << "\t  ";
            cout << setw(10) << processEntry32.cntThreads << "\t ";
            cout << setw(40) << WCHARtoString(processEntry32.szExeFile) << "\t";
            showSize(pmc.WorkingSetSize, 10); cout << endl;
        }
    } while (Process32Next(handle, &processEntry32));
    CloseHandle(handle);
    cout << "----------------------------------------------------" << endl;
}

void showVirtualProcessMemory() {
    cout << "------------查询虚拟地址空间布局与工作集------------" << endl;
    cout << "请输入你想要查询的进程PID: ";
    DWORD pid;
    HANDLE handle;
    while (1) {
        cin >> pid;
        handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
        if (handle != NULL) {
            break;
        }
        else {
            cout << "您输入的PID无效，请输入正确的PID: ";
        }
    }
    MEMORY_BASIC_INFORMATION mbi;
    ZeroMemory(&mbi, sizeof(mbi));
    SYSTEM_INFO systemInfo;
    // 得到当前的系统信息
    ZeroMemory(&systemInfo, sizeof(systemInfo));
    GetSystemInfo(&systemInfo);
    LPCVOID pBlock = (LPCVOID)systemInfo.lpMinimumApplicationAddress;
    int len = sizeof(pBlock);
    cout << left;
    cout << setw(4*len+1) << "Address" << "\t";
    cout << setw(10) << "Size" << "\t";
    cout << setw(15) << "State" << "\t";
    cout << setw(25) << "Protect" << "\t";
    cout << setw(15) << "Type" << endl;
    while (pBlock < systemInfo.lpMaximumApplicationAddress) {
        if (VirtualQueryEx(handle, pBlock, &mbi, sizeof(mbi)) == sizeof(mbi)) {
            LPCVOID pEnd = (PBYTE)pBlock + mbi.RegionSize;
            cout << left;
            cout << pBlock << "-" << pEnd << "\t";
            showSize(mbi.RegionSize, 10); cout << "\t";
            cout << setw(15) << state[mbi.State] << "\t";
            cout << setw(25) << protect[mbi.Protect] << "\t";
            cout << setw(15) << type[mbi.Type] << endl;
            pBlock = pEnd;
        }
        else {
            cout << "获取虚拟内存空间失败!" << endl;
            break;
        }
    }
    cout << "----------------------------------------------------" << endl;
}

void showHelp() {
    cout << "------------------------------  " << endl;
    cout << "s  (查看当前系统的状态信息)     " << endl;
    cout << "m  (查看当前系统的内存信息)     " << endl;
    cout << "b  (查看当前系统的性能)         " << endl;
    cout << "p  (查看当前系统的所有进程)     " << endl;
    cout << "v  (查看某一进程的虚拟内存信息) " << endl;
    cout << "q  (关闭程序)                   " << endl;
    cout << "h  (查看命令)                   " << endl;
    cout << "--------------------------------" << endl;
}

void showSize(DWORDLONG size, int len) {
    cout << fixed << setprecision(4);
    cout << setw(len);
    cout << left;
    if (size >= to_TB) {
        cout << (DOUBLE)size / to_TB << "TB";
    }
    else if (size >= to_GB) {
        cout << (DOUBLE)size / to_GB << "GB";
    }
    else if (size >= to_MB) {
        cout << (DOUBLE)size / to_MB << "MB";
    }
    else if (size >= to_KB) {
        cout << (DOUBLE)size / to_KB << "KB";
    }
    else {
        cout << (DOUBLE)size << "B";
    }
}

// 将16位的 unicode 编码转化为string类型
string WCHARtoString(LPCWSTR pwszSrc)
{
    // 使用 WideCharToMultiByte 将2字节的Unicode转化为 char 类型
    int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
    if (nLen <= 0)
        return string("");

    char* pszDst = new char[nLen];
    if (NULL == pszDst)
        return string("");

    WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
    pszDst[nLen - 1] = '\0';
    string strTmp(pszDst);
    delete[] pszDst;
    return strTmp;
}