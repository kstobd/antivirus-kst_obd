#pragma comment(lib, "WtsApi32.lib")
#include <Windows.h>
#include <WtsApi32.h>
#include <sddl.h>
#include <fstream>
#include <format>

#include <string>
#include <thread>

#include <iostream>
#include <sstream>
#define BUFSIZE 512
using namespace std;

std::wofstream errorLog;
WCHAR serviceName[];

SERVICE_STATUS serviceStatus;
SERVICE_STATUS_HANDLE serviceStatusHandle;

// template<typename T>
// void WriteLog(const T& data, std::wstring prefix = L"", size_t position = std::string::npos)
// {
// 	if (!errorLog.is_open())
// 		errorLog.open("C:\\Users\\Алексей\\Desktop\\TestAntivirus\\TestAntivirus\\logs.txt", std::ios::app);
// 	errorLog << prefix << data << std::endl;
// }

// bool Read(HANDLE handle, uint8_t* data, uint64_t length, DWORD& bytesRead)
// {
// 	bytesRead = 0;
// 	BOOL fSuccess = ReadFile(
// 		handle,
// 		data,
// 		length,
// 		&bytesRead,
// 		NULL);
// 	if (!fSuccess || bytesRead == 0)
// 	{
// 		return false;
// 	}
// 	return true;
// }

// bool Write(HANDLE handle, uint8_t* data, uint64_t length)
// {
// 	DWORD cbWritten = 0;
// 	BOOL fSuccess = WriteFile(
// 		handle,
// 		data,
// 		length,
// 		&cbWritten,
// 		NULL);
// 	if (!fSuccess || length != cbWritten)
// 	{
// 		return false;
// 	}
// 	return true;
// }

// std::wstring GetUserSid(HANDLE userToken)
// {
// 	std::wstring userSid;
// 	DWORD err = 0;
// 	LPVOID pvInfo = NULL;
// 	DWORD cbSize = 0;
// 	if (!GetTokenInformation(userToken, TokenUser, NULL, 0, &cbSize))
// 	{
// 		err = GetLastError();
// 		if (ERROR_INSUFFICIENT_BUFFER == err)
// 		{
// 			err = 0;
// 			pvInfo = LocalAlloc(LPTR, cbSize);
// 			if (!pvInfo)
// 			{
// 				err = ERROR_OUTOFMEMORY;
// 			}
// 			else if (!GetTokenInformation(userToken, TokenUser, pvInfo, cbSize, &cbSize))
// 			{
// 				err = GetLastError();
// 			}
// 			else
// 			{
// 				err = 0;
// 				const TOKEN_USER* pUser = (const TOKEN_USER*)pvInfo;
// 				LPWSTR userSidBuf;
// 				ConvertSidToStringSidW(pUser->User.Sid, &userSidBuf);
// 				userSid.assign(userSidBuf);
// 				LocalFree(userSidBuf);
// 			}
// 		}
// 	}
// 	return userSid;
// }

// SECURITY_ATTRIBUTES GetSecurityAttributes(const std::wstring& sddl)
// {
// 	SECURITY_ATTRIBUTES securityAttributes{};
// 	securityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
// 	securityAttributes.bInheritHandle = TRUE;

// 	PSECURITY_DESCRIPTOR psd = nullptr;

// 	if (ConvertStringSecurityDescriptorToSecurityDescriptorW(sddl.c_str(), SDDL_REVISION_1, &psd, nullptr)) {
// 		securityAttributes.lpSecurityDescriptor = psd;
// 	}
// 	return securityAttributes;
// }

void StartUiProcessInSession(DWORD wtsSession)
{
    HANDLE userToken;
    if (WTSQueryUserToken(wtsSession, &userToken))
    {
        WCHAR commandLine[] = L"C:\\Users\\kitca\\Documents\\Crate_AV\\app.exe";
    }
}
// 			// Получение SDDL для процесса и потока, чтобы запускать их от имени пользователя
// 			std::wstring processSddl = std::format(L"O:SYG:SYD:(D;OICI;0x{:08x};;;WD)(A;OICI;0x{:08x};;;WD)",
// 				PROCESS_TERMINATE, PROCESS_ALL_ACCESS);
// 			std::wstring threadSddl = std::format(L"O:SYG:SYD:(D;OICI;0x{:08x};;;WD)(A;OICI;0x{:08x};;;WD)",
// 				THREAD_TERMINATE, THREAD_ALL_ACCESS);

// 			PROCESS_INFORMATION pi{};
// 			STARTUPINFO si{};

// 			// Получение Security Attributes для процесса и потока
// 			SECURITY_ATTRIBUTES psa = GetSecurityAttributes(processSddl);
// 			SECURITY_ATTRIBUTES tsa = GetSecurityAttributes(threadSddl);
// 			if (psa.lpSecurityDescriptor != nullptr &&
// 				tsa.lpSecurityDescriptor != nullptr)
// 			{
// 				// Создание анонимного канала для связи между процессом и службой
// 				std::wstring path = std::format(L"\\\\.\\pipe\\Antivirus");
// 				std::wstring userSid = GetUserSid(userToken);
// 				std::wstring pipeSddl = std::format(L"O:SYG:SYD:(A;OICI;GA;;;{})", userSid);
// 				SECURITY_ATTRIBUTES npsa = GetSecurityAttributes(pipeSddl);
// 				HANDLE pipe = CreateNamedPipeW(
// 					path.c_str(),
// 					PIPE_ACCESS_DUPLEX,
// 					PIPE_TYPE_MESSAGE |
// 					PIPE_READMODE_MESSAGE |
// 					PIPE_WAIT,
// 					1,
// 					BUFSIZE,
// 					BUFSIZE,
// 					0,
// 					&npsa
// 				);

// 				// Запуск процесса от имени пользователя
// 				if (CreateProcessAsUserW(
// 					userToken,
// 					NULL,
// 					commandLine,
// 					&psa,
// 					&tsa,
// 					FALSE,
// 					0,
// 					NULL,
// 					NULL,
// 					&si,
// 					&pi))
// 				{
// 					// Подключение к анонимному каналу
// 					ULONG clientProcessId;
// 					BOOL clientIdentified;
// 					do
// 					{
// 						BOOL fConnected = ConnectNamedPipe(pipe, NULL) ?
// 							TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
// 						clientIdentified = GetNamedPipeClientProcessId(pipe, &clientProcessId);
// 						if (clientIdentified)
// 						{
// 							if (clientProcessId == pi.dwProcessId)
// 							{
// 								break;
// 							}
// 							else
// 							{
// 								DisconnectNamedPipe(pipe);
// 							}
// 						}
// 					} while (true);
//                 }
//             }
//         }
//     }
// }

void WINAPI ServiceMain(DWORD argc, wchar_t** argv)
{
	serviceStatusHandle = RegisterServiceCtrlHandlerExW(serviceName, (LPHANDLER_FUNCTION_EX)ControlHandler, argv[0]);
	if (serviceStatusHandle == (SERVICE_STATUS_HANDLE)0) {
		return;
	}

	serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_SESSIONCHANGE | SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;

	serviceStatus.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(serviceStatusHandle, &serviceStatus);

	PWTS_SESSION_INFOW wtsSessions;
	DWORD sessionsCount;
	if (WTSEnumerateSessionsW(WTS_CURRENT_SERVER_HANDLE, 0, 1, &wtsSessions, &sessionsCount))
	{

		for (DWORD i = 0; i < sessionsCount; ++i)
		{
			auto wtsSession = wtsSessions[i].SessionId;

			if (wtsSession != 0)
			{
				StartUiProcessInSession(wtsSession);
			}
		}
	}
}
