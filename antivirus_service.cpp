#include <windows.h>
#include <wtsapi32.h>
#include <userenv.h>
#include <iostream>

// Подключаем библиотеки Wtsapi32.lib и Userenv.lib
#pragma comment(lib, "Wtsapi32.lib")
#pragma comment(lib, "Userenv.lib")

// Класс для работы с антивирусной службой
class AntivirusService {
public:
    // Метод для запуска приложения в активных сеансах
    void launchAppInActiveSessions() {
        // Получаем количество сеансов
        DWORD sessionCount;
        if (!ProcessIdToSessionId(GetCurrentProcessId(), &sessionCount))
            return;

        // Проходим по всем сеансам
        for (DWORD sessionId = 0; sessionId < sessionCount; ++sessionId) {
            HANDLE hToken;
            // Получаем токен пользователя для сеанса
            if (WTSQueryUserToken(sessionId, &hToken)) {
                LPVOID lpEnvironment;
                // Создаем окружение для пользователя
                if (CreateEnvironmentBlock(&lpEnvironment, hToken, TRUE)) {
                    // Настраиваем параметры запуска приложения
                    STARTUPINFOW si = { sizeof(si) };
                    PROCESS_INFORMATION pi;
                    // Запускаем процесс от имени пользователя с токеном
                    if (CreateProcessAsUserW(hToken, L"C:\\Users\\kitca\\Documents\\Crate_AV\\app.exe", NULL, NULL, NULL, FALSE, 0, lpEnvironment, NULL, &si, &pi)) {
                        // Закрываем дескрипторы процесса и потока
                        CloseHandle(pi.hProcess);
                        CloseHandle(pi.hThread);
                    }
                    // Уничтожаем окружение пользователя
                    DestroyEnvironmentBlock(lpEnvironment);
                }
                // Закрываем дескриптор токена
                CloseHandle(hToken);
            }
        }
    }
};

// Точка входа в приложение
int main() {
    // Создаем объект службы антивируса
    AntivirusService service;
    // Запускаем приложение в активных сеансах
    service.launchAppInActiveSessions();
    return 0;
}
