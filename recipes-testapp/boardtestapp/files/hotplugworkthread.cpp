#include "hotplugworkthread.h"
#include <QtDebug>
#include <QThread>


#ifdef OS_WINDOWS
// define GUID
DEFINE_GUID(GUID_DEVINTERFACE_USB_DEVICE,
            0xA5DCBF10L, 0x6530, 0x11D2, 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED);

// define GUID_DEVINTERFACE_DISK check usb disk
DEFINE_GUID(GUID_DEVINTERFACE_DISK,
            0x53f56307,0xb6bf,0x11d0,0x94,0xf2,0x00,0xa0,0xc9,0x1e,0xfb,0x8b);
#endif


HotPlugWorkThread::HotPlugWorkThread() {}
HotPlugWorkThread::~HotPlugWorkThread() {}

void HotPlugWorkThread::run()
{
    qDebug()<<"HotPlugWorkThread is run!";
    while(!isInterruptionRequested())
    {
        usbCnt = getUSBDeviceCount();
            emit RefreshUsbOSD(usbCnt);
        getEthernetStatus();
        QThread::sleep(1);
    }
}

#ifdef OS_WINDOWS
int HotPlugWorkThread::getUSBDeviceCount() {
    // Get usb device info
    HDEVINFO deviceInfoSet = SetupDiGetClassDevs(
        &GUID_DEVINTERFACE_USB_DEVICE,
        nullptr,
        nullptr,
        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
        );

    if (deviceInfoSet == INVALID_HANDLE_VALUE) {
        qWarning() << "GetDeviceInfo fail";
        return -1;
    }

    DWORD dataType;
    TCHAR buffer[1024];
    DWORD bufferSize = sizeof(buffer);

    // loop device info
    int deviceCount = 0;
    SP_DEVINFO_DATA deviceInfoData;
    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (int i = 0; SetupDiEnumDeviceInfo(deviceInfoSet, i, &deviceInfoData); i++) {
        if (SetupDiGetDeviceRegistryProperty(deviceInfoSet, &deviceInfoData, SPDRP_DEVICEDESC, &dataType, (PBYTE)buffer, bufferSize, nullptr))
        {
            qDebug() << "Device Description: "<< QString::fromWCharArray(buffer);
        }
        deviceCount++;
    }

    // clean device info
    SetupDiDestroyDeviceInfoList(deviceInfoSet);
    return deviceCount;
}
#endif
#ifdef OS_UNIX
bool HotPlugWorkThread::isUsbStorage(const std::string& devicePath) {
std::string ueventPath = "/sys/class/block/" + devicePath + "/device/uevent";    
    FILE* file = fopen(ueventPath.c_str(), "r");
    if (file) {
        char line[256];
        while (fgets(line, sizeof(line), file)) {
            //qDebug() << line;
            if ((strstr(line, "DEVTYPE=scsi_device")!= nullptr)||(strstr(line, "MMC_TYPE=SD")!= nullptr)) {
                fclose(file);
                return true;
            }
        }
        fclose(file);
    }
    return false;
}

int HotPlugWorkThread::getUSBDeviceCount() {
    int usbCount = 0;

    DIR* dir = opendir("/sys/class/block");
    if (!dir) {
        qDebug() << "Failed to open /sys/class/block";
        return -1;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        // exclude "." and ".."
        if (entry->d_name[0] == '.')
            continue;

        std::string deviceName(entry->d_name);

        if (isUsbStorage(deviceName)) {
            usbCount++;
            //qDebug() << "Found USB storage: " << deviceName.c_str();
        }
    }

    closedir(dir);
    return usbCount;
}
#endif

#ifdef OS_WINDOWS
void HotPlugWorkThread::getEthernetStatus(void)
{
    IP_ADAPTER_ADDRESSES *pAdapterAddresses = nullptr;
    ULONG outBufLen = 15000;
    ULONG flags = GAA_FLAG_INCLUDE_GATEWAYS;
    DWORD dwRetVal = GetAdaptersAddresses(AF_UNSPEC, flags, nullptr, pAdapterAddresses, &outBufLen);

    if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
        pAdapterAddresses = (IP_ADAPTER_ADDRESSES *)malloc(outBufLen);
        dwRetVal = GetAdaptersAddresses(AF_UNSPEC, flags, nullptr, pAdapterAddresses, &outBufLen);
    }

    if (dwRetVal == ERROR_SUCCESS) {
        IP_ADAPTER_ADDRESSES *pAdapter = pAdapterAddresses;
        while (pAdapter) {
            //qDebug() << "Adapter name: " << pAdapter->AdapterName;
            //qDebug() << "Description: " << pAdapter->Description;

            if (pAdapter->IfType == IF_TYPE_ETHERNET_CSMACD)
            {
                //qDebug() << "Type: Ethernet (ETHERNET)";
                // check connect
                //qDebug() << "OperStatus: " << pAdapter->OperStatus;
                if (pAdapter->OperStatus == IfOperStatusUp)
                {
                    //qDebug() << "Status: Connected";
                    emit RefreshEthOSD(true);
                }
                else
                {
                    //qDebug() << "Status: Disconnected";
                    emit RefreshEthOSD(false);
                }
            }
            else if (pAdapter->IfType == IF_TYPE_IEEE80211)
            {
                ;//qDebug() << "Type: Wireless (Wi-Fi)";
            }
            else
            {
                ;//qDebug() << "Type: Other";
            }

            pAdapter = pAdapter->Next;
        }
    } else {
        qDebug() << "Error getting adapter information, error code:" << dwRetVal;
    }

    if (pAdapterAddresses) {
        free(pAdapterAddresses);
    }
}
#else // OS_UNIX
void HotPlugWorkThread::getEthernetStatus(void)
{
    // call ifconfig get network inface info 
  
    QProcess process;
    QString program = "ip";
    QStringList arguments = {"link", "show","eth0"};
    process.start(program, arguments);
    process.waitForFinished();
    QString output = process.readAllStandardOutput();

    // check net port
    if (output.contains("wlan"))
    {
        qDebug() << "Network type: Wireless (Wi-Fi)";
    }
    else if (output.contains("eth"))
    {
        //qDebug() << "Network type: Wired (Ethernet)";
        if (output.contains("UP "))
        {
            //qDebug() << "Network is up";
            emit RefreshEthOSD(true);
        }
        else if(output.contains("DOWN "))
        {
            //qDebug() << "Network is down";
            emit RefreshEthOSD(false);
        }
        else
        {
            qDebug() << "Network State:Unknown ";
        }
    }
    else
    {
        qDebug() << "Network type: Unknown";
    }  

}
#endif
