#ifndef OTHER_H
#define OTHER_H
#include <QTimer>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include "ap_sensor.h"
#include "gps_read.h"

class other : public QWidget
{
    Q_OBJECT
public:
    other(QWidget *parent = nullptr);
    ~other();

private slots:
    void on_pushButton_back();
    void update_ap();
    void updata_gps();
    void timer_start();


private:
    int counter = 0;
    QPushButton *back;
    QPushButton *start;
    QLabel *ap_sensor;
    QLabel *gps;
    QTimer *ap_timer;
    int num = 0;
//    int ap_fd;

    ap3216c_data *data = new ap3216c_data;

signals:
    void other_back_sig();




};

#endif
