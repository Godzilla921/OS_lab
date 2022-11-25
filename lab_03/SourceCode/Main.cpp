# include <iostream>
# include <windows.h>
# include <psapi.h>
# include <tlhelp32.h>
# include <iomanip>
# include <map>
# include <string>
using namespace std;

// �����ֽ�ת���ĳ����γ���
const DWORDLONG to_KB = (DWORDLONG)1 << 10;
const DWORDLONG to_MB = (DWORDLONG)1 << 20;
const DWORDLONG to_GB = (DWORDLONG)1 << 30;
const DWORDLONG to_TB = (DWORDLONG)1 << 40;

// ������������
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
    cout << "-----------��ӭ�����ڴ����ʵ��-----------" << endl;
    cout << "���������һЩ�������鿴��ǰϵͳ���ڴ���Ϣ" << endl;
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
    cout << "���������" << endl;;
    return 0;
}

// ��ӡ��ǰϵͳ����Ϣ
void showSystemInfo() {
    cout << "---------------ϵͳ��Ϣ---------------" << endl;
    SYSTEM_INFO systemInfo;
    // �õ���ǰ��ϵͳ��Ϣ
    ZeroMemory(&systemInfo, sizeof(systemInfo));
    GetSystemInfo(&systemInfo);
    cout << "����������: " << systemInfo.dwActiveProcessorMask << endl;
    cout << "�������ȼ�: " << systemInfo.wProcessorLevel << endl;
    cout << "�������汾: " << systemInfo.wProcessorRevision << endl;
    cout << "����������: " << systemInfo.dwProcessorType << endl;
    cout << "����������: " << systemInfo.dwNumberOfProcessors << endl;
    cout << "��������ҳ��С(KB): " << systemInfo.dwPageSize / to_KB << endl;
    cout << "�����ڴ�ռ������(KB): " << systemInfo.dwAllocationGranularity / to_KB << endl;
    cout << "���Ѱַ��Ԫ: " << systemInfo.lpMaximumApplicationAddress << endl;
    cout << "��СѰַ��Ԫ: " << systemInfo.lpMinimumApplicationAddress << endl;
    cout << "--------------------------------------" << endl;
}

void showMemoryStatus() {
    cout << "-----------�����������ڴ���Ϣ------------" << endl;
    MEMORYSTATUSEX memoryStatusEx;
    ZeroMemory(&memoryStatusEx, sizeof(memoryStatusEx));
    // ��ʼ���ṹ�ĳ���
    memoryStatusEx.dwLength = sizeof(memoryStatusEx);
    // �õ���ǰ���õ�����������ڴ���Ϣ
    if (GlobalMemoryStatusEx(&memoryStatusEx)) {
        cout << "�ṹ�ĳ���B: " << memoryStatusEx.dwLength << endl;
        cout << "�����ڴ��ʹ����(%): " << memoryStatusEx.dwMemoryLoad << endl;
        cout << "�����ڴ������: "; showSize(memoryStatusEx.ullTotalPhys); cout << endl;
        cout << "�����ڴ��ʣ����: "; showSize(memoryStatusEx.ullAvailPhys); cout << endl;
        cout << "ϵͳҳ���ļ���С: "; showSize(memoryStatusEx.ullTotalPageFile); cout << endl;
        cout << "ϵͳ����ҳ���ļ���С: "; showSize(memoryStatusEx.ullAvailPageFile); cout << endl;
        cout << "�����ڴ������: "; showSize(memoryStatusEx.ullTotalVirtual); cout << endl;
        cout << "�����ڴ��ʣ����: "; showSize(memoryStatusEx.ullAvailVirtual); cout << endl;
    }
    else {
        cout << "��ȡ�����������ڴ�ʧ��!" << endl;
    }
    cout << "-----------------------------------------" << endl;
}

void showSystemPerformence() {
    cout << "----------------ϵͳ����-----------------" << endl;
    PERFORMANCE_INFORMATION performanceInfo;
    ZeroMemory(&performanceInfo, sizeof(performanceInfo));
    performanceInfo.cb = sizeof(performanceInfo);
    if (GetPerformanceInfo(&performanceInfo, sizeof(performanceInfo))) {
        cout << "ϵͳ��ǰ�ύ��ҳ��: " << performanceInfo.CommitTotal << endl;
        cout << "ϵͳ�����ύ�ĵ�ǰ���ҳ��: " << performanceInfo.CommitLimit << endl;
        cout << "���ϴ�ϵͳ�������������������ύ״̬�����ҳ��: " << performanceInfo.CommitPeak << endl;
        cout << "ʵ�������ڴ���(��ҳΪ��λ): " << performanceInfo.PhysicalTotal << endl;
        cout << "��ǰ���õ������ڴ���(��ҳΪ��λ): " << performanceInfo.PhysicalAvailable << endl;
        cout << "��ҳΪ��λ��ϵͳ�����ڴ���: " << performanceInfo.SystemCache << endl;
        cout << "��ҳ�ͷǷ�ҳ�ں˳��е�ǰ�ڴ���ܺ�(��ҳΪ��λ): " << performanceInfo.KernelTotal << endl;
        cout << "��ҳ�ں˳��е�ǰλ��ҳ�е��ڴ�: " << performanceInfo.KernelPaged << endl;
        cout << "��ǰλ�ڷǷ�ҳ�ں˳��е��ڴ�(��ҳΪ��λ): " << performanceInfo.KernelNonpaged << endl;
        cout << "ҳ���С :"; showSize(performanceInfo.PageSize); cout << endl;
        cout << "��ǰ�򿪾����: " << performanceInfo.HandleCount << endl;
        cout << "��ǰ������: " << performanceInfo.ProcessCount << endl;
        cout << "��ǰ�߳���: " << performanceInfo.ThreadCount << endl;
    }
    else {
        cout << "��ȡϵͳ����ʧ�ܣ�" << endl;
    }
    cout << "-----------------------------------------" << endl;
}

// ��ӡ��ǰϵͳ�����н�����Ϣ
void showProcessInfo() {
    cout << "---------------�������̵��ڴ�ʹ�����---------------" << endl;
    PROCESSENTRY32 processEntry32;
    ZeroMemory(&processEntry32, sizeof(processEntry32));
    processEntry32.dwSize = sizeof(processEntry32);
    // ��õ�ǰϵͳ�Ŀ���
    HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (handle == INVALID_HANDLE_VALUE) {
        cout << "��ȡϵͳ���̿���ʧ�ܣ�" << endl;
        return;
    }
    // ��ϵͳ���յĵ�һ������û�н��뻺���������ȡ����ʧ��
    if (!Process32First(handle, &processEntry32)) {
        cout << "��ȡ����ʧ�ܣ�" << endl;
        return;
    }
    cout << left;
    cout << setw(10) << "PID" << "\t";
    cout << setw(10) << "cntThreads" << "\t";
    cout << setw(40) << "szExeFile" << "\t";
    cout << setw(10) << "Size" << endl;
    do {
        // ʹ��OpenProcess ������ȡ���̵ľ��
        HANDLE hd = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processEntry32.th32ProcessID);
        PROCESS_MEMORY_COUNTERS pmc;  // �洢���̵��ڴ�ʹ�����
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
    cout << "------------��ѯ�����ַ�ռ䲼���빤����------------" << endl;
    cout << "����������Ҫ��ѯ�Ľ���PID: ";
    DWORD pid;
    HANDLE handle;
    while (1) {
        cin >> pid;
        handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
        if (handle != NULL) {
            break;
        }
        else {
            cout << "�������PID��Ч����������ȷ��PID: ";
        }
    }
    MEMORY_BASIC_INFORMATION mbi;
    ZeroMemory(&mbi, sizeof(mbi));
    SYSTEM_INFO systemInfo;
    // �õ���ǰ��ϵͳ��Ϣ
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
            cout << "��ȡ�����ڴ�ռ�ʧ��!" << endl;
            break;
        }
    }
    cout << "----------------------------------------------------" << endl;
}

void showHelp() {
    cout << "------------------------------  " << endl;
    cout << "s  (�鿴��ǰϵͳ��״̬��Ϣ)     " << endl;
    cout << "m  (�鿴��ǰϵͳ���ڴ���Ϣ)     " << endl;
    cout << "b  (�鿴��ǰϵͳ������)         " << endl;
    cout << "p  (�鿴��ǰϵͳ�����н���)     " << endl;
    cout << "v  (�鿴ĳһ���̵������ڴ���Ϣ) " << endl;
    cout << "q  (�رճ���)                   " << endl;
    cout << "h  (�鿴����)                   " << endl;
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

// ��16λ�� unicode ����ת��Ϊstring����
string WCHARtoString(LPCWSTR pwszSrc)
{
    // ʹ�� WideCharToMultiByte ��2�ֽڵ�Unicodeת��Ϊ char ����
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