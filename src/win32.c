#include "win32.h"

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>

#define IDM_FILE_NEW 1
#define IDM_FILE_OPEN 2
#define IDM_FILE_SAVE 3
#define IDM_FILE_EXIT 4
#define IDM_EDIT_UNDO 5
#define IDM_EDIT_REDO 6
#define IDM_HELP_ABOUT 7
#define IDM_STATUSBAR 8


void ShowAboutDialog(HWND hwnd);

static char szSavePath[MAX_PATH];
static float scaleFactor = 1.0f;

void OpenTileMapFromFile()
{
    //open file dialog
    OPENFILENAME ofn;
    char szFile[MAX_PATH];
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GetActiveWindow();
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "File type (.txt)\0*.txt\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE)
    {
        MessageBox(GetActiveWindow(), ofn.lpstrFile, "File Path", MB_OK);
    }
}

void SaveTileMapToFile(const char *filename)
{
    // Save file dialog
    OPENFILENAME ofn;
    char szFile[MAX_PATH];
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GetActiveWindow();
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "File type (.txt)\0*.txt\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&ofn) == TRUE)
    {
        // Check if the file extension is provided, if not, append .map
        if (strchr(ofn.lpstrFile, '.') == NULL)
        {
            strcat(ofn.lpstrFile, ".map");
        }

        MessageBox(GetActiveWindow(), ofn.lpstrFile, "File Path", MB_OK);
    }
}

LRESULT CALLBACK AboutDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            DestroyWindow(hwnd);
            return TRUE;
        }
        if (LOWORD(wParam) == 1001)
        {
            MessageBox(hwnd, "Button 1", "Button", MB_OK);
        }
        if (LOWORD(wParam) == 1002)
        {
            MessageBox(hwnd, "Button 2", "Button", MB_OK);
        }
        if (LOWORD(wParam) == 1003)
        {
            CHOOSECOLOR cc;
            static COLORREF acrCustClr[16];
            HBRUSH hbrush;
            static DWORD rgbCurrent; // initial color selection
            // Initialize CHOOSECOLOR
            ZeroMemory(&cc, sizeof(cc));
            cc.lStructSize = sizeof(cc);
            cc.hwndOwner = hwnd;
            cc.lpCustColors = (LPDWORD)acrCustClr;
            cc.rgbResult = rgbCurrent;
            cc.Flags = CC_FULLOPEN | CC_RGBINIT;
            if (ChooseColor(&cc) == TRUE)
            {
                hbrush = CreateSolidBrush(cc.rgbResult);
                rgbCurrent = cc.rgbResult;
            }
        }
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void RegisterAboutDialogClass(HINSTANCE hInstance)
{
    WNDCLASS wc = {0};
    wc.lpfnWndProc = AboutDialogProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "AboutDialogClass";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);
}

void ShowAboutDialog(HWND hwnd)
{
    HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);

    RegisterAboutDialogClass(hInstance);

    // Get DPI scaling factor
    HDC hdc = GetDC(hwnd);
    int dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
    float scaleFactor = dpiX / 96.0f;
    ReleaseDC(hwnd, hdc);

    // Get parent window dimensions
    RECT parentRect;
    GetWindowRect(hwnd, &parentRect);

    // Calculate center position
    int parentWidth = parentRect.right - parentRect.left;
    int parentHeight = parentRect.bottom - parentRect.top;
    int dlgWidth = (int)(300 * scaleFactor);
    int dlgHeight = (int)(200 * scaleFactor);
    int xPos = parentRect.left + (parentWidth - dlgWidth) / 2;
    int yPos = parentRect.top + (parentHeight - dlgHeight) / 2;

    HWND hDlg = CreateWindowEx(
        0,
        "AboutDialogClass",
        "About",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
        xPos, yPos, dlgWidth, dlgHeight,
        hwnd, NULL, hInstance, NULL);

    if (hDlg)
    {
        HFONT hFont = CreateFont(
            (int)(16 * scaleFactor),  // Height
            0,                        // Width
            0,                        // Escapement
            0,                        // Orientation
            FW_NORMAL,                // Weight
            FALSE,                    // Italic
            FALSE,                    // Underline
            FALSE,                    // StrikeOut
            DEFAULT_CHARSET,          // CharSet
            OUT_DEFAULT_PRECIS,       // OutPrecision
            CLIP_DEFAULT_PRECIS,      // ClipPrecision
            DEFAULT_QUALITY,          // Quality
            DEFAULT_PITCH | FF_SWISS, // PitchAndFamily
            "Segoe UI");              // FontName

        // Create and position controls (e.g., static text, buttons)
        HWND hStatic = CreateWindowEx(0, WC_STATIC, "Raylib Application\nVersion 1.0\n\nDeveloped by Your Name",
                                      WS_VISIBLE | WS_CHILD,
                                      (int)(10 * scaleFactor), (int)(10 * scaleFactor), (int)(280 * scaleFactor), (int)(100 * scaleFactor),
                                      hDlg, NULL, hInstance, NULL);
        SendMessage(hStatic, WM_SETFONT, (WPARAM)hFont, TRUE);

        HWND hButtonOK = CreateWindowEx(0, WC_BUTTON, "OK",
                                        WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                                        (int)(50 * scaleFactor), (int)(130 * scaleFactor), (int)(60 * scaleFactor), (int)(25 * scaleFactor),
                                        hDlg, (HMENU)IDOK, hInstance, NULL);
        SendMessage(hButtonOK, WM_SETFONT, (WPARAM)hFont, TRUE);

        HWND hButton1 = CreateWindowEx(0, WC_BUTTON, "Button 1",
                                       WS_VISIBLE | WS_CHILD,
                                       (int)(120 * scaleFactor), (int)(130 * scaleFactor), (int)(60 * scaleFactor), (int)(25 * scaleFactor),
                                       hDlg, (HMENU)1001, hInstance, NULL);
        SendMessage(hButton1, WM_SETFONT, (WPARAM)hFont, TRUE);

        HWND hButton2 = CreateWindowEx(0, WC_BUTTON, "Button 2",
                                       WS_VISIBLE | WS_CHILD,
                                       (int)(190 * scaleFactor), (int)(130 * scaleFactor), (int)(60 * scaleFactor), (int)(25 * scaleFactor),
                                       hDlg, (HMENU)1002, hInstance, NULL);
        SendMessage(hButton2, WM_SETFONT, (WPARAM)hFont, TRUE);

        // Run the dialog message loop
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0))
        {
            if (!IsDialogMessage(hDlg, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        // Clean up the font
        DeleteObject(hFont);
    }
}

// Example Window Procedure to handle menu commands
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDM_FILE_NEW:
            MessageBox(hwnd, "New File", "Menu", MB_OK);
            break;
        case IDM_FILE_OPEN:
            OpenTileMapFromFile();
            break;
        case IDM_FILE_SAVE:
            SaveTileMapToFile("test.map");
            break;
        case IDM_FILE_EXIT:
            PostQuitMessage(0);
            break;
        case IDM_EDIT_UNDO:
            MessageBox(hwnd, "Undo", "Menu", MB_OK);
            break;
        case IDM_EDIT_REDO:
            MessageBox(hwnd, "Redo", "Menu", MB_OK);
            break;
        case IDM_HELP_ABOUT:
            ShowAboutDialog(hwnd);
            break;
        }
        break;
    case WM_KEYDOWN:
        if (GetKeyState(VK_CONTROL) & 0x8000)
        {
            switch (wParam)
            {
            case 'N':
                SendMessage(hwnd, WM_COMMAND, IDM_FILE_NEW, 0);
                break;
            case 'O':
                SendMessage(hwnd, WM_COMMAND, IDM_FILE_OPEN, 0);
                break;
            case 'S':
                SendMessage(hwnd, WM_COMMAND, IDM_FILE_SAVE, 0);
                break;
            case 'Z':
                SendMessage(hwnd, WM_COMMAND, IDM_EDIT_UNDO, 0);
                break;
            case 'Y':
                SendMessage(hwnd, WM_COMMAND, IDM_EDIT_REDO, 0);
                break;
            }
        }
        else if (wParam == VK_F1)
        {
            SendMessage(hwnd, WM_COMMAND, IDM_HELP_ABOUT, 0);
        }
        else if (wParam == VK_F4 && GetKeyState(VK_MENU) & 0x8000)
        {
            SendMessage(hwnd, WM_COMMAND, IDM_FILE_EXIT, 0);
        }
        return CallWindowProc((WNDPROC)GetWindowLongPtr(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
        break;
    case WM_SIZE:
    {
        // Get the new width and height of the window
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);

        // Adjust the status bar position and size
        HWND hStatusBar = FindWindowEx(hwnd, NULL, STATUSCLASSNAME, NULL);
        if (hStatusBar)
        {
            SendMessage(hStatusBar, WM_SIZE, 0, 0);
        }

        // Redraw the window
        InvalidateRect(hwnd, NULL, TRUE);
        UpdateWindow(hwnd);

        // Call Raylib's internal window procedure for unhandled messages
        return CallWindowProc((WNDPROC)GetWindowLongPtr(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        // Call Raylib's internal window procedure for unhandled messages
        return CallWindowProc((WNDPROC)GetWindowLongPtr(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void SetupMenubar()
{
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icex);

    HINSTANCE hInstance = GetModuleHandle(NULL);

    HWND hwnd = GetActiveWindow();

    LONG_PTR originalWndProc = GetWindowLongPtr(hwnd, GWLP_WNDPROC); // Get the original callback from Raylib

    SetWindowLongPtr(hwnd, GWLP_USERDATA, originalWndProc);     // Store the original callback from Raylib
    SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)WindowProc); // Set our callback so we can handle menu commands

    HMENU hMenubar = CreateMenu();
    HMENU hMenuFile = CreatePopupMenu();
    HMENU hMenuEdit = CreatePopupMenu();
    HMENU hMenuHelp = CreatePopupMenu();

    AppendMenu(hMenuFile, MF_STRING, IDM_FILE_NEW, "New\tCtrl+N");
    AppendMenu(hMenuFile, MF_STRING, IDM_FILE_OPEN, "Open\tCtrl+O");
    AppendMenu(hMenuFile, MF_SEPARATOR, 0, NULL);
    AppendMenu(hMenuFile, MF_STRING, IDM_FILE_SAVE, "Save\tCtrl+S");
    AppendMenu(hMenuFile, MF_SEPARATOR, 0, NULL);
    AppendMenu(hMenuFile, MF_STRING, IDM_FILE_EXIT, "Exit\tAlt+F4");

    AppendMenu(hMenuEdit, MF_STRING, IDM_EDIT_UNDO, "Undo\tCtrl+Z");
    AppendMenu(hMenuEdit, MF_STRING, IDM_EDIT_REDO, "Redo\tCtrl+Y");

    AppendMenu(hMenuHelp, MF_STRING, IDM_HELP_ABOUT, "About\tF1");

    AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hMenuFile, "File");
    AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hMenuEdit, "Edit");
    AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hMenuHelp, "Help");

    SetMenu(hwnd, hMenubar);

    // Get DPI scaling factor the Windows way
    HDC hdc = GetDC(hwnd);
    int dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
    scaleFactor = dpiX / 96.0f;
    ReleaseDC(hwnd, hdc);
}

void SetupStatusBar()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    HWND hwnd = GetActiveWindow();

    HWND hStatus = CreateWindowEx(0, STATUSCLASSNAME, NULL,
                                  WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
                                  0, 0, 0, 0,
                                  hwnd, (HMENU)IDM_STATUSBAR, hInstance, NULL);

    int parts[] = {200, -1};
    SendMessage(hStatus, SB_SETPARTS, 2, (LPARAM)parts);
    SendMessage(hStatus, SB_SETTEXT, 0, (LPARAM)"Ready");

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "Windows DPI Scale: %.0f%%", 100 * scaleFactor);
    SendMessage(hStatus, SB_SETTEXT, 1, (LPARAM)buffer);

}

int GetStatusBarHeight(void)
{ 
    HWND hwndMain = GetActiveWindow();// Get the handle to the main window
    HWND hStatusBar = FindWindowEx(hwndMain, NULL, STATUSCLASSNAME, NULL);// Find the status bar window

    if (hStatusBar)
    {
        RECT rect;
        GetWindowRect(hStatusBar, &rect);
        return (rect.bottom - rect.top);// Calculate and return the height, multiplied by the DPI scale factor
    }

    return 0; // If the status bar is not found, return 0
}