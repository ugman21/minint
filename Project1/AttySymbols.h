#pragma once

#include "framework.h"
#include <fstream>
#include <json/json.h>
#include <algorithm>

class AttySymbols
{
public:
    static LRESULT CALLBACK ChildWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void do_something() {
        // test
    };
    void loadAndDeploy(HINSTANCE hInst, HWND parentWindow, const char* ftype);
    void pushText(HDC hdc, Json::Value mainJson, Json::Value apperanceJson, Json::FastWriter fastWriter);

};