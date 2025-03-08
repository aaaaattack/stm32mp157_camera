#include "other.h"
#include <stdio.h>
#include <QDebug>
other::other(QWidget *parent) : QWidget(parent)
{
    this->resize(1024, 600);
    this->setMinimumSize(1024, 600);
    printf("sensor ui运行\n");

    // 传感器数据刷新
    ap_timer = new QTimer(this);
    ap_timer->start(100);

    ap_sensor = new QLabel("ap",this);
    ap_sensor->setGeometry(50,50,400, 50);
    gps = new QLabel("gps",this);
    gps->setGeometry(50,110,400, 50);
//    connect(ap_timer, &QTimer::timeout, this, &other::update_ap);
    connect(ap_timer, &QTimer::timeout, this, [this](){
        update_ap();
        updata_gps();
    });
    // back按钮
    QPushButton *back = new QPushButton("back",this);
    back->setGeometry(920,440,80,40);
    connect(back, SIGNAL(clicked()), this, SLOT(on_pushButton_back()));
    // timer开始按钮
//    QPushButton *start = new QPushButton("start",this);
//    start->setGeometry(920,50,80,40);
//    connect(start, SIGNAL(clicked()), this, SLOT(timer_start()));

    // 设备初始化
    ap_init();
//    gps_init();

}

other::~other()
{

}

void other::update_ap()
{
    ap_read_data(data);

//    printf("ir = %d ,als = %d, ps = %d\n", data->ir, data->als, data->ps);
    // 使用 QString::arg() 进行格式化，增加可读性
    QString formattedText = QString("ir: %1 ps: %2 als: %3")
        .arg(data->ir)
        .arg(data->ps)
        .arg(data->als);

    ap_sensor->setText("ap:"+formattedText);
}

void other::on_pushButton_back()
{
    emit other_back_sig();

}

void other::timer_start()
{
//    if (ap_timer) {
//        qDebug() << "ap_timer is initialized!";
//        ap_timer->start(100);
//    } else {
//        qDebug() << "ap_timer is not initialized!";
//    }
}

void other::updata_gps()
{
//    printf("xiewei");

//    int iRet;
//    char c;
//    char buf[1000];
//    char time[100];
//    char Lat[100];
//    char ns[100];
//    char Lng[100];
//    char ew[100];

//    float fLat, fLng;

//    /* 1. open */

//    /* 2. setup
//     * 115200,8N1
//     * RAW mode
//     * return data immediately
//     */

//    /* 3. write and read */





//        /* eg. $GPGGA,082559.00,4005.22599,N,11632.58234,E,1,04,3.08,14.6,M,-5.6,M,,*76"<CR><LF>*/
//        /* read line */
//        iRet = read_gps_raw_data(gps_fd, buf);

//        /* parse line */
//        if (iRet == 0)
//        {
//            iRet = parse_gps_raw_data(buf, time, Lat, ns, Lng, ew);
//        }

//        /* printf */
//        if (iRet == 0)
//        {
//            printf("Time : %s\n", time);
//            printf("ns   : %s\n", ns);
//            printf("ew   : %s\n", ew);
//            printf("Lat  : %s\n", Lat);
//            printf("Lng  : %s\n", Lng);

//            /* 纬度格式: ddmm.mmmm */
//            sscanf(Lat+2, "%f", &fLat);
//            fLat = fLat / 60;
//            fLat += (Lat[0] - '0')*10 + (Lat[1] - '0');

//            /* 经度格式: dddmm.mmmm */
//            sscanf(Lng+3, "%f", &fLng);
//            fLng = fLng / 60;
//            fLng += (Lng[0] - '0')*100 + (Lng[1] - '0')*10 + (Lng[2] - '0');
//            printf("Lng,Lat: %.06f,%.06f\n", fLng, fLat);
//        }

//    printf("xiewei");
    if(num < 100)
    {
        num++;
        gps->setText("gps:"+QString::number(num));
    }
    else {
        num = 0;
    }



}
