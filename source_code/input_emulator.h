//
// Created by Wawerma on 24.02.2020.
//

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



class InputEmulator {
    ptrWrap<INPUT> keys;
    unsigned textLength = 0;
    const TCHAR *text = nullptr;

    void AddInputToQueue(INPUT *);

    void FillKeysArray();

    void AddCharToKeys(INPUT *, TCHAR);

    unsigned resultKeystokesAmount = 0;

    void AnalyzeText();

public:

    int InputTextTmpVer(const TCHAR *, unsigned);

    int InputText(const TCHAR *, unsigned);

    static InputEmulator *GetInstance() {
        static InputEmulator eiInst;
        return &eiInst;
    }

};

#endif //NPUT_EMULATOR_H
