#include "wirelessdeviceworkthread.h"
#include <QtDebug>
#include <QThread>
#include <QProcess>

WirelessDeviceWorkThread::WirelessDeviceWorkThread()
{
    discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
            this, &WirelessDeviceWorkThread::addBtDevice);
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished,
            this, &WirelessDeviceWorkThread::BtscanFinished);
    discoveryAgent->start();
}

WirelessDeviceWorkThread::~WirelessDeviceWorkThread() {}

void WirelessDeviceWorkThread::run()
{
    qDebug()<<"WirelessDeviceWorkThread is run!";
    while(!isInterruptionRequested())
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
