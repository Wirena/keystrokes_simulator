//
// Created by Wawerma on 22.02.2020.
//

#ifndef GUI_H
#define GUI_H
#define WINVER 0x0503
#define _WIN32_IE 0x1000
#define UNICODE
#define _UNICODE
#define DELAY_INFO_TEXT TEXT("Delay after pressing Start:")

#include <windows.h>
#include <winuser.h>
#include <commctrl.h>
#include <uxtheme.h>
#include <functional>
#include <iostream>
#include <tchar.h>
#include "ptr.h"


class Control {
protected:
    HWND controlHandle;
    HINSTANCE hInst;
    int x, y, width, height;
public:
    virtual void MoveControl(int deltaX, int deltaY);

    void ResizeControl(int deltaX, int deltaY);

    bool SetSubclassPrc(LRESULT CALLBACK(*subFunc)(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR));

    //friend LRESULT CALLBACK MyWndProc (HWND , UINT , WPARAM , LPARAM , UINT_PTR ,DWORD_PTR );

    HWND GetHWND() { return controlHandle; };

    virtual ~Control();

    Control(HWND hParentWindow, DWORD style, const TCHAR *clsName, const TCHAR *name, int x, int y, int width,
            int height,
            HINSTANCE hInst);
};

class Button : public Control {

public:
    Button(HWND hParentWindow, const TCHAR *name, int x, int y, int width, int height, HINSTANCE hInstance);
};

class Edit : public Control {
public:
    Edit(HWND hParentWindow, const TCHAR *name, int x, int y, int width, int height, HINSTANCE hInstance);;

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
    Static(HWND hParentWindow, const TCHAR *name, int x, int y, int width, int height, HINSTANCE hInstance);;

};

class MainWindow {
    MainWindow() {};
    ptrWrap<TCHAR> editorTextPtr;
    HINSTANCE hInst;
    TCHAR name[17] = TEXT("MainWindowClass");
    bool cStyleCallbackUsed = true;


    union {
        std::function<bool(const TCHAR *textPtr, unsigned length)> *stdFunctionalCallback;

        bool (*classicCallback)(const TCHAR *textPtr, unsigned length);
        //callbackFunction(){};
    } keyEmulatorCallback;

    Button *clearButton = nullptr, *pasteButton = nullptr, *startButton = nullptr;
    Edit *textEditor = nullptr;
    Static *delayInfo = nullptr;
    TrackBar *delayTrackBar = nullptr;
    HWND hMainWnd = NULL;
    WNDCLASSEX WindowClass = {0};
    int currentWidth;
    int currentHeight;


    ATOM RegisterMainWindowClass();

    int callBack(TCHAR *text, unsigned length);

    void RelocateControls(int deltaX, int deltaY);

    void FillEditFromClipboard();

public:


    friend LRESULT CALLBACK MainProc(HWND, UINT, WPARAM, LPARAM);



    Edit *GetTextEditor() { return textEditor; };


    const int MIN_WIDTH = 410;
    const int MIN_HEIGHT = 500;


    int PrintText();


    int GetCurrentWidth() { return currentWidth; };

    int GetCurrentHeight() { return currentHeight; };

    void SetKeyPressEmulatorFunc(std::function<bool(const TCHAR *, unsigned )> *callbackFunc);

    void SetKeyPressEmulatorFunc(bool(*classicCallback)(const TCHAR *, unsigned ));

    void SetCurrentWidth(int width) { currentWidth = width; };

    void SetCurrentHeight(int height) { currentHeight = height; };



    ~MainWindow();

    void InitGui(HINSTANCE hInst);


    static MainWindow *GetInstance() {
        static MainWindow instMain;
        return &instMain;
    }

};

LRESULT CALLBACK MainProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

#endif
