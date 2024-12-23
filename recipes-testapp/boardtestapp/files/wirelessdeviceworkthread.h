#ifndef WIRELESSDEVICEWORKTHREAD_H
#define WIRELESSDEVICEWORKTHREAD_H

#include <QObject>
#include <QThread>
#include <QRegularExpression>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>

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
private:
    bool stopRequested;
    QStringList wifiList;
    QStringList btList;
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;

    QStringList getWifiList();
};
#endif // WIRELESSDEVICEWORKTHREAD_H
