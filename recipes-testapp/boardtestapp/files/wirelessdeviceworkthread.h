#ifndef WIRELESSDEVICEWORKTHREAD_H
#define WIRELESSDEVICEWORKTHREAD_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QFile>  
#include <QRegularExpression>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QBluetoothLocalDevice>

class WirelessDeviceWorkThread : public QThread
{
    Q_OBJECT

public:
    WirelessDeviceWorkThread();
    ~WirelessDeviceWorkThread();
    void run() override;

    void stop();
signals:
    void RefreshWifiOSD(const QStringList& wifiList);
    void RefreshBtOSD(const QStringList& btList);

private slots:
    void addBtDevice(const QBluetoothDeviceInfo &device);
    void BtscanFinished();
    void discoveryError(QBluetoothDeviceDiscoveryAgent::Error error);
    void checkBluetoothReady();
private:
    bool stopRequested;
    QStringList wifiList;
    QStringList btList;
    QBluetoothDeviceDiscoveryAgent *discoveryAgent = nullptr;

    QStringList getWifiList();
    void startDiscovery();
};
#endif // WIRELESSDEVICEWORKTHREAD_H
