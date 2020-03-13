//
// Created by Wawerma on 24.02.2020.
//
#include "input_emulator.h"


int InputEmulator::InputTextTmpVer(const TCHAR *text, unsigned length) {
    textLength = length;
    this->text = text;
    AnalyzeText();
    keys.require(resultKeystokesAmount);
    keys.zeroAll();
    INPUT *ptr = keys.getPtr();
    unsigned keysIterator = 0, textIterator = 0;
    TCHAR currentChar, lastChar = 0x1000; //0x1000 is not tab, space, cr or rf
    while (textIterator < length) {
        currentChar = *(text + textIterator);
        if (!(currentChar == TAB_SYMBOL ||
              ((lastChar == CR_SYMBOL || lastChar == LF_SYMBOL) && currentChar == SPACE_SYMBOL))) {
            AddCharToKeys(ptr + keysIterator, currentChar);
            keysIterator += 2;
            lastChar = currentChar;
        }
        textIterator++;
    }
    unsigned leftOver = resultKeystokesAmount % SENDINPUT_LIMIT;
    unsigned fullPacks = resultKeystokesAmount / SENDINPUT_LIMIT;
    //std::cout<<resultKeystokesAmount<<std::endl<<fullPacks<<std::endl<<leftOver;
    unsigned i = 0;

    for (; i < fullPacks; i++) {
        SendInput(SENDINPUT_LIMIT, ptr + i * SENDINPUT_LIMIT, sizeof(INPUT));
        Sleep(2000);
    }
    if (leftOver)
        std::cout << SendInput(leftOver, ptr + i * SENDINPUT_LIMIT, sizeof(INPUT)) << std::endl;
    keys.zeroAll();

}


void InputEmulator::FillKeysArray() {
    unsigned textIterator = 0;
    INPUT *ptr = keys.getPtr();
    TCHAR currentChar, lastChar = 0x100;//0x100 is not a CR or a LF
    while (textIterator < textLength) {
        currentChar = *(text + textIterator);
        if (!(currentChar == TAB_SYMBOL ||
              ((lastChar == CR_SYMBOL || lastChar == LF_SYMBOL) && currentChar == SPACE_SYMBOL))) {
            AddCharToKeys(ptr, currentChar);
            ptr += 2;
            lastChar = currentChar;
        }
        textIterator++;
    }
}

void InputEmulator::AddCharToKeys(INPUT *key, TCHAR chr) {
    key->type = INPUT_KEYBOARD;
    key->ki.wScan = chr;
    key->ki.dwFlags = KEYEVENTF_UNICODE;
    key++;
    key->type = INPUT_KEYBOARD;
    key->ki.wScan = chr;
    key->ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
}

void InputEmulator::AnalyzeText() {
    resultKeystokesAmount = 0;
    TCHAR currentChar, lastChar = 0x1000;//0x1000 is not tab, space, cr or rf
    for (unsigned i = 0; i < textLength; i++) {
        currentChar = *(text + i);
        if (!(currentChar == TAB_SYMBOL ||
              ((lastChar == CR_SYMBOL || lastChar == LF_SYMBOL) && currentChar == SPACE_SYMBOL))) {
            resultKeystokesAmount++;
            lastChar = currentChar;
        }
    }
    resultKeystokesAmount *= 2;
}

int InputEmulator::InputText(const TCHAR *text, unsigned length) {
    textLength = length;
    this->text = text;
    AnalyzeText();
    keys.require(resultKeystokesAmount);
    keys.zeroAll();
    FillKeysArray();
    for (int i = 0; i < resultKeystokesAmount; i++)
        AddInputToQueue(keys.getPtr() + i);
    return 0;
}

void InputEmulator::AddInputToQueue(INPUT *inp) {
    SendInput(1, inp, sizeof(INPUT));
    if (GetLastError() == ERROR_NOT_ENOUGH_QUOTA) {
        SetLastError(EXIT_SUCCESS);
        Sleep(10);
        AddInputToQueue(inp);
    }

}
