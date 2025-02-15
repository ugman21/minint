#include "AttySymbols.h"

using namespace std;
#include <algorithm>
#include <array>
#include <iostream>
#include <iterator>
#include <string>

LRESULT AttySymbols::ChildWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    Json::Value mainJson; // MAIN SHIT ONG
    Json::Value apperanceJson; // APPERANCE!!!!
    Json::Reader reader;       // LEAVE THIS SHI
    Json::FastWriter fastWriter; // ?????

    std::ifstream json("currentlyopen.txt");
    std::string mystring;
    getline(json, mystring);

    std::wstring place = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(mystring);

    //grab file from opened
    char* outputString = new char[place.length() + 1];
    size_t outputSize = place.length() + 1; // +1 for null terminator
    outputString = new char[outputSize]; // we want this
    size_t charsConverted = 0;
    const wchar_t* inputW = place.c_str();
    wcstombs_s(&charsConverted, outputString, outputSize, inputW, place.length());

    // addition for loading directories instead of direct json.
    std::string bufferstringa(outputString);
    std::string bufferstringb(outputString);
    bufferstringa.append("\\main.json");
    bufferstringb.append("\\apperance.json");

    const char* pullmdir = bufferstringa.c_str();
    const char* pulladir = bufferstringb.c_str();

    std::ifstream json2(pullmdir);
    std::ifstream json3(pulladir);
    if (json2.is_open()) {
        // parse
        reader.parse(json2, mainJson, false);
    }
    else { std::cout << "cant find main.json, incorrect directory?" << std::endl; }

    if (json3.is_open()) {
        // parse
        reader.parse(json3, apperanceJson, false);
    }
    else { std::cout << "cant find apperance.json, incorrect directory?" << std::endl; }

    switch (message) {
    case WM_NCACTIVATE:
    {
        // wParam tells us whether we are active or inactive, but we are going to ignore
        // that and always pass active down to DefWindowProc so it will draw us active.
        DefWindowProc(hWnd, message, TRUE, lParam);
        std::cout << wParam << std::endl;
        std::cout << "a" << std::endl;
        //return FALSE; // returning false here prevents actual deactivation
        return TRUE; // return true allows deactivation (even though we draw as active)
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        AttySymbols core;

        core.pushText(hdc, mainJson, apperanceJson, fastWriter);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_LBUTTONDOWN:
        //MessageBox(hWnd, TEXT("Child clicked!"), TEXT("Event"), MB_OK);
        SetForegroundWindow(hWnd); // place
        BringWindowToTop(hWnd);
        RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

        std::cout << "should be foreground." << std::endl;
        break;
    case WM_DESTROY:
        //PostQuitMessage(0);
        break;
    case WM_INITDIALOG:
        //AttySymbols::childWindows.push_back(hWnd);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void AttySymbols::loadAndDeploy(HINSTANCE hInst, HWND parentWindow, const char* ftype)
{
    std::string bufferstring;
    bufferstring = ftype;
    bufferstring.append("\\main.json");

    const char* pulldir = bufferstring.c_str();

    std::ifstream json(pulldir);

    Json::Value actualJson;
    Json::Reader reader;
    Json::FastWriter fastWriter;

    std::ofstream myfile;
    myfile.open("currentlyopen.txt");
    std::cout << myfile.is_open() << std::endl;

    myfile << ftype;
    myfile.close();



    reader.parse(json, actualJson);
    std::string output = fastWriter.write(actualJson["name"]);
    std::string holder = fastWriter.write(actualJson["cname"]);

    std::wstring place = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(holder);

    //childWindows.push_back(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(output));

    const wchar_t* cname = place.c_str();

    // Initialize child class
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = AttySymbols::ChildWndProc;  // Pointer to the child window's procedure
    wc.hInstance = hInst;
    wc.lpszClassName = cname;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&wc)) {
        //MessageBox(NULL, TEXT("Failed to register class"), TEXT("Error"), MB_OK);
        std::cout << "Couldnt register, json could already be initialized?" << std::endl;
        //return 1;
    }

    int w;
    int h;

    const Json::Value& array1 = actualJson["size"];
    std::cout << array1 << std::endl;
    w = array1["w"].asInt();
    h = array1["h"].asInt();

    std::wstring place2 = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(output);
    const wchar_t* cname2 = place2.c_str();

    HWND hChildWnd = CreateWindow(
        cname,  // Class name
        cname2,      // Window title (can be empty for a pure child)
        WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WM_NCACTIVATE | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,     // Styles (always include WS_CHILD)
        10, 10,                    // X, Y position relative to parent
        w, h,                  // Width and height
        parentWindow,                // Handle to parent window
        NULL,                      // No menu for child windows
        hInst,                 // Application instance
        NULL                       // No additional parameters
    );
}

void AttySymbols::pushText(HDC hdc, Json::Value mainJson, Json::Value apperanceJson, Json::FastWriter fastWriter) {
    auto entriesArray = mainJson["list"];
    auto& objectsArray = mainJson["objects"];
    auto& objectsArray2 = apperanceJson["objects"];
    cout << objectsArray2 << endl;
    //std::cout << objectsArray << std::endl;

    auto txyobj = objectsArray2[0];
    cout << txyobj << endl;

    auto textObj = objectsArray[0];
    std::vector<Json::Value> doc;

    const Json::Value in = mainJson["objects"][0];

    // HYBRID
    bool xys = false;
    int d = entriesArray[0].asInt();
    cout << entriesArray[0].asInt() << endl;
    for (int i = 0; i < d; i++) {
        std::string s = std::to_string(i);
        //Sleep(3);
        if (i == d) {
            
        }
        else {
            std::string output = fastWriter.write(textObj[s]);

            std::wstring place = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(output);

            const wchar_t* cname = place.c_str();

            //cout << txyobj[i] << endl;

            auto x = txyobj[i]["x"].asInt();
            auto y = txyobj[i]["y"].asInt();

            //cout << x << y << endl;

            TextOut(hdc, x, y, cname, output.size());
            xys = true;
            /*
            if (d > 2) {
                if (i == 0) {
                    auto x = txyobj[i]["x"].asInt();
                    auto y = txyobj[i]["y"].asInt();

                    cout << output << endl;

                    TextOut(hdc, x, y, cname, output.size());
                    xys = true;
                }
                else {
                    auto x = txyobj[i]["x"].asInt();
                    auto y = txyobj[i]["y"].asInt();

                    cout << output << endl;

                    TextOut(hdc, x, y, cname, output.size());
                }
            }
            */

        }
        //cout << in[s] << "\n";

    }
    //auto placeholder = objectsArray["1"];

    //std::string sender = firstelem["sender"].asString();
    //int i = std::stoi(sender);


    //std::string output = fastWriter.write(textObj["1"]);

    //std::wstring place = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(output);

    //const wchar_t* cname = place.c_str();

    //TextOut(hdc, 10, 10, cname, output.size());
    std::cout << mainJson["av"] << std::endl;
}