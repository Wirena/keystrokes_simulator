#include "input_emulator.h"

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
    /* Yep yep yep, im sending ctrl+s keystroke right after the main message.
    it works on my machine ;) but some crashes before ctrl+s sequence was sent were reported
    dont really know the nature of this behavior, perhaps it depends on amount of time aurora needs to handle input message,
    the less it takes, the more chances it would proccess all message before it notices high type speed*/
    
    ptr->type = INPUT_KEYBOARD;
    ptr->ki.wVk = VK_CONTROL;
    ptr->ki.wScan = 0;
    ptr->ki.dwFlags = 0;
    ptr++;

    ptr->type = INPUT_KEYBOARD;
    ptr->ki.wScan = L'S';
    ptr->ki.dwFlags = KEYEVENTF_UNICODE;
    ptr++;
    ptr->type = INPUT_KEYBOARD;
    ptr->ki.wScan = L'S';
    ptr->ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
    ptr++;
    ptr->type = INPUT_KEYBOARD;
    ptr->ki.wVk = VK_CONTROL;
    ptr->ki.wScan = 0;
    ptr->ki.dwFlags = KEYEVENTF_KEYUP;
    ptr++;

}

void InputEmulator::AddCharToKeys(INPUT *key, TCHAR chr) {
    if(chr==TAB_SYMBOL){
        return;   //gonna try to add formating later, sending tabs in unicode mode hangs Aurora, i have to check other options
        key->type = INPUT_KEYBOARD;
        key->ki.wVk = VK_TAB;
        key->ki.wScan=0;
        key->ki.dwFlags = 0;
        key++;
        key->type = INPUT_KEYBOARD;
        key->ki.wVk = VK_TAB;
        key->ki.wScan=0;
        key->ki.dwFlags = 0;
    }else {
        key->type = INPUT_KEYBOARD;
        key->ki.wScan = chr;
        key->ki.dwFlags = KEYEVENTF_UNICODE;
        key++;
        key->type = INPUT_KEYBOARD;
        key->ki.wScan = chr;
        key->ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
    }
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
    resultKeystokesAmount+=5;//for last ctrl+s;
}

int InputEmulator::InputText(const TCHAR *text, unsigned length) {
    textLength = length;
    this->text = text;
    AnalyzeText();
    keys.require(resultKeystokesAmount);
    keys.zeroAll();
    FillKeysArray();
    for (unsigned i = 0; i < resultKeystokesAmount; i++) {
        AddInputToQueue(keys.getPtr() + i);
    }
    return 0;
}

void InputEmulator::AddInputToQueue(INPUT *inp) {
    SendInput(1, inp, sizeof(INPUT));
    if (GetLastError() == ERROR_NOT_ENOUGH_QUOTA) {
        SetLastError(EXIT_SUCCESS);
        Sleep(2);
        AddInputToQueue(inp);
    }

}
