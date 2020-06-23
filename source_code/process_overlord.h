
#ifndef CTRLV_KEYPASTER_PROCESS_OVERLORD_H
#define CTRLV_KEYPASTER_PROCESS_OVERLORD_H
#define WINVER 0x0503
#define UNICODE
#define _UNICODE

#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <chrono>
#include <string>


class ProcessOverlord {

    class Process {
        std::wstring filePath;
        bool suspended = false;
        HANDLE processSnapshot{0};
        THREADENTRY32 threadEntry{0};
        STARTUPINFO processStartupInfo{0};
        PROCESS_INFORMATION processInfo{0};
    public:
        Process(const std::wstring &path) : filePath{path} {}

        bool Start();

        bool Suspend();

        bool Resume();

        bool IsAlive();

        bool IsSuspended() { return suspended; };

        bool Kill();

        ~Process() {
            TerminateProcess(processInfo.hProcess, 0);
            CloseHandle(processInfo.hProcess);
            CloseHandle(processInfo.hThread);
        };
    };


    const std::chrono::duration<int, std::milli> switchDelay{30};
    std::chrono::time_point<std::chrono::system_clock> lastSwitch;
    bool suppress;
    bool lastSwitchSuspend;
public:
    ~ProcessOverlord() {
        if (lastSwitchSuspend)
            proc.Resume();
    };

    ProcessOverlord(const std::wstring &file) : proc{file}, lastSwitchSuspend{false},
                                                suppress{false} { lastSwitch = std::chrono::system_clock::now(); }

    Process proc;

    bool SetSuppressState(bool);

    bool Switch();
};


#endif //CTRLV_KEYPASTER_PROCESS_OVERLORD_H
