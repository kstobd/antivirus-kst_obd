#include <windows.h>
#include <iostream>
#include <string>

// Прототип функции для запуска приложения
void RunApplication();

// Функция для обработки команд службы
VOID WINAPI ServiceMain(DWORD argc, LPTSTR *argv);

// Глобальные переменные
SERVICE_STATUS        g_ServiceStatus = {0};
SERVICE_STATUS_HANDLE g_StatusHandle = NULL;
HANDLE                g_ServiceStopEvent = INVALID_HANDLE_VALUE;

// Название службы
#define SERVICE_NAME  "MyService"

// Функция для установки статуса службы
VOID WINAPI ServiceCtrlHandler(DWORD);

// Основная функция, которая устанавливает статус и запускает службу
VOID WINAPI ServiceMain(DWORD argc, LPTSTR *argv) {
    DWORD Status = E_FAIL;

    // Установка обработчика для службы
    g_StatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceCtrlHandler);

    if (g_StatusHandle == NULL) {
        return;
    }

    // Устанавливаем статус службы
    g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_ServiceStatus.dwServiceSpecificExitCode = 0;

    // Устанавливаем состояние службы
    g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    SetServiceStatus(g_StatusHandle, &g_ServiceStatus);

    // Запускаем приложение
    RunApplication();

    // Устанавливаем конечный статус службы
    g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(g_StatusHandle, &g_ServiceStatus);
}

// Функция для обработки команд службы
VOID WINAPI ServiceCtrlHandler(DWORD CtrlCode) {
    switch(CtrlCode) {
        case SERVICE_CONTROL_STOP:
            if (g_ServiceStatus.dwCurrentState != SERVICE_RUNNING)
                break;
            g_ServiceStatus.dwControlsAccepted = 0;
            g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
            g_ServiceStatus.dwWin32ExitCode = 0;
            g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
            SetServiceStatus(g_StatusHandle, &g_ServiceStatus);
            break;
        default:
            break;
    }
}

// Функция для запуска приложения
void RunApplication() {
    // Путь к приложению, которое нужно запустить
    std::wstring appPath = L"C:\\Users\\kitca\\Documents\\Create_AV\\app.exe";
    
    // Создаем процесс
    STARTUPINFOW si = {0};
    PROCESS_INFORMATION pi = {0};
    BOOL success = CreateProcessW(NULL, const_cast<LPWSTR>(appPath.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    
    if (success) {
        // Ждем завершения процесса
        WaitForSingleObject(pi.hProcess, INFINITE);
        
        // Закрываем хендлы процесса и потока
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        std::cerr << "Failed to start application!" << std::endl;
    }
}

// Основная функция
int main() {
    SERVICE_TABLE_ENTRY ServiceTable[] = {
        {SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain},
        {NULL, NULL}
    };

    // Регистрируем службу
    if (StartServiceCtrlDispatcher(ServiceTable) == FALSE) {
        return GetLastError();
    }

    return 0;
}
