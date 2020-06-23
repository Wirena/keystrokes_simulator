
#include "process_overlord.h"


bool ProcessOverlord::Process::Start() {
    if (IsAlive())
        return false;
    ZeroMemory(&processInfo, sizeof(processInfo));
    if (FAILED(CreateProcess(NULL, const_cast<wchar_t *>(filePath.c_str()), NULL, NULL, FALSE, 0, NULL, NULL,
                             &processStartupInfo, &processInfo)))
        return false;

    return true;
}

bool ProcessOverlord::Process::Suspend() {
    HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    threadEntry.dwSize = sizeof(THREADENTRY32);
    Thread32First(hThreadSnapshot, &threadEntry);
    do {
        if (threadEntry.th32OwnerProcessID == processInfo.dwProcessId) {
            HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, threadEntry.th32ThreadID);
            SuspendThread(hThread);
            CloseHandle(hThread);
        }
    } while (Thread32Next(hThreadSnapshot, &threadEntry));
    CloseHandle(hThreadSnapshot);
    suspended = true;
    return true;
}

bool ProcessOverlord::Process::Resume() {
    HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    threadEntry.dwSize = sizeof(THREADENTRY32);
    Thread32First(hThreadSnapshot, &threadEntry);
    do {
        if (threadEntry.th32OwnerProcessID == processInfo.dwProcessId) {
            HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, threadEntry.th32ThreadID);
            ResumeThread(hThread);
            CloseHandle(hThread);
        }
    } while (Thread32Next(hThreadSnapshot, &threadEntry));
    CloseHandle(hThreadSnapshot);
    suspended = false;
    return true;
}

bool ProcessOverlord::Process::IsAlive() {
    DWORD state = 0;
    GetExitCodeProcess(processInfo.hThread, &state);
    return state == STILL_ACTIVE;
}

bool ProcessOverlord::Process::Kill() {
    if (FAILED(TerminateProcess(processInfo.hProcess, 0)))
        return false;
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
    ZeroMemory(&processInfo, sizeof(processInfo));
    return true;
}


bool ProcessOverlord::Switch() {
   auto twiceInactiveTime= switchDelay*2;
    if (suppress) {
        auto time = std::chrono::system_clock::now();
        if (time - lastSwitch > (proc.IsSuspended()?(twiceInactiveTime):(switchDelay))) {
            lastSwitch = time;
            if (lastSwitchSuspend) {
                proc.Resume();
                lastSwitchSuspend = false;
            } else {
                proc.Suspend();
                lastSwitchSuspend = true;
            }
        }
    }
    return true;
}

bool ProcessOverlord::SetSuppressState(bool state) {
    suppress = state;
    if (lastSwitchSuspend)
        proc.Resume();
    return true;
}
