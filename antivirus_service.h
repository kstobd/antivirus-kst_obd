#ifndef ANTIVIRUS_SERVICE_H
#define ANTIVIRUS_SERVICE_H

#include <windows.h>
#include <userenv.h>
#include <wtsapi32.h>

class AntivirusService {
public:
    AntivirusService();
    ~AntivirusService();
    void start();

private:
    void launchAppInActiveSessions();
};

#endif // ANTIVIRUS_SERVICE_H
