#include "AttySymbols.h"

using namespace std;

LRESULT AttySymbols::ChildWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    Json::Value actualJson;
    Json::Reader reader;
    Json::FastWriter fastWriter;

    std::ifstream json("currentlyopen.txt");
    std::string mystring;
    getline(json, mystring);

    std::wstring place = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(mystring);

    //childWindows.push_back(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(output));
    char* outputString = new char[place.length() + 1];
    size_t outputSize = place.length() + 1; // +1 for null terminator
    outputString = new char[outputSize]; // we want this
    size_t charsConverted = 0;
    const wchar_t* inputW = place.c_str();
    wcstombs_s(&charsConverted, outputString, outputSize, inputW, place.length());
    std::ifstream njson(outputString);

    if (njson.is_open()) {
        // parse
        reader.parse(njson, actualJson, false);
    }
    else { std::cout << "shit code mate" << std::endl; }

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
        auto entriesArray = actualJson["list"];
        auto objectsArray = actualJson["objects"];
        std::cout << objectsArray << std::endl;
        auto textObj = objectsArray[0];
        //auto placeholder = objectsArray["1"];

        //std::string sender = firstelem["sender"].asString();
        //int i = std::stoi(sender);
        std::cout << "list of objects:" << entriesArray << "\n";
        std::cout << "text objects available:" << entriesArray[0] << "\n";
        std::cout << "? objects available:" << entriesArray[1] << "\n";
        std::cout << "? objects available:" << entriesArray[2] << "\n";
        std::cout << "text object values: " << textObj["1"].asString() << std::endl;




        TextOut(hdc, 10, 10, TEXT("Hello from child!"), 18);
        std::cout << actualJson["av"] << std::endl;
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
    std::ifstream json(ftype);

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
