#include "printer.h"

printer::printer(QWidget *parent) : QWidget(parent)
{
    this->resize(1024, 600);
    this->setMinimumSize(1024, 600);

    QString adaddress = "A9:1B:31:DF:B7:A5";
    QBluetoothAddress address(adaddress);

    QString aduuid = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
    QBluetoothUuid uuid(aduuid);

    socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol,this);

    back = new QPushButton("back",this);
    back->setGeometry(900,200,40,40);
    connect(back, SIGNAL(clicked()), this,SLOT(on_pushButton_back()));
    send = new QPushButton("send",this);
    send->setGeometry(900,300,40,40);

    connect(send,SIGNAL(clicked()), this, SLOT(send_msg()));
//    qDebug() << "1";
    socket->connectToService(address, uuid);
    connect(socket, &QBluetoothSocket::connected, this, &printer::on_connected);
//    connect(socket, &QBluetoothSocket::connected, this, &printer::send_msg);
//    qDebug() << "2";
//    qDebug() << "3";
//    connect(socket, &QBluetoothSocket::connected, this, &printer::send_msg);
}

printer::~printer()
{

}

void printer::on_connected()
{
    qDebug() << "connect get it\n";
}
void printer::send_msg()
{
    qDebug() << "Connected to device!";
    QString msg = "nihao";
    socket->write(msg.toUtf8());
}

void printer::on_pushButton_back()
{
    emit printer_back_sig();

}
