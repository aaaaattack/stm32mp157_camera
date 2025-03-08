#ifndef PRINTER_H
#define PRINTER_H

#include <QCoreApplication>
#include <QBluetoothDeviceInfo>
#include <QBluetoothSocket>

#include <QBluetoothAddress>
#include <QBluetoothUuid>
#include <QDebug>
#include <QPushButton>

class printer : public QWidget
{
    Q_OBJECT
public:
    printer(QWidget *parent = nullptr);
    ~printer();

private slots:
    void on_pushButton_back();
    void send_msg();
    void on_connected();



private:
    int counter = 0;
    QPushButton *back;
    QPushButton *send;
    QBluetoothSocket *socket;

//    QBluetoothAddress address;
//    QBluetoothUuid uuid;

signals:
    void printer_back_sig();
};


#endif // PRINTER_H
