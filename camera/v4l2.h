#ifndef V4L2_H
#define V4L2_H

#include <QProcess>
#include <QPainter>
#include <QFont>
#include <QPixmap>
#include <QSlider>
#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <QTimer>
#include <QCamera>
#include <QCameraViewfinder>
#include <QMediaRecorder>
#include <QMessageBox>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#include <fcntl.h>
#include <unistd.h>
#include <linux/rtc.h>
#include <time.h>
#include <string.h>

#include <poll.h>
#include <linux/videodev2.h>
#include <linux/fb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <showphoto.h>
#include <QThread>

#include "button.h"

#define		video_width     800
#define 	video_height	600
#define CAMERA_DEV "/dev/video0"
#define CAMERA_FMT AV_PIX_FMT_YUYV422
#define ENCODE_FMT AV_PIX_FMT_YUV420P



class RtcThread;

class v4l2 : public QWidget
{
    Q_OBJECT

public:
    v4l2(QWidget *parent = nullptr);
    ~v4l2();



private slots:
    void on_pushButton_open_clicked();
    void on_pushButton_take_clicked();
    void on_pushButton_photos_clicked();
    void on_pushButton_record_clicked();

private:
    // photo page
    showphoto *photo_page;
    ButtonThread *button_thread;
    RtcThread *rtc_thread;
    struct rtc_time rtc_v4l2;
    // 录制视频相关
    QProcess *video_process;
    QPushButton *startButton;
    QPushButton *stopButton;



    char *userbuff[4];        /* 存放摄像头设备内核缓冲区映射后的用户内存地址 */
    int userbuff_length[4];   /* 保存映射后的数据长度，释放缓存时要用 */

    int show_flag = 1;    /* 1:保存到本地   2:显示在lcd上 */
    int video_fd;

    int lcd_fd;
    int lcd_xres, lcd_yres;
    int lcd_realvirtual;
    int start = 0;

    void camera_open();
    int num = 0;
    QTimer *timer;

    QLabel *label;

    QPushButton *pushButton_open;
    QPushButton *pushButton_take;
    QPushButton *pushbutton_photos;
    QPushButton *back;

    QSlider *bright_Slider;
    int v4l2_open();   //初始化相机参数
    int v4l2_close();
    QPixmap addWatermark(const QPixmap &originalPixmap, const QString &watermarkText);
    QPixmap addWatermark_centre(const QPixmap &originalPixmap, const QString &watermarkText);

signals:
    // v4l2 to menu
    void cam_back_sig();

public slots:
    void video_show();

private slots:
    // v4l2 to menu
    void back_clicked();
    // light
    int bright_adapt(int delta);
    void bright_value_display(int val);
    // photo to v4l2
    void photo_to_v4l2();


    //录制视频
    void startRecording();
    void stopRecording();


};


class RtcThread : public QThread
{
    Q_OBJECT
  public:
    RtcThread(QObject *parent = nullptr){
        Q_UNUSED(parent);
    }
    ~RtcThread();
    void run();
    QMutex rtc_mutex;
    int rtc_fd;
    struct rtc_time rtc;
    int rtc_init(void);
    struct rtc_time* rtc_time_get();
    void print_rtc_time();
};
#endif // V4L2_H
