#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>

static const QLatin1String serviceUuid("00001101-0000-1000-8000-00805F9B34FB");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->ptimer = new QTimer;

    //QBluetoothDeviceDiscoveryAgent 这个是指扫描周围蓝牙设备！
    discoveryAgent = new QBluetoothDeviceDiscoveryAgent();
    //QBluetoothLocalDevice 是指配置获取设备的蓝牙状态信息等！
    localDevice = new QBluetoothLocalDevice();
    //QBluetoothSocket指进行链接蓝牙设备，读写信息！
    socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);

    connect(discoveryAgent,
            SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),
            this,
            SLOT(addBlueToothDevicesToList(QBluetoothDeviceInfo))
            );
    connect(ui->list,
            SIGNAL(itemActivated(QListWidgetItem*)),
            this,
            SLOT(itemActivated(QListWidgetItem*))
            );
    connect(socket,
            SIGNAL(readyRead()),
            this,
            SLOT(readBluetoothDataEvent())
            );
    connect(socket,
            SIGNAL(connected()),
            this,
            SLOT(bluetoothConnectedEvent())
            );
    connect(socket,
            SIGNAL(disconnected()),
            this,
            SLOT(bluetoothDisconnectedEvent())
            );

    if( localDevice->hostMode() == QBluetoothLocalDevice::HostPoweredOff ) {
        ui->pushButton_openBluetooth->setEnabled(true);
        ui->pushButton_closeDevice->setEnabled(false);
        localDevice->powerOn();
    }else {
        ui->pushButton_openBluetooth->setEnabled(false);
        ui->pushButton_closeDevice->setEnabled(true);
    }

    if( localDevice->hostMode() == QBluetoothLocalDevice::HostDiscoverable ) {
        ui->checkBox_discoverable->setChecked(true);
    }else {
        ui->checkBox_discoverable->setChecked(false);
    }
    QMessageBox::information(this,tr("welcome"),tr("热烈欢迎您使用 ‘三丫’ 蓝牙助手！"));


}

MainWindow::~MainWindow()
{
    delete ui;
}

/***/
void MainWindow::on_pushButton_scan_clicked()
{
    //discoveryAgent->setInquiryType(QBluetoothDeviceDiscoveryAgent::GeneralUnlimitedInquiry);
    discoveryAgent->start();
    ui->pushButton_scan->setEnabled(false);

}

/***/
void MainWindow::addBlueToothDevicesToList( const QBluetoothDeviceInfo &info )
{
    QString label = QString("%1 %2").arg(info.address().toString()).arg(info.name());

    QList<QListWidgetItem *> items = ui->list->findItems(label, Qt::MatchExactly);

    if (items.empty()) {
        QListWidgetItem *item = new QListWidgetItem(label);
        QBluetoothLocalDevice::Pairing pairingStatus = localDevice->pairingStatus(info.address());
        if (pairingStatus == QBluetoothLocalDevice::Paired || pairingStatus == QBluetoothLocalDevice::AuthorizedPaired )
            item->setTextColor(QColor(Qt::green));
        else
            item->setTextColor(QColor(Qt::black));
        ui->list->addItem(item);
    }

}

/***/
void MainWindow::on_pushButton_openBluetooth_clicked()
{
    localDevice->powerOn();
    ui->pushButton_closeDevice->setEnabled(true);
    ui->pushButton_openBluetooth->setEnabled(false);
    ui->pushButton_scan->setEnabled(true);
}

/***/
void MainWindow::on_pushButton_closeDevice_clicked()
{
    this->ptimer->stop();
    localDevice->setHostMode(QBluetoothLocalDevice::HostPoweredOff);
    ui->pushButton_closeDevice->setEnabled(false);
    ui->pushButton_openBluetooth->setEnabled(true);
    ui->pushButton_scan->setEnabled(false);
}


void MainWindow::itemActivated(QListWidgetItem *item)
{
    QString text = item->text();

    int index = text.indexOf(' ');

    if (index == -1)
        return;

    QBluetoothAddress address(text.left(index));
    QString name(text.mid(index + 1));
    qDebug() << "You has choice the bluetooth address is " << address;
    qDebug() << "The device is connneting.... ";
    QMessageBox::information(this,tr("提示"),tr("设备正在连接中，请稍等！"));
    socket->connectToService(address, QBluetoothUuid(serviceUuid) ,QIODevice::ReadWrite);

}

/***/
void MainWindow::readBluetoothDataEvent()
{

    QByteArray line = socket->readAll();
    //QString strData = line.toHex();
    //comStr.append(strData);
    ui->textBrowser_info->append(line);
    line.clear();
}

/***/
void MainWindow::bluetoothConnectedEvent()
{
    qDebug() << "The android device has been connected successfully!";
    QMessageBox::information(this,tr("提示"),tr("蓝牙连接成功!"));
}

/***/
void MainWindow::bluetoothDisconnectedEvent()
{
    qDebug() << "The android device has been disconnected successfully!";
    QMessageBox::information(this,tr("提示"),tr("successful disconnection!"));
}

void MainWindow::on_pushButton_disconnect_clicked()
{
    socket->disconnectFromService();
    this->ptimer->stop();
}

void MainWindow::on_pushButton_send_clicked()
{
    this->ptimer->stop();
    QByteArray arrayData;
    //QString s("Hello 杨毅远!!!\nThis message is sended via bluetooth of android device!\n");
    QString s = ui->textEdit_send->toPlainText();
    qDebug()<<ui->lineEdit_time->text().toInt();
    arrayData = s.toUtf8();
    if(ui->radioButton_time->isChecked() == true)
    {
        this->num = ui->lineEdit_time->text().toInt();
        this->ptimer->start(num);
        connect(this->ptimer,SIGNAL(timeout()),this,SLOT(Send_Data()));
    }
    socket->write(arrayData);
    s.clear();
    arrayData.clear();
}

void MainWindow::on_pushButton_clear_clicked()
{
    ui->textBrowser_info->clear();
    ui->textEdit_send->clear();
}

void MainWindow::Send_Data()
{
    QByteArray arrayData;
    QString s = ui->textEdit_send->toPlainText();
    arrayData = s.toUtf8();
    socket->write(arrayData);
}
