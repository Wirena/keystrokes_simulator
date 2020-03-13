//
// Created by Wawerma on 23.02.2020.
//

#pragma comment(linker,"/manifestdependency:\"type='win32' \
    name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
    processorArchitecture='*' publicKeyToken='6595b64144ccf1df' \
    language='*'\"")

#define UNICODE
#define _UNICODE
#define WINVER 0x0503
#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <functional>
#include <iostream>
#include "gui.h"
#include "input_emulator.h"
#include "ptr.h"
