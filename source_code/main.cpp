#include "header.h"

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine,
                     int nShowCmd) {
    MainWindow *mainGUI = MainWindow::GetInstance();
    mainGUI->InitGui(hInstance);
    TCHAR ** argList;
    int argCount;
    argList = CommandLineToArgvW(GetCommandLine(), &argCount);
    if(argCount==2)
        mainGUI->SetExecPath(argList[1]);

    MSG message = {0};
    int mainWindowState = 0;
    while ((mainWindowState = GetMessage(&message, NULL, 0, 0)) != 0) {
        if(!mainWindowState || mainWindowState==-1)
            break;
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
    return message.wParam;
}
