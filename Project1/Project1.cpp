/* MINT.EXE - Located in X:\MiniNT\mint.exe
* 
* 
*                       MiniNT Stage 2 Session Manager
*                       Copyright (C) cubesided 2025
* 
*                       not affiliated with microsoft.
* 
* 
*                  (THIS IS A MICROSOFT WINDOWS NT MODIFICATION.)
*       (MININT IS AND WILL NOT EVER BE DISTRIBUTED IN A CLOSED SOURCE MANER.)
* 
* 
* 
* 
* 
* Developed and Distributed by Cooper Greene (cgree)
*/

#include "framework.h"
#include "Project1.h"

#include <string>
#include <vector>
#include <iostream>
#include <locale>
#include <codecvt>

#define MAX_LOADSTRING 100


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // the title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
std::vector<HWND> childWindows;                 // list of child windows
std::wstring g_wndLoc;                        // Run dialog cmd write LOCATION
std::wstring g_wndName;                        // Run dialog cmd write NAME
BOOL g_grabWnd = false;
BOOL debug = true;

// Child Window Variables 
HWND runDialog; // PLACEHOLDER

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    ChildWindowProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK    ChildDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK    RunDialog(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
void                LaunchChildProcess(HWND hWnd, const std::wstring& appName, const std::wstring& windowTitle); // win32 processes

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    if (debug) {
        AllocConsole();

        FILE* stream;
        freopen_s(&stream, "CONOUT$", "w", stdout);
        freopen_s(&stream, "CONOUT$", "w", stderr);
        //printf("Hello console on\n");
    }

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROJECT1));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!IsDialogMessage(runDialog, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }



    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PROJECT1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   for (HWND hwndChild : childWindows) {
       ShowWindow(hwndChild, SW_SHOWNORMAL);
       UpdateWindow(hwndChild);
   }

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_SETFOCUS: {
        if (!childWindows.empty()) {
            // Avoid taking focus if a child exists
            SetFocus(childWindows[0]);
        }
        break;
    }
    case WM_KEYDOWN: {
        if (wParam == VK_F9) {
            HWND currentFocus = GetFocus();
            if (currentFocus == hWnd) {
                OutputDebugString(L"hello");
                if (!childWindows.empty()) {
                    SetFocus(childWindows[0]);
                    OutputDebugString(L"bring to last child");
                }
            }
            else {
                for (size_t i = 0; i < childWindows.size(); ++i) {
                    if (currentFocus == childWindows[i]) {
                        size_t nextIndex = (i + 1) % childWindows.size();
                        SetFocus(childWindows[nextIndex]);
                        OutputDebugString(L"bring to next child/parent");
                        return 0;
                    }
                }
            }
            // Fallback: return focus to parent if none is found
            SetFocus(hWnd);
            OutputDebugString(L"fallback");
            HWND focusedWnd = GetFocus();
            if (focusedWnd) {
                char wndClass[256];
                GetClassNameA(focusedWnd, wndClass, sizeof(wndClass));
                OutputDebugStringA(("Focused window: " + std::string(wndClass) + "\n").c_str());
            }
        }
        break;
    }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case ID_NEWWINDOW_CMD: {
                LaunchChildProcess(hWnd, L"X:\\Windows\\System32\\cmd.exe", L"Administrator: X:\\Windows\\System32\\cmd.exe");
                break;
            }
            case ID_NEWWINDOW_NOTEPAD: {
                LaunchChildProcess(hWnd, L"X:\\Windows\\notepad.exe", L"Untitled - Notepad");
                break;
            }
            case ID_NEWWINDOW_APPLICATION2: {
                HWND hwndChild = CreateDialog(hInst, MAKEINTRESOURCE(IDD_FORMVIEW), hWnd, ChildWindowProc);
                ShowWindow(hwndChild, SW_SHOWNORMAL);
                UpdateWindow(hwndChild);
                BringWindowToTop(hwndChild);
                std::cout << "Dialog(" << hwndChild << ") Is open." << std::endl;
                break;
            }
            case IDM_ABOUT: {// dialog boxes should only be used for errors.
                //DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                HWND abbBox = CreateDialog(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, ChildDialogProc);
                if (abbBox) {
                    SetForegroundWindow(abbBox);  // Bring it to the foreground
                    SetFocus(abbBox);            // Set keyboard focus
                    ShowWindow(abbBox, SW_SHOWNORMAL);
                    UpdateWindow(abbBox);
                }
                std::cout << "Dialog(" << abbBox << ") Is open." << std::endl;
                break;
            }
            case ID_RUNPROGRAM: {
                runDialog = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_RUNDIALOG), hWnd, RunDialog);
                if (runDialog) {
                    SetWindowLongPtr(runDialog, GWL_STYLE, GetWindowLongPtr(runDialog, GWL_STYLE) | WS_CHILD);
                    SetParent(runDialog, hWnd);
                    ShowWindow(runDialog, SW_SHOW);
                }
                std::cout << "Dialog(" << runDialog << ") Is open." << std::endl;
                break;
            }
            case IDM_EXIT: // End MiniNT Session
                MessageBox(hWnd, L"This will end your Windows Session.", L"End Session", MB_OK | MB_ICONQUESTION);
                DestroyWindow(hWnd);
                break;
            default:

                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
            if (g_grabWnd) {
                LaunchChildProcess(hWnd, g_wndLoc, g_wndName);
                //OutputDebugStringW(g_wndLoc);
                g_grabWnd = false;
            }
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


INT_PTR CALLBACK ChildWindowProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        childWindows.push_back(hwndDlg);
        return TRUE;  // Initialization code here
    /*case WM_COMMAND:
        if (LOWORD(wParam) == ID)
        {
            EndDialog(hwndDlg, IDCLOSE);
            return TRUE;
        }
        break;
    */
    case WM_CLOSE:
        EndDialog(hwndDlg, IDCANCEL);
        return true;
    }

    return FALSE;
}

INT_PTR CALLBACK ChildDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        return TRUE;  // Initialization code here
        childWindows.push_back(hwndDlg);
        //SetDlgItemText(hwndDlg, IDC_STATIC, L"the new text");
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            EndDialog(hwndDlg, IDOK);
            return TRUE;
            }
            break;
        
    case WM_CLOSE:
        EndDialog(hwndDlg, IDCANCEL);
        return true;
        childWindows.erase(
            std::remove(childWindows.begin(), childWindows.end(), hwndDlg),
            childWindows.end()
        );

    }
    return FALSE;
}

INT_PTR CALLBACK RunDialog(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    /*case WM_MOUSEACTIVATE:
        SetFocus(hwndDlg);
        return MA_ACTIVATE;  // Allow activation on mouse click
    case WM_LBUTTONDOWN:
        SetFocus(hwndDlg);
        break;
    /*case WM_ACTIVATE: {
        if (wParam != WA_INACTIVE) {
            SetFocus(hwndDlg);
        }
        break;
    }*/
    case WM_INITDIALOG:
        return TRUE;  // Initialization code here
        childWindows.push_back(hwndDlg);
        //SetDlgItemText(hwndDlg, IDC_STATIC, L"the new text");
    case WM_COMMAND: {
        // Check if the button (with ID IDC_OK) was pressed
        if (LOWORD(wParam) == IDOK) {
            // Get the HWND of the control (edit control in this case)
            HWND wndLoc = GetDlgItem(hwndDlg, IDC_WNDLOC);
            HWND wndName = GetDlgItem(hwndDlg, IDC_WNDNAME);
            if (wndLoc != NULL && wndName != NULL) {
                // Allocate a buffer to store the text
                const int bufferSize = 256;
                char buffer1[bufferSize];
                char buffer2[bufferSize];

                // Get the text from the edit control
                int length1 = GetWindowTextA(wndLoc, buffer1, bufferSize);
                int length2 = GetWindowTextA(wndName, buffer2, bufferSize);

                // Ensure there was some text
                if (length1 > 0) {
                    // Null-terminate the buffer if the length is smaller than bufferSize
                    buffer1[length1] = '\0';

                    // Push the buffer into a std::string
                    std::string retrievedText(buffer1);
                    g_wndLoc = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(retrievedText);

                    std::cout << buffer1 << std::endl;
                }
                else {
                    // Handle the case where no text was found
                    std::cout << "No text in the IDC_WNDLOC control." << std::endl;
                    //std::cout << "debug test, hi console" << "\n";
                }
                Sleep(400);
                if (length2 > 0) {
                    // Null-terminate the buffer if the length is smaller than bufferSize
                    buffer2[length2] = '\0';

                    // Push the buffer into a std::string
                    std::string retrievedText(buffer2);
                    g_wndName = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(retrievedText);

                    // Output the retrieved text
                    std::cout << buffer2 << std::endl;

                    // start process
                    g_grabWnd = true;
                }
                else {
                    // Handle the case where no text was retrieved
                    std::cout << "No text in the IDC_WNDNAME control." << std::endl;
                }
            }
            else {
                std::cout << "Could not find controls with IDs" << std::endl;
            }

            // Close the dialog
            EndDialog(hwndDlg, IDOK);
            return (INT_PTR)TRUE;
        }
        break;
    }

    case WM_CLOSE:
        EndDialog(hwndDlg, IDCANCEL);
        return true;
    }
    return FALSE;
}

// Function to launch a process and embed its window
void LaunchChildProcess(HWND hWnd, const std::wstring& appName, const std::wstring& windowTitle) {
    // Initialize STARTUPINFO and PROCESS_INFORMATION structures
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    // Create the child process. Use the appName as the command line.
    BOOL processCreated = CreateProcess(
        appName.c_str(),  // Application name
        NULL,             // Command line (NULL means the appName is used)
        NULL,             // Process security attributes
        NULL,             // Thread security attributes
        FALSE,            // Inherit handles
        CREATE_NEW_CONSOLE,  // Create a new console for the child process
        NULL,             // Environment variables
        NULL,             // Current directory
        &si,              // STARTUPINFO structure
        &pi               // PROCESS_INFORMATION structure
    );

    if (!processCreated) {
        std::wcerr << L"CreateProcess failed: " << GetLastError() << std::endl;
        return;
    }

    // Wait for the child process to initialize its window
    WaitForInputIdle(pi.hProcess, INFINITE);

    // Give time for the window to be fully created before finding it
    HWND hwndChild = NULL;
    int attempts = 0;
    while (hwndChild == NULL && attempts < 10) {
        hwndChild = FindWindow(NULL, windowTitle.c_str()); // Find child window by title
        if (hwndChild == NULL) {
            Sleep(100); // Sleep briefly to give time for the window to appear
        }
        attempts++;
    }

    // If the window was found, embed it
    if (hwndChild != NULL) {
        SetParent(hwndChild, hWnd);  // Embed the child window inside the parent window
        //childWindows.push_back(hwndChild); // Store the child window handle
        SetWindowPos(hwndChild, HWND_TOP, 0, 0, 400, 400, SWP_NOZORDER); // Position the window
        std::cout << "Process(" << hwndChild << ") Is open" << std::endl;
    }
    else {
        std::wcerr << L"Failed to find child window: " << windowTitle << std::endl;
    }

    // Close handles for the child process
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}