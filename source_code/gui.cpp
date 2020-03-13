
#include "gui.h"

ULONG_PTR EnableVisualStyles(VOID) {
    TCHAR dir[MAX_PATH];
    ULONG_PTR ulpActivationCookie = FALSE;
    ACTCTX actCtx =
            {
                    sizeof(actCtx),
                    ACTCTX_FLAG_RESOURCE_NAME_VALID
                    | ACTCTX_FLAG_SET_PROCESS_DEFAULT
                    | ACTCTX_FLAG_ASSEMBLY_DIRECTORY_VALID,
                    TEXT("shell32.dll"), 0, 0, dir, (LPCTSTR) 124
            };
    UINT cch = GetSystemDirectory(dir, sizeof(dir) / sizeof(*dir));
    if (cch >= sizeof(dir) / sizeof(*dir)) { return FALSE; /*shouldn't happen*/ }
    dir[cch] = TEXT('\0');
    ActivateActCtx(CreateActCtx(&actCtx), &ulpActivationCookie);
    return ulpActivationCookie;
}


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
            //if (mainWndInst->delayInfo->GetHWND() == (HWND)lparam || mainWndInst->delayTrackBar->GetHWND() ==(HWND)lparam)
            {
                return (INT_PTR) GetStockObject(WHITE_BRUSH);
            }
        }

    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}


/*LRESULT CALLBACK
MyWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    MainWindow *mainWndInst = MainWindow::GetInstance();
    std::cout<<"weee";
    switch (msg) {
        case WM_CTLCOLORSTATIC: {
            std::cout<<"paintin";
            HDC hdcStatic = (HDC) wparam;
            SetTextColor(hdcStatic, RGB(0, 0, 0));
            SetBkMode(hdcStatic, TRANSPARENT);
            return (INT_PTR) GetStockObject(WHITE_BRUSH);
        }

    }

    return DefSubclassProc(hwnd, msg, wparam, lparam);
}
*/


ATOM MainWindow::RegisterMainWindowClass() {
    WindowClass.cbSize = sizeof(WindowClass);
    WindowClass.style = CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = (WNDPROC) MainProc;
    WindowClass.lpszClassName = name;
    WindowClass.hInstance = hInst;
    WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WindowClass.hbrBackground = static_cast<HBRUSH> (GetStockObject(WHITE_BRUSH));
    return RegisterClassEx(&WindowClass);
}

void MainWindow::InitGui(HINSTANCE hInst) {

    InitCommonControls();
    //EnableVisualStyles();

    RegisterMainWindowClass();
    hMainWnd = CreateWindow(name, TEXT("Paster"),
                            WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_CAPTION, 100, 100, MIN_WIDTH,
                            MIN_HEIGHT, NULL, NULL, hInst, NULL);
    //SetWindowTheme(hMainWnd,L"", NULL);
    RECT mainWndClientCoords;
    GetClientRect(hMainWnd, &mainWndClientCoords);
    currentWidth = mainWndClientCoords.right;
    currentHeight = mainWndClientCoords.bottom;
    textEditor = new Edit(hMainWnd, TEXT("Editor"), 10, 10, 380, 350, hInst);
    clearButton = new Button(hMainWnd, TEXT("Clear"), 250, 370, 60, 30, hInst);
    pasteButton = new Button(hMainWnd, TEXT("Paste"), 320, 370, 60, 30, hInst);
    delayTrackBar = new TrackBar(hMainWnd, TEXT("Delay"), 40, 410, 150, 30, hInst);
    startButton = new Button(hMainWnd, TEXT("Start"), 250, 420, 130, 30, hInst);
    delayInfo = new Static(hMainWnd, DELAY_INFO_TEXT, 5, 370, 240, 20, hInst);
    delayTrackBar->SetBuddys(hMainWnd, TEXT("1 sec"), TEXT("6 sec"));
    //std::cout<<delayInfo->SetSubclassPrc(MyWndProc);
    SendMessage(delayTrackBar->GetHWND(), TBM_SETRANGE, true, MAKELPARAM(1000, 6000));
    SendMessage(delayTrackBar->GetHWND(), TBM_SETPOS, true, static_cast<LPARAM>(3000));
    SendMessage(delayTrackBar->GetHWND(), TBM_SETTIC, NULL, 2000);
    SendMessage(delayTrackBar->GetHWND(), TBM_SETTIC, NULL, 3000);
    SendMessage(delayTrackBar->GetHWND(), TBM_SETTIC, NULL, 4000);
    SendMessage(delayTrackBar->GetHWND(), TBM_SETTIC, NULL, 5000);
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
    Sleep(SendMessage(delayTrackBar->GetHWND(), TBM_GETPOS, NULL, NULL));
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

void MainWindow::FillEditFromClipboard() {
    OpenClipboard(hMainWnd);
    HANDLE hClipbrdData;
#ifdef UNICODE
    hClipbrdData = GetClipboardData(CF_UNICODETEXT);
#elif
    hClipbrdData = GetClipboardData(CF_TEXT);
#endif
    auto clipbrdPtr = static_cast<TCHAR *>(GlobalLock(hClipbrdData));
    SetWindowText(textEditor->GetHWND(), clipbrdPtr);
    GlobalUnlock(hClipbrdData);
    CloseClipboard();

}


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
        hParentWindow, WS_VISIBLE | WS_CHILD | SS_CENTER, WC_STATIC, name, x, y,
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


