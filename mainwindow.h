#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
//#include <QtBluetooth/qbluetoothglobal.h>
#include <QtBluetooth/qtbluetoothglobal.h>
#include <QtBluetooth/qbluetoothlocaldevice.h>
#include <qbluetoothaddress.h>
#include <qbluetoothdevicediscoveryagent.h>
#include <qbluetoothlocaldevice.h>
#include <qbluetoothsocket.h>
#include <QTimer>

#define MAX_LENGTH 256

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_pushButton_scan_clicked();
    void addBlueToothDevicesToList(const QBluetoothDeviceInfo&);
    void on_pushButton_openBluetooth_clicked();
    void on_pushButton_closeDevice_clicked();
    void itemActivated(QListWidgetItem *item);
    void readBluetoothDataEvent();
    void bluetoothConnectedEvent();
    void bluetoothDisconnectedEvent();
    void on_pushButton_disconnect_clicked();
    void on_pushButton_send_clicked();

    void on_pushButton_clear_clicked();
    void Send_Data();


private:
    Ui::MainWindow *ui;
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    QBluetoothLocalDevice *localDevice;
    QBluetoothSocket *socket;
    unsigned char comBuffer[15];
    unsigned int  comCount;
    QString comStr;
    QTimer *ptimer;
    int num;
};

#endif // MAINWINDOW_H
