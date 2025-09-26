#include "wirelessdeviceworkthread.h"
#include <QtDebug>
#include <QThread>
#include <QProcess>


WirelessDeviceWorkThread::WirelessDeviceWorkThread()
{
    stopRequested = false;

    checkBluetoothReady();
}

WirelessDeviceWorkThread::~WirelessDeviceWorkThread() 
{
    stopRequested = false;
}

void WirelessDeviceWorkThread::stop()
{
    qDebug()<<"WirelessDeviceWorkThread to stop!";
    stopRequested = true;
}

void WirelessDeviceWorkThread::run()
{
    qDebug()<<"WirelessDeviceWorkThread is run!";
    while(!stopRequested)
    {
        wifiList = getWifiList();
        emit RefreshWifiOSD(wifiList);
        QThread::sleep(1);
    }
}

QStringList WirelessDeviceWorkThread::getWifiList(void)
{
    QStringList wifiList;
    QProcess process;
#ifdef OS_WINDOWS
    process.start("netsh", QStringList() << "wlan" << "show" << "network");
    process.waitForFinished();

    QString output = process.readAllStandardOutput();
    QStringList lines = output.split('\n');

    for (const QString &line : lines) {
        if (line.contains("SSID")) {
            QStringList parts = line.split(':');
            if (parts.size() > 1) {
                wifiList.append(parts[1].trimmed());
            }
        }
    }
#else
    process.start("nmcli", QStringList() << "d" << "wifi" << "list");
    process.waitForFinished();
    QTextStream stream(process.readAllStandardOutput());

    while (!stream.atEnd()) {
        QString line = stream.readLine();

        if (line.startsWith("SSID")) continue; // skip table

        QStringList fields = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        //qDebug()<<fields;
        if (fields.size() > 0) {
            wifiList.append(fields[1]); // add SSID
        }
    }
#endif

    return wifiList;
}

void WirelessDeviceWorkThread::addBtDevice(const QBluetoothDeviceInfo &device) {
    //scan found bt device
    QString deviceInfo = QString("%1 [%2]").arg(device.name(), device.address().toString());
    qDebug()<<"Bt device:"<<deviceInfo;
    btList.append(deviceInfo);
}

void WirelessDeviceWorkThread::BtscanFinished() {
    qDebug() << "Scan completed!";
    emit RefreshBtOSD(btList);
}

void WirelessDeviceWorkThread::discoveryError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    qWarning() << "Discovery error:" << discoveryAgent->errorString();
}

void WirelessDeviceWorkThread::startDiscovery()
{
    if (!discoveryAgent) {
        discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
        connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
                this, &WirelessDeviceWorkThread::addBtDevice);
        connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished,
                this, &WirelessDeviceWorkThread::BtscanFinished);
        connect(discoveryAgent, QOverload<QBluetoothDeviceDiscoveryAgent::Error>::of(&QBluetoothDeviceDiscoveryAgent::error),
                this, &WirelessDeviceWorkThread::discoveryError);   
    }
    qDebug()<<"BT startDiscovery!!";
    discoveryAgent->start();
}

void WirelessDeviceWorkThread::checkBluetoothReady()
{
    static int retryCount = 0;

    // 最大重试10次，每次间隔2秒
    if (retryCount++ > 10) {
        qFatal("Bluetooth initialization failed after 10 retries");
        return;
    }

    // 综合检测条件
    bool isReady = QFile::exists("/sys/class/bluetooth/hci0") && 
                  QBluetoothLocalDevice().isValid() &&
                  (QBluetoothLocalDevice().hostMode() != QBluetoothLocalDevice::HostPoweredOff);

    if (isReady) {
        qDebug() << "Bluetooth fully initialized";
        startDiscovery();
    } else {
        qDebug() << "Bluetooth not ready, retrying..." << retryCount;
        QTimer::singleShot(2000, this, &WirelessDeviceWorkThread::checkBluetoothReady);
    }
}