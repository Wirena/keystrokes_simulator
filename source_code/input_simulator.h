

#ifndef INPUT_EMULATOR_H
#define INPUT_EMULATOR_H
#define WINVER 0x0503
#define SENDINPUT_LIMIT 4000
#define TAB_SYMBOL 0x9
#define CR_SYMBOL 0xD
#define LF_SYMBOL 0xA
#define SPACE_SYMBOL 0x20

#define UNICODE
#define _UNICODE

#include <windows.h>
#include <cstdlib>
#include <iostream>
#include <tchar.h>
#include <cstring>
#include "ptr.h"
#include "process_overlord.h"

enum class OutputType {
    FAST = 0, SLOW = 1
};

class InputSimulator {

    ptrWrap<INPUT> keys;
    unsigned textLength = 0;
    const TCHAR *text = nullptr;
    int slowTypeOutputDelay;
    OutputType outputType;
    unsigned resultKeystokesAmount = 0;

    InputSimulator() = default;

    void AddCtrlSTokeys(INPUT *);

    void AddInputToQueue(INPUT *);

    void FillKeysArray();

    void AddCharToKeys(INPUT *, TCHAR);

    void AnalyzeText();

    ProcessOverlord *prc = nullptr;

public:

    void SetPrcOverlord(ProcessOverlord *ptr) {
        if (prc) delete prc;
        prc = ptr;
    }

    bool SetOutputType(OutputType newType) {
        outputType = newType;
        return true;
    }

    OutputType GetOutputType() { return outputType; }

    bool setSpeed(int spd) {
        slowTypeOutputDelay = spd;
        return true;
    };

    int getSpeed() { return slowTypeOutputDelay; };

    int WriteText(const TCHAR *, unsigned);

    static InputSimulator *GetInstance() {
        static InputSimulator eiInst;
        return &eiInst;
    }

};

#endif //NPUT_EMULATOR_H
