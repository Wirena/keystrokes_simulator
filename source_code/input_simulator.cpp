
#include "input_simulator.h"


void InputSimulator::FillKeysArray() {
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
    AddCtrlSTokeys(ptr);

}

void InputSimulator::AddCtrlSTokeys(INPUT *ptr) {
    ptr->type = INPUT_KEYBOARD;
    ptr->ki.wVk = VK_CONTROL;
    ptr->ki.wScan = 0;
    ptr->ki.dwFlags = 0;
    ptr++;
    ptr->type = INPUT_KEYBOARD;
    ptr->ki.wScan = 'S';
    ptr->ki.dwFlags = KEYEVENTF_UNICODE;
    ptr++;
    ptr->type = INPUT_KEYBOARD;
    ptr->ki.wScan = 'S';
    ptr->ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
    ptr++;
    ptr->type = INPUT_KEYBOARD;
    ptr->ki.wVk = VK_CONTROL;
    ptr->ki.wScan = 0;
    ptr->ki.dwFlags = KEYEVENTF_KEYUP;
    ptr++;
}

void InputSimulator::AddCharToKeys(INPUT *key, TCHAR chr) {
    if (chr == TAB_SYMBOL) {
        return;
        key->type = INPUT_KEYBOARD;
        key->ki.wVk = VK_TAB;
        key->ki.wScan = 0;
        key->ki.dwFlags = 0;
        key++;
        key->type = INPUT_KEYBOARD;
        key->ki.wVk = VK_TAB;
        key->ki.wScan = 0;
        key->ki.dwFlags = 0;
    } else {
        key->type = INPUT_KEYBOARD;
        key->ki.wScan = chr;
        key->ki.dwFlags = KEYEVENTF_UNICODE;
        key++;
        key->type = INPUT_KEYBOARD;
        key->ki.wScan = chr;
        key->ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
    }
}

void InputSimulator::AnalyzeText() {
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
    resultKeystokesAmount += 4;//for last ctrl+s;
}

int InputSimulator::WriteText(const TCHAR *text, unsigned length) {
    textLength = length;
    this->text = text;
    AnalyzeText();
    keys.require(resultKeystokesAmount);
    keys.zeroAll();
    FillKeysArray();
    if (outputType == OutputType::SLOW) {

        for (unsigned i = 0; i < resultKeystokesAmount; i++) {
            Sleep(slowTypeOutputDelay);
            AddInputToQueue(keys.getPtr() + i);
        }
    } else {
        prc->SetSuppressState(true);
        for (unsigned i = 0; i < resultKeystokesAmount; i++) {
            prc->Switch();
            AddInputToQueue(keys.getPtr() + i);
        }
        prc->SetSuppressState(false);
    }
    //
    return 0;
}

void InputSimulator::AddInputToQueue(INPUT *inp) {
    SendInput(1, inp, sizeof(INPUT));
    if (GetLastError() == ERROR_NOT_ENOUGH_QUOTA) {
        SetLastError(EXIT_SUCCESS);
        Sleep(2);
        prc->Switch();
        AddInputToQueue(inp);
    }
}
