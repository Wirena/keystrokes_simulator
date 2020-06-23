#include "gui.h"


LRESULT CALLBACK MainProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    MainWindow *mainWndInst = MainWindow::GetInstance();
    switch (msg) {
        case WM_CLOSE: {
            PostQuitMessage(0);
            break;
        }
        case WM_GETMINMAXINFO: {
            auto sizeInfo = reinterpret_cast<MINMAXINFO *>(lparam);
            sizeInfo->ptMinTrackSize.x = mainWndInst->MIN_WIDTH;
            sizeInfo->ptMinTrackSize.y = mainWndInst->MIN_HEIGHT;
            break;
        }
        case WM_SIZE: {
            static bool fistCall = true;
            if (!fistCall) {
                mainWndInst->RelocateControls(static_cast<int>(LOWORD(lparam) - mainWndInst->currentWidth),
                                              static_cast<int>(HIWORD(lparam) - mainWndInst->currentHeight));
                mainWndInst->currentHeight = static_cast<int>(HIWORD(lparam));
                mainWndInst->currentWidth = static_cast<int>(LOWORD(lparam));
                break;
            } else
                fistCall = false;
            break;
        }
        case WM_COMMAND: {
            if (reinterpret_cast<HWND>(lparam) == mainWndInst->startButton->GetHWND() &&
                HIWORD(wparam) == BN_CLICKED) {
                if (mainWndInst->processCaptured && mainWndInst->outType == OutputType::SLOW) {
                    TCHAR delay[10]{0};
                    GetWindowText(mainWndInst->delayEditor->GetHWND(), delay, 10);
                    mainWndInst->inpSimulator->setSpeed(_wtoi(delay));
                }
                mainWndInst->PrintText();
            } else if (reinterpret_cast<HWND>(lparam) == mainWndInst->modeSlowButton->GetHWND() &&
                       HIWORD(wparam) == BN_CLICKED) {
                mainWndInst->SetSlowMode(true);

            } else if (reinterpret_cast<HWND>(lparam) == mainWndInst->modeFastButton->GetHWND() &&
                       HIWORD(wparam) == BN_CLICKED) {
                if (mainWndInst->processCaptured)
                    mainWndInst->SetSlowMode(false);
                else {
                    MessageBox(0, TEXT("Drag and drop Aurora EXE to the Paster icon"), TEXT("Error"), MB_OK);
                    SendMessage(mainWndInst->modeSlowButton->GetHWND(), BM_CLICK, 0, 0);
                }
            } else if (reinterpret_cast<HWND>(lparam) == mainWndInst->clearButton->GetHWND() &&
                       HIWORD(wparam) == BN_CLICKED) {
                SetWindowText(mainWndInst->GetTextEditor()->GetHWND(), TEXT(""));
            } else if (reinterpret_cast<HWND>(lparam) == mainWndInst->pasteButton->GetHWND() &&
                       HIWORD(wparam) == BN_CLICKED) {
                mainWndInst->FillEditFromClipboard();
            } else if (reinterpret_cast<HWND>(lparam) == mainWndInst->restartPrcButton->GetHWND() &&
                       HIWORD(wparam) == BN_CLICKED) {
                mainWndInst->restartPrc =
                        BST_CHECKED == SendMessage(mainWndInst->restartPrcButton->GetHWND(), BM_GETCHECK, 0, 0);
            }

            break;
        }
        case WM_CTLCOLORSTATIC: {
            return (INT_PTR) GetStockObject(WHITE_BRUSH);
        }
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}


ATOM MainWindow::RegisterMainWindowClass() {
    auto icon = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(MAIN_ICON), IMAGE_ICON, 64, 64, 0));
    WindowClass.cbSize = sizeof(WindowClass);
    WindowClass.style = CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = (WNDPROC) MainProc;
    WindowClass.lpszClassName = name;
    WindowClass.hInstance = hInst;
    WindowClass.hIcon = icon;
    WindowClass.hIconSm = icon;
    WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WindowClass.hbrBackground = static_cast<HBRUSH> (GetStockObject(WHITE_BRUSH));
    return RegisterClassEx(&WindowClass);
}

void MainWindow::InitGui(HINSTANCE hInst) {
    inpSimulator = InputSimulator::GetInstance();
    this->hInst = hInst;
    InitCommonControls();
    RegisterMainWindowClass();
    hMainWnd = CreateWindow(name, TEXT("Paster 1.1.0"),
                            WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_CAPTION, 100, 100, MIN_WIDTH,
                            MIN_HEIGHT, NULL, NULL, NULL, hInst);
    RECT mainWndClientCoords;
    GetClientRect(hMainWnd, &mainWndClientCoords);
    currentWidth = mainWndClientCoords.right;
    currentHeight = mainWndClientCoords.bottom;
    textEditor = new Edit(hMainWnd, TEXT("Editor"), 10, 10, 380, 350, hInst);
    delayStatic = new Static(hMainWnd, TEXT("input delay, ms"), 80, 430, 100, 20, hInst);
    delayEditor = new Edit(hMainWnd, TEXT("65"), WS_VISIBLE | WS_CHILD | ES_NUMBER | WS_BORDER, 185, 430, 40, 20,
                           hInst);
    TCHAR delayDefault[]{TEXT("65")};

    clearButton = new Button(hMainWnd, TEXT("Clear"), 250, 375, 60, 30, hInst);
    pasteButton = new Button(hMainWnd, TEXT("Paste"), 320, 375, 60, 30, hInst);
    startButton = new Button(hMainWnd, TEXT("Start"), 250, 420, 130, 30, hInst);

    modeFastButton = new Button(hMainWnd, TEXT("fast"), WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP, 10, 385,
                                50, 20, hInst);
    modeSlowButton = new Button(hMainWnd, TEXT("slow"), WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 10, 410, 50, 20,
                                hInst);


    prcCapturedSignal = new Button(hMainWnd, TEXT("process captured:"),
                                   WS_VISIBLE | WS_CHILD | BS_CHECKBOX | BS_RIGHTBUTTON, 80, 370, 130,
                                   20,
                                   hInst);
    SendMessage(prcCapturedSignal->GetHWND(), BM_SETCHECK, BST_CHECKED, 0);
    restartPrcButton = new Button(hMainWnd, TEXT("restart after pasting"),
                                  WS_VISIBLE | WS_CHILD | BS_CHECKBOX | BS_AUTOCHECKBOX,
                                  80, 400, 160, 20,
                                  hInst);
    SendMessage(restartPrcButton->GetHWND(), BM_SETCHECK, BST_CHECKED, 0);
    SendMessage(prcCapturedSignal->GetHWND(), BM_SETCHECK, BST_UNCHECKED, 0);
    SendMessage(modeSlowButton->GetHWND(), BM_CLICK, 0, 0);
    SetWindowText(delayEditor->GetHWND(), delayDefault);

}

MainWindow::~MainWindow() {
    delete startButton;
    delete textEditor;
    delete prcCapturedSignal;
    delete modeSlowButton;
    delete modeFastButton;
    delete delayStatic;
    delete pasteButton;
    delete clearButton;
    delete delayEditor;
    delete restartPrcButton;
    if (AuroraPrc)
        delete AuroraPrc;
}

void MainWindow::RelocateControls(int deltaX, int deltaY) {

    restartPrcButton->MoveControl(deltaX, deltaY);
    startButton->MoveControl(deltaX, deltaY);
    modeSlowButton->MoveControl(deltaX, deltaY);
    modeFastButton->MoveControl(deltaX, deltaY);
    delayStatic->MoveControl(deltaX, deltaY);
    delayEditor->MoveControl(deltaX, deltaY);
    pasteButton->MoveControl(deltaX, deltaY);
    clearButton->MoveControl(deltaX, deltaY);
    prcCapturedSignal->MoveControl(deltaX, deltaY);
    textEditor->ResizeControl(deltaX, deltaY);

}


int MainWindow::PrintText() {
    Sleep(3000);
    unsigned length = GetWindowTextLength(textEditor->GetHWND());
    editorTextPtr.require(length + 1);
    editorTextPtr.zeroAll();
    GetWindowText(textEditor->GetHWND(), editorTextPtr.getPtr(), length + 1);
    InputSimulator *inptSim = InputSimulator::GetInstance();
    bool res = inptSim->WriteText(editorTextPtr.getPtr(), length);
    if (processCaptured && outType == OutputType::FAST && restartPrc) {
        Sleep(100);
        if (!AuroraPrc->proc.IsAlive())
            AuroraPrc->proc.Start();
    }
    return res;
}


#define SHOW_CLIPBOARD_ERROR_MSG MessageBox(NULL, TEXT("Something went wrong with clipboard, close this message and try again"), TEXT("Error"),MB_OK | MB_ICONWARNING)

void MainWindow::FillEditFromClipboard() {
    if (!OpenClipboard(hMainWnd)) {
        SHOW_CLIPBOARD_ERROR_MSG;
        return;
    }
    HANDLE hClipbrdData;
#ifdef UNICODE
    hClipbrdData = GetClipboardData(CF_UNICODETEXT);
#elif
    hClipbrdData = GetClipboardData(CF_TEXT);
#endif
    if (!hClipbrdData) {
        SHOW_CLIPBOARD_ERROR_MSG;
        CloseClipboard();
        return;
    }
    auto clipbrdPtr = static_cast<TCHAR *>(GlobalLock(hClipbrdData));
    if (!clipbrdPtr) {
        SHOW_CLIPBOARD_ERROR_MSG;
        CloseClipboard();
        return;
    }
    SetWindowText(textEditor->GetHWND(), clipbrdPtr);
    GlobalUnlock(hClipbrdData);
    CloseClipboard();
}

#undef SHOW_CLIPBOARD_ERROR_MSG

void MainWindow::SetSlowMode(bool slowSet) {
    outType = (slowSet) ? (OutputType::SLOW) : (OutputType::FAST);
    EnableWindow(restartPrcButton->GetHWND(), !slowSet);
    EnableWindow(delayEditor->GetHWND(), slowSet);
    EnableWindow(delayStatic->GetHWND(), slowSet);
    if (slowSet)
        inpSimulator->SetOutputType(OutputType::SLOW);
    else
        inpSimulator->SetOutputType(OutputType::FAST);
}

void MainWindow::SetExecPath(TCHAR *path) {
    if (AuroraPrc)
        delete AuroraPrc;
    AuroraPrc = new ProcessOverlord(path);
    if (!AuroraPrc->proc.Start()) {
        MessageBox(NULL, TEXT("Couldn't start process, error occured"), TEXT("Error"), MB_OK | MB_ICONWARNING);
        delete AuroraPrc;
        processCaptured = false;
    } else {
        processCaptured = true;
        restartPrc = true;
        SendMessage(prcCapturedSignal->GetHWND(), BM_SETCHECK, BST_CHECKED, 0);
        inpSimulator->SetPrcOverlord(AuroraPrc);
    }

}


Control::Control(HWND
                 hParentWindow, DWORD style, const TCHAR *clsName, const TCHAR *name, int x,
                 int y, int width, int height, HINSTANCE hInst) {
    controlHandle = CreateWindow(clsName, name, style, x, y, width, height, hParentWindow, NULL, hInst, NULL);
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->hInst = hInst;
}

void TrackBar::SetBuddys(HWND hMainWnd, const TCHAR *fistText, const TCHAR *secondText) {
    firstBuddy = CreateWindow(WC_STATIC, fistText, WS_CHILD | WS_VISIBLE | SS_RIGHT, 0, 0, 50, 20, hMainWnd, NULL,
                              hInst, NULL);
    secondBuddy = CreateWindow(WC_STATIC, secondText, WS_CHILD | WS_VISIBLE | SS_LEFT, 0, 0, 50, 20, hMainWnd, NULL,
                               hInst, NULL);
    SendMessage(controlHandle, TBM_SETBUDDY, (WPARAM) true, (LPARAM) firstBuddy);
    SendMessage(controlHandle, TBM_SETBUDDY, (WPARAM) false, (LPARAM) secondBuddy);

}

Static::Static(HWND hParentWindow,
               const TCHAR *name,int x,int y,int width,int height, HINSTANCE hInstance)
        : Control(hParentWindow, WS_VISIBLE | WS_CHILD | SS_LEFT, WC_STATIC, name, x, y,
        width, height, hInstance) {}

Control::~Control() {
    DestroyWindow(controlHandle);
}

Edit::Edit(HWND hParentWindow, const TCHAR *name,int x,int y,int width,int height, HINSTANCE hInstance) :
        Control(hParentWindow, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT | WS_HSCROLL | WS_VSCROLL | ES_MULTILINE, WC_EDIT, TEXT(""), x, y, width, height, hInstance) {}

Edit::Edit(HWND hParentWindow, const TCHAR *name, DWORD style, int x, int y, int width, int height, HINSTANCE hInstance)
        : Control(hParentWindow, style,
                  WC_EDIT, TEXT(""),
                  x, y, width, height, hInstance) {}

Button::Button(HWND hParentWindow,const TCHAR *name,int x,int y,int width,int height, HINSTANCE hInstance) :
        Control(hParentWindow, WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, WC_BUTTON, name, x, y,
                width, height,
                hInstance) {}

Button::Button(HWND hParentWindow, const TCHAR *name, DWORD style, int x, int y, int width, int height,
               HINSTANCE hInstance) :
        Control(hParentWindow, style, WC_BUTTON, name, x, y,width, height,hInstance) {}

TrackBar::TrackBar(HWND hParentWindow, const TCHAR *name, int x, int y, int width, int height, HINSTANCE hInstance) :
        Control(hParentWindow, WS_VISIBLE | WS_CHILD, TRACKBAR_CLASS,
                name, x, y, width, height, hInstance) {}

bool Control::SetSubclassPrc(LRESULT CALLBACK(*subFunc)(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR)) {
    return SetWindowSubclass(controlHandle, subFunc, 0, 0);
}

void Control::MoveControl(int deltaX, int deltaY) {
    this->x += deltaX;
    this->y += deltaY;
    MoveWindow(controlHandle, this->x, this->y, this->width,
               this->height, true);
}

void TrackBar::MoveControl(int deltaX, int deltaY) {
    this->x += deltaX;
    this->y += deltaY;
    MoveWindow(controlHandle, this->x, this->y, this->width,
               this->height, true);
    SendMessage(controlHandle, TBM_SETBUDDY, (WPARAM) true, (LPARAM) firstBuddy);
    SendMessage(controlHandle, TBM_SETBUDDY, (WPARAM) false, (LPARAM) secondBuddy);

}

TrackBar::~TrackBar() {
    if (firstBuddy)
        DestroyWindow(firstBuddy);
    if (secondBuddy)
        DestroyWindow(secondBuddy);
}

void Control::ResizeControl(int deltaX, int deltaY) {
    this->width += deltaX;
    this->height += deltaY;
    MoveWindow(controlHandle, this->x, this->y, this->width,
               this->height, true);
}


