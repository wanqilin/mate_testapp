#ifndef HOTPLUGWORKTHREAD_H
#define HOTPLUGWORKTHREAD_H

#include <QObject>
#include <QThread>
#include <QProcess>
#ifdef OS_UNIX
#include <dirent.h>
#endif
#ifdef OS_WINDOWS
#include <winsock2.h>
#include <windows.h>
#include <QDir>
#include <QFileInfo>
#include <dbt.h>
#include <SetupAPI.h>
#include <initguid.h>
#include <devguid.h>
#include <iphlpapi.h>
#endif

class HotPlugWorkThread : public QThread
{
    Q_OBJECT

public:
    HotPlugWorkThread();
    ~HotPlugWorkThread();
    void run() override;

signals:
    void RefreshUsbOSD(const int usbCnt);
    void RefreshEthOSD(const bool bEthStatus);

private:
    int getUSBDeviceCount();
#ifdef OS_UNIX
    bool isUsbStorage(const std::string &devicePath);
#endif
    int usbCnt;
    void getEthernetStatus();
};
#endif // HOTPLUGWORKTHREAD_H
