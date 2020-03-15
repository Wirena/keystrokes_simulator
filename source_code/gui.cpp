//
// Created by Wawerma on 22.02.2020.
//
#include "gui.h"

const TCHAR MainWindow::delayInfoText[] = TEXT("Delay after pressing Start:");


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
                mainWndInst->PrintText();
            } else if (reinterpret_cast<HWND>(lparam) == mainWndInst->clearButton->GetHWND() &&
                       HIWORD(wparam) == BN_CLICKED) {
                SetWindowText(mainWndInst->GetTextEditor()->GetHWND(), TEXT(""));
            } else if (reinterpret_cast<HWND>(lparam) == mainWndInst->pasteButton->GetHWND() &&
                       HIWORD(wparam) == BN_CLICKED) {
                mainWndInst->FillEditFromClipboard();
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
    this->hInst = hInst;
    InitCommonControls();
    RegisterMainWindowClass();
    hMainWnd = CreateWindow(name, TEXT("Paster"),
                            WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_CAPTION, 100, 100, MIN_WIDTH,
                            MIN_HEIGHT, NULL, NULL, NULL, hInst);
    RECT mainWndClientCoords;
    GetClientRect(hMainWnd, &mainWndClientCoords);
    currentWidth = mainWndClientCoords.right;
    currentHeight = mainWndClientCoords.bottom;
    textEditor = new Edit(hMainWnd, TEXT("Editor"), 10, 10, 380, 350, hInst);
    clearButton = new Button(hMainWnd, TEXT("Clear"), 250, 375, 60, 30, hInst);
    pasteButton = new Button(hMainWnd, TEXT("Paste"), 320, 375, 60, 30, hInst);
    delayTrackBar = new TrackBar(hMainWnd, TEXT("Delay"), 40, 410, 150, 30, hInst);
    startButton = new Button(hMainWnd, TEXT("Start"), 250, 420, 130, 30, hInst);
    delayInfo = new Static(hMainWnd, delayInfoText, 40, 375, 200, 20, hInst);
    delayTrackBar->SetBuddys(hMainWnd, TEXT("1 sec"), TEXT("6 sec"));
    SendMessage(delayTrackBar->GetHWND(), TBM_SETRANGE, true, MAKELPARAM(1000, 6000));
    SendMessage(delayTrackBar->GetHWND(), TBM_SETPOS, true, 3000);
    SendMessage(delayTrackBar->GetHWND(), TBM_SETTIC, 0, 2000);
    SendMessage(delayTrackBar->GetHWND(), TBM_SETTIC, 0, 3000);
    SendMessage(delayTrackBar->GetHWND(), TBM_SETTIC, 0, 4000);
    SendMessage(delayTrackBar->GetHWND(), TBM_SETTIC, 0, 5000);

}

MainWindow::~MainWindow() {
    delete startButton;
    delete pasteButton;
    delete clearButton;
    delete delayTrackBar;
    delete textEditor;
    delete delayInfo;
}

void MainWindow::RelocateControls(int deltaX, int deltaY) {
    startButton->MoveControl(deltaX, deltaY);
    clearButton->MoveControl(deltaX, deltaY);
    delayTrackBar->MoveControl(deltaX, deltaY);
    pasteButton->MoveControl(deltaX, deltaY);
    textEditor->ResizeControl(deltaX, deltaY);
    delayInfo->MoveControl(deltaX, deltaY);
}

void MainWindow::SetKeyPressEmulatorFunc(std::function<bool(const TCHAR *, unsigned)> *callbackFunc) {
    cStyleCallbackUsed = false;
    keyEmulatorCallback.stdFunctionalCallback = new std::function<bool(const TCHAR *textPtr, unsigned length)>;
    keyEmulatorCallback.stdFunctionalCallback = callbackFunc;
}

void MainWindow::SetKeyPressEmulatorFunc(bool (*classicCallback)(const TCHAR *, unsigned)) {
    cStyleCallbackUsed = true;
    keyEmulatorCallback.classicCallback = classicCallback;
}

int MainWindow::PrintText() {
    Sleep(SendMessage(delayTrackBar->GetHWND(), TBM_GETPOS, 0, 0));
    unsigned length = GetWindowTextLength(textEditor->GetHWND());
    editorTextPtr.require(length + 1);
    editorTextPtr.zeroAll();
    GetWindowText(textEditor->GetHWND(), editorTextPtr.getPtr(), length + 1);
    return callBack(editorTextPtr.getPtr(), length);
}

int MainWindow::callBack(TCHAR *text, unsigned length) {
    return (cStyleCallbackUsed) ? (keyEmulatorCallback.classicCallback(text, length))
                                : ((*(keyEmulatorCallback.stdFunctionalCallback))(text, length));
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

Control::Control(HWND
                 hParentWindow, DWORD
                 style,
                 const TCHAR *clsName,
                 const TCHAR *name,
                 int x,
                 int y,
                 int width,
                 int height,
                 HINSTANCE hInst) {
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

Static::Static(HWND
               hParentWindow,
               const TCHAR *name,
               int x,
               int y,
               int width,
               int height, HINSTANCE
               hInstance)
        : Control(
        hParentWindow, WS_VISIBLE | WS_CHILD | SS_LEFT, WC_STATIC, name, x, y,
        width, height, hInstance) {}

Control::~Control() {
    DestroyWindow(controlHandle);
}

Edit::Edit(HWND
           hParentWindow,
           const TCHAR *name,
           int x,
           int y,
           int width,
           int height, HINSTANCE
           hInstance) :
        Control(hParentWindow, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT | WS_HSCROLL | WS_VSCROLL | ES_MULTILINE,
                WC_EDIT, TEXT(""),
                x, y,
                width, height,
                hInstance) {}

Button::Button(HWND
               hParentWindow,
               const TCHAR *name,
               int x,
               int y,
               int width,
               int height, HINSTANCE
               hInstance) :
        Control(hParentWindow, WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, WC_BUTTON, name, x, y,
                width, height,
                hInstance) {}

TrackBar::TrackBar(HWND
                   hParentWindow,
                   const TCHAR *name,
                   int x,
                   int y,
                   int width,
                   int height, HINSTANCE
                   hInstance) :
        Control(hParentWindow, WS_VISIBLE | WS_CHILD, TRACKBAR_CLASS,
                name, x, y,
                width, height,
                hInstance) {}

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


