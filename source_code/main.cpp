#include "header.h"

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine,
                     int nShowCmd) {
    InputEmulator *inptEm = InputEmulator::GetInstance();
    std::function<bool(const TCHAR *, unsigned )> callbackFunc = std::bind(&InputEmulator::InputText,
                                                                                        inptEm, std::placeholders::_1,
                                                                                        std::placeholders::_2);
    //FreeConsole();
    MainWindow *mainGUI = MainWindow::GetInstance();
    mainGUI->SetKeyPressEmulatorFunc(&callbackFunc);
    mainGUI->InitGui(hInstance);

    MSG message = {0};
    int mainWindowState = 0;
    while ((mainWindowState = GetMessage(&message, NULL, 0, 0)) != 0) {
        if(!mainWindowState || mainWindowState==-1)
            break;
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
    return message.wParam;

    return 0;
}
