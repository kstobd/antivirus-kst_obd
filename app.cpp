#include <Windows.h>
#include <shellapi.h>
#include <Commdlg.h>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <cstring>
#include <iterator>
#include <iostream>
#include <cmath>
#include <sstream>
#include <Shlobj.h>
#include <unordered_map>

#define IDM_SHOW_WINDOW 1
#define IDM_EXIT_APP 2
#define IDM_LOAD_DB 3
#define IDM_SCAN_FILE 4
#define IDM_SCAN_FOLDER 5

#define SIGNATURE_LENGTH 256

NOTIFYICONDATAW nid;
HWND mainWindowHandle = NULL;

std::unordered_map<std::string, std::string> signatureTable;

std::vector<std::string> signatures; // Вектор для хранения сигнатур

std::string calculateHash(const std::string &signature)
{
    return signature.substr(0, 8); // Просто берем первые 8 байт
}

// Функция для добавления сигнатуры в таблицу
void addSignatureToTable(const std::string &signature)
{
    std::string hash = calculateHash(signature);
    signatureTable[hash] = signature;
}

// Функция для поиска сигнатуры в таблице
bool searchSignatureInTable(const std::string &signature)
{
    std::string hash = calculateHash(signature);
    return signatureTable.find(hash) != signatureTable.end() && signatureTable[hash] == signature;
}

// Функция для сравнения строк без учета регистра
bool compareIgnoreCase(const std::string &str1, const std::string &str2)
{
    if (str1.length() != str2.length())
        return false;
    for (size_t i = 0; i < str1.length(); ++i)
    {
        if (std::tolower(str1[i]) != std::tolower(str2[i]))
            return false;
    }
    return true;
}

void ShowContextMenu(HWND hwnd, POINT pt)
{
    HMENU hMenu = CreatePopupMenu();
    if (hMenu)
    {
        InsertMenuW(hMenu, -1, MF_BYPOSITION | MF_STRING, IDM_SHOW_WINDOW, L"Показать главное окно");
        InsertMenuW(hMenu, -1, MF_BYPOSITION | MF_STRING, IDM_EXIT_APP, L"Выход из приложения");

        SetForegroundWindow(hwnd);
        TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, NULL);
        DestroyMenu(hMenu);
    }
}

void LoadAVDatabase(HWND hwnd, const std::string &filename);
void ChooseAVDatabase(HWND hwnd);
void ScanFile(const std::string &filename, HWND hwnd);
void ScanFolder(const std::string &folderPath, HWND hwnd);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        // Создаем кнопки "Загрузить AV базу" и "Сканировать файл"
        CreateWindowW(L"BUTTON", L"Загрузить AV базу", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 10, 10, 150, 30, hwnd, (HMENU)IDM_LOAD_DB, NULL, NULL);
        CreateWindowW(L"BUTTON", L"Сканировать файл", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 170, 10, 150, 30, hwnd, (HMENU)IDM_SCAN_FILE, NULL, NULL);
        CreateWindowW(L"BUTTON", L"Сканировать папку", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 330, 10, 150, 30, hwnd, (HMENU)IDM_SCAN_FOLDER, NULL, NULL);

        memset(&nid, 0, sizeof(nid));
        nid.cbSize = sizeof(NOTIFYICONDATAW);
        nid.hWnd = hwnd;
        nid.uID = 1;
        nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        nid.uCallbackMessage = WM_USER + 1;
        nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        lstrcpyW(nid.szTip, L"Антивирус kst_obd");
        Shell_NotifyIconW(NIM_ADD, &nid);
        break;

    case WM_CLOSE:
        // Скрываем окно вместо его закрытия
        ShowWindow(hwnd, SW_HIDE);
        break;

    case WM_DESTROY:
        Shell_NotifyIconW(NIM_DELETE, &nid);
        PostQuitMessage(0);
        break;

    case WM_USER + 1:
        switch (lParam)
        {
        case WM_RBUTTONDOWN:
        case WM_CONTEXTMENU:
        {
            POINT pt;
            GetCursorPos(&pt);
            ShowContextMenu(hwnd, pt);
            break;
        }
        case WM_LBUTTONDOWN:
            ShowWindow(mainWindowHandle, SW_SHOW);
            break;
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDM_LOAD_DB:
            // Вызываем функцию выбора файла базы данных антивируса
            ChooseAVDatabase(hwnd);
            break;
        case IDM_SHOW_WINDOW:
            ShowWindow(mainWindowHandle, SW_SHOW);
            break;
        case IDM_EXIT_APP:
            DestroyWindow(hwnd);
            break;
        case IDM_SCAN_FILE:
        {
            // Открываем диалоговое окно для выбора файла
            OPENFILENAME ofn;
            CHAR szFile[260] = {0};

            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFile = szFile;
            ofn.lpstrFile[0] = '\0';
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = "All Files\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrInitialDir = NULL;
            ofn.lpstrTitle = "Выберите файл для сканирования";
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_DONTADDTORECENT;

            if (GetOpenFileName(&ofn) == TRUE)
            {
                // Вызываем функцию сканирования выбранного файла
                ScanFile(ofn.lpstrFile, hwnd);
            }
            break;
        }
        case IDM_SCAN_FOLDER:
        {
            BROWSEINFOW bi = {0};
            bi.lpszTitle = L"Выберите папку для сканирования";
            LPITEMIDLIST pidl = SHBrowseForFolderW(&bi); // Note the 'W' suffix

            if (pidl != NULL)
            {
                WCHAR path[MAX_PATH];
                if (SHGetPathFromIDListW(pidl, path))
                {
                    char narrowPath[MAX_PATH];
                    WideCharToMultiByte(CP_UTF8, 0, path, -1, narrowPath, MAX_PATH, NULL, NULL);
                    std::string folderPath(narrowPath);
                    // Call the function to scan the folder
                    ScanFolder(folderPath, hwnd);
                }

                CoTaskMemFree(pidl);
            }
            break;
        }
        }
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void replaceAll(std::string &str, const std::string &from, const std::string &to)
{
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

void LoadAVDatabase(HWND hwnd, const std::string &filename)
{
    std::ifstream file(filename, std::ios::binary);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    // if content[]

    if (file.is_open())
    {
        signatures.clear();
        replaceAll(content, "00001010", "\n");

        std::istringstream iss(content);
        std::string signature;
        while (std::getline(iss, signature))
        {
            signatures.push_back(signature);
        }

        for (const auto &sig : signatures)
        {
            addSignatureToTable(sig);
        }

        MessageBoxW(hwnd, L"База данных антивируса загружена успешно.", L"Сообщение", MB_OK);

        // Запись содержимого базы данных в лог-файл
        std::ofstream logFile("log.txt", std::ios::app);
        if (logFile.is_open())
        {
            logFile << "Содержимое базы данных антивируса:" << std::endl;
            for (const auto &sig : signatures)
            {
                logFile << sig << std::endl;
            }
            logFile.close();
        }
        else
        {
            MessageBoxW(hwnd, L"Не удалось записать содержимое базы данных антивируса в лог-файл.", L"Ошибка", MB_OK | MB_ICONERROR);
        }
    }
    else
    {
        MessageBoxW(hwnd, L"Не удалось загрузить базу данных антивируса.", L"Ошибка", MB_OK | MB_ICONERROR);
    }
}

// Функция для выбора файла базы данных антивируса через диалоговое окно
void ChooseAVDatabase(HWND hwnd)
{
    OPENFILENAME ofn;
    CHAR szFile[260] = {0};

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Binary Files\0*.bin\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = "Выберите базу данных антивируса";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_DONTADDTORECENT;

    if (GetOpenFileName(&ofn) == TRUE)
    {
        // Вызываем функцию загрузки базы данных антивируса
        LoadAVDatabase(hwnd, ofn.lpstrFile);
    }
}

std::string decoder(const std::string &filename)
{
    std::ifstream f(filename, std::ios::binary);
    std::string result;
    char ch;

    if (!f)
    {
        std::cerr << "Error opening file!" << std::endl;
        return "";
    }

    while (f.get(ch))
    {
        for (int i = 7; i >= 0; i--)
        {
            result += (((int)ch & (int)std::pow(2, i)) > 0 ? "1" : "0");
        }
    }

    f.close();
    return result;
}

// Функция сканирования файла на наличие сигнатур из базы данных антивируса
void ScanFile(const std::string &filename, HWND hwnd)
{
    std::string ScanningFile = decoder(filename);

    int foundCount = 0;

    for (const auto &signature : signatures)
    {
        if (std::search(ScanningFile.begin(), ScanningFile.end(), signature.begin(), signature.end()) != ScanningFile.end())
        {
            foundCount++;
        }
    }

    // for (const auto &signature : signatures)
    // {
    //     if (searchSignatureInTable(ScanningFile))
    //     {
    //         foundCount++;
    //     }
    // }

    if (foundCount > 0)
    {
        std::wstring message = L"Обнаружено " + std::to_wstring(foundCount) + L" совпадений с базой данных антивируса.";
        MessageBoxW(hwnd, message.c_str(), L"Сообщение", MB_OK | MB_ICONINFORMATION);
    }
    else
    {
        MessageBoxW(hwnd, L"Файл не содержит вредоносных сигнатур.", L"Сообщение", MB_OK | MB_ICONINFORMATION);
    }

    // Запись содержимого сканированного файла в лог-файл
    std::ofstream logFile("log.txt", std::ios::app);
    if (logFile.is_open())
    {
        logFile << "Содержимое сканированного файла:" << std::endl;
        logFile << ScanningFile << std::endl;
        // std::ifstream scannedFile(str);
        // if (scannedFile.is_open())
        // {
        //     std::string line;
        //     while (std::getline(scannedFile, line))
        //     {
        //         logFile << line << std::endl;
        //     }
        //     scannedFile.close();
        // }
        // else
        // {
        //     logFile << "Не удалось открыть сканируемый файл для чтения." << std::endl;
        // }
        logFile.close();
    }
    else
    {
        MessageBoxW(hwnd, L"Не удалось записать содержимое сканированного файла в лог-файл.", L"Ошибка", MB_OK | MB_ICONERROR);
    }
}

void ScanFolder(const std::string &folderPath, HWND hwnd)
{
    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = FindFirstFileW((std::wstring(folderPath.begin(), folderPath.end()) + L"\\*.*").c_str(), &findFileData);

    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                std::wstring filePath = std::wstring(folderPath.begin(), folderPath.end()) + L"\\" + findFileData.cFileName;

                ScanFile(std::string(filePath.begin(), filePath.end()), hwnd);
            }
        } while (FindNextFileW(hFind, &findFileData) != 0);
        FindClose(hFind);
    }
    else
    {
        MessageBoxW(hwnd, L"Не удалось открыть папку для сканирования.", L"Ошибка", MB_OK | MB_ICONERROR);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"TrayWindowClass";

    RegisterClassW(&wc);

    // Создаем главное окно
    mainWindowHandle = CreateWindowExW(0, wc.lpszClassName, L"A", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 550, 100, NULL, NULL, hInstance, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}