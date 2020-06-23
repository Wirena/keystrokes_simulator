//
// Created by Wawerma on 22.02.2020.
//

#ifndef GUI_H
#define GUI_H
#define WINVER 0x0503
#define UNICODE
#define _UNICODE
#define IDI_MAINICON 101


#include <windows.h>
#include <winuser.h>
#include <commctrl.h>
#include <uxtheme.h>
#include <iostream>
#include <tchar.h>
#include "ptr.h"
#include "resource.h"
#include "input_simulator.h"
#include "process_overlord.h"


class Control {
protected:
    HWND controlHandle;
    HINSTANCE hInst;
    int x, y, width, height;
public:
    virtual void MoveControl(int deltaX, int deltaY);

    void ResizeControl(int deltaX, int deltaY);

    bool SetSubclassPrc(LRESULT CALLBACK(*subFunc)(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR));

    HWND GetHWND() { return controlHandle; };

    virtual ~Control();

    Control(HWND hParentWindow, DWORD style, const TCHAR *clsName, const TCHAR *name, int x, int y, int width,
            int height,
            HINSTANCE hInst);
};

class Button : public Control {

public:
    Button(HWND hParentWindow, const TCHAR *name, int x, int y, int width, int height, HINSTANCE hInstance);

    Button(HWND hParentWindow, const TCHAR *name, DWORD style, int x, int y, int width, int height,
           HINSTANCE hInstance);
};

class Edit : public Control {
public:
    Edit(HWND hParentWindow, const TCHAR *name, int x, int y, int width, int height, HINSTANCE hInstance);

    Edit(HWND hParentWindow, const TCHAR *name, DWORD style, int x, int y, int width, int height, HINSTANCE hInstance);
};

class TrackBar : public Control {
protected:
    HWND firstBuddy = NULL, secondBuddy = NULL;
public:
    TrackBar(HWND hParentWindow, const TCHAR *name, int x, int y, int width, int height, HINSTANCE hInstance);;

    ~TrackBar();

    void MoveControl(int deltaX, int deltaY) override;

    void SetBuddys(HWND hParentWindow, const TCHAR *fistText, const TCHAR *secondText);
};


class Static : public Control {
public:
    Static(HWND hParentWindow, const TCHAR *name, int x, int y, int width, int height, HINSTANCE hInstance);

};

class MainWindow {
    InputSimulator *inpSimulator;

    MainWindow() {};
    ptrWrap<TCHAR> editorTextPtr;
    HINSTANCE hInst;
    TCHAR name[17] = TEXT("MainWindow");
    Button *clearButton = nullptr, *pasteButton = nullptr, *startButton = nullptr;
    Edit *textEditor = nullptr, *delayEditor = nullptr;
    Button *modeFastButton = nullptr, *restartPrcButton = nullptr, *modeSlowButton = nullptr, *prcCapturedSignal = nullptr;
    Static *delayStatic = nullptr;
    HWND hMainWnd = NULL;
    WNDCLASSEX WindowClass = {0};
    int currentWidth;
    int currentHeight;
    const int MIN_WIDTH = 410;
    const int MIN_HEIGHT = 500;
    bool processCaptured = false;
    ProcessOverlord *AuroraPrc = nullptr;

    bool restartPrc;
    ATOM RegisterMainWindowClass();

    void RelocateControls(int deltaX, int deltaY);

    void FillEditFromClipboard();

    void SetSlowMode(bool);

    OutputType outType;

    int PrintText();
public:
    friend LRESULT CALLBACK MainProc(HWND, UINT, WPARAM, LPARAM);

    Edit *GetTextEditor() { return textEditor; };

    ~MainWindow();

    void SetExecPath(TCHAR *);

    void InitGui(HINSTANCE hInst);


    static MainWindow *GetInstance() {
        static MainWindow instMain;
        return &instMain;
    }

};

LRESULT CALLBACK MainProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

#endif
