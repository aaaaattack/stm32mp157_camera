#include "v4l2.h"
#include "button.h"
#include <QtGui/QScreen>
#include <QDateTime>
#include <QRandomGenerator>
#include <QDebug>
#include <QPixmap>
#include <QUrl>
#include <QCameraInfo>
#include <QFileDialog>
v4l2::v4l2(QWidget *parent)
    : QWidget(parent)
{
    printf("v4l2 ui运行\n");
    this->resize(1024, 600);
    this->setMinimumSize(1024, 600);

    // photo page
    this->photo_page = new showphoto();
    this->button_thread = new ButtonThread();
    button_thread->start();

    this->rtc_thread = new RtcThread();
    rtc_thread->start();




    // 划条调节亮度
    bright_Slider = new QSlider(Qt::Horizontal,this);
    bright_Slider->setGeometry(870,150, 150, 20);
    bright_Slider->setRange(0,100);
    bright_Slider->setValue(50);

//    connect(bright_Slider, SIGNAL(valueChanged(int)), this, SLOT(bright_adapt(valueChanged(int))));
    connect(bright_Slider, &QSlider::valueChanged, [&](int value) {
        bright_adapt(value);
    });

    //刷帧
    timer = new QTimer();
    connect(timer, &QTimer::timeout, this, [&](){
        video_show();
        rtc_v4l2 = *(rtc_thread->rtc_time_get());
        // gps
    });


    // open
    pushButton_open = new QPushButton("open", this);
    pushButton_open->setGeometry(920,0,100,40);
    connect(pushButton_open, SIGNAL(clicked()), this, SLOT(on_pushButton_open_clicked()));

    // take
    pushButton_take = new QPushButton("take", this);
    pushButton_take->setGeometry(920,50,100,40);
    connect(this->button_thread, SIGNAL(buttonPressed()), this, SLOT(on_pushButton_take_clicked()));
    connect(pushButton_take, SIGNAL(clicked()), this, SLOT(on_pushButton_take_clicked()));


    // photos
    pushbutton_photos = new QPushButton("image", this);
    pushbutton_photos->setGeometry(920,100,100,40);
    connect(pushbutton_photos, SIGNAL(clicked()), this, SLOT(on_pushButton_photos_clicked()));
    connect(this->photo_page, SIGNAL(photo_back_sig()), this, SLOT(photo_to_v4l2()));

    // 设置开始和停止按钮
    startButton = new QPushButton("record", this);
    startButton->setGeometry(900,300,100,40);
    stopButton = new QPushButton("done", this);
    stopButton->setGeometry(900,250,100,40);
    stopButton->setEnabled(false); // 停止按钮一开始不可用
    // 连接信号与槽
    connect(startButton, &QPushButton::clicked, this, &v4l2::startRecording);
    connect(stopButton, &QPushButton::clicked, this, &v4l2::stopRecording);
    // back
    back = new QPushButton("back", this);
    back->setGeometry(920,550,100,40);
    connect(back, SIGNAL(clicked()), this ,SLOT(back_clicked()));


    //打开摄像头显示图像
    camera_open();
    // video label
    label = new QLabel(this);
    label->setGeometry(0,0,900,600);
    video_show();


    // 获取当前目录
    QString currentDir = QDir::currentPath();
    // 拼接photo文件夹路径
    QString photoDirPath = currentDir + "/photo";
    // 创建QDir对象
    QDir photoDir(photoDirPath);
    // 判断photo文件夹是否存在
    if (!photoDir.exists()){
        // 创建photo文件夹
        if (photoDir.mkdir(photoDirPath))
            printf("创建 photo 文件夹成功\n");
        else
            printf("创建 photo 文件夹失败\n");
    }
    else
        printf("photo 文件夹已存在\n");

       video_process = new QProcess(this);

}

v4l2::~v4l2()
{

}

void v4l2::video_show()
{
    QPixmap pix;
    // 格式化时间为字符串
    QString watermark = QString("%1-%2-%3 %4:%5:%6")
               .arg(rtc_v4l2.tm_year + 1900)
               .arg(rtc_v4l2.tm_mon + 1)
               .arg(rtc_v4l2.tm_mday)
               .arg(rtc_v4l2.tm_hour)
               .arg(rtc_v4l2.tm_min)
               .arg(rtc_v4l2.tm_sec);
    // 输出 QString
    qDebug() << watermark; // 输出结果

    /* 采集图片数据 */
    //定义结构体变量，用于获取内核队列数据
    struct v4l2_buffer buffer;
    buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    /* 从内核中捕获好的输出队列中取出一个 */
    if(0 == ioctl(video_fd, VIDIOC_DQBUF, &buffer)){
        /* 显示在label控件上 */
        //获取一帧显示
        pix.loadFromData((unsigned char *)userbuff[buffer.index], buffer.bytesused);
        pix.scaled(label->width(),label->height(),Qt::KeepAspectRatio);
        pix = addWatermark(pix, watermark);
        pix = addWatermark_centre(pix, "Fucking NUIST");
        label->setPixmap(pix);
    }
    /* 将使用后的缓冲区放回到内核的输入队列中 (VIDIOC_QBUF) */
    if(0 > ioctl(video_fd, VIDIOC_QBUF, &buffer)){
        perror("返回队列失败！");
    }
}

//打开 & 设置相机
int v4l2::v4l2_open()
{
    /* 1.打开摄像头设备 /dev/video0 */
    video_fd = open("/dev/video0", O_RDWR);
    if(video_fd < 0){
        perror("打开摄像头设备失败");
        return -1;
    }
    /* 3.获取摄像头的能力 (VIDIOC_QUERYCAP：是否支持视频采集、内存映射等) */
    struct v4l2_capability capability;
    if(0 == ioctl(video_fd, VIDIOC_QUERYCAP, &capability)){
        if((capability.capabilities & V4L2_CAP_VIDEO_CAPTURE) == 0){
            perror("该摄像头设备不支持视频采集！");
            ::close(video_fd);
            return -2;
        }
        if((capability.capabilities & V4L2_MEMORY_MMAP) == 0){
            perror("该摄像头设备不支持mmap内存映射！");
            ::close(video_fd);
            return -3;
        }
    }

    /* 4.枚举摄像头支持的格式           (VIDIOC_ENUM_FMT：MJPG、YUYV等)
      列举出每种格式下支持的分辨率      (VIDIOC_ENUM_FRAMESIZES) */
    struct v4l2_fmtdesc fmtdesc;
    memset(&fmtdesc, 0, sizeof(fmtdesc));
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  //设置视频采集设备类型
    int i = 0;
    while(1){
        fmtdesc.index = i++;
        // 获取支持格式
        if(0 == ioctl(video_fd, VIDIOC_ENUM_FMT, &fmtdesc)){
            printf("支持格式：%s, %c%c%c%c\n", fmtdesc.description,
                                            fmtdesc.pixelformat & 0xff,
                                            fmtdesc.pixelformat >> 8 & 0xff,
                                            fmtdesc.pixelformat >> 16 & 0xff,
                                            fmtdesc.pixelformat >> 24 & 0xff);
            // 列出该格式下支持的分辨率             VIDIOC_ENUM_FRAMESIZES & 默认帧率 VIDIOC_G_PARM
            // 1.默认帧率
            struct v4l2_streamparm streamparm;
            streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            if(0 == ioctl(video_fd, VIDIOC_G_PARM, &streamparm))
                printf("该格式默认帧率 %d fps\n", streamparm.parm.capture.timeperframe.denominator);
            // 2.循环列出支持的分辨率
            struct v4l2_frmsizeenum frmsizeenum;
            frmsizeenum.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            frmsizeenum.pixel_format = fmtdesc.pixelformat;   //设置成对应的格式
            int j = 0;
            printf("支持的分辨率有：\n");
            while(1){
                frmsizeenum.index = j++;
                if(0 == ioctl(video_fd, VIDIOC_ENUM_FRAMESIZES, &frmsizeenum))
                    printf("%d x %d\n", frmsizeenum.discrete.width, frmsizeenum.discrete.height);
                else break;
            }
            printf("\n");
        }else break;
    }

    /* 5.设置摄像头类型为捕获、设置分辨率、视频采集格式 (VIDIOC_S_FMT) */
    struct v4l2_format format;
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;   /* 视频采集 */
    format.fmt.pix.width = video_width;          /* 宽 */
    format.fmt.pix.height = video_height;    	 /* 高 */
    format.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;   /* 设置输出类型：MJPG */
    if(0 > ioctl(video_fd, VIDIOC_S_FMT, &format)){
        perror("设置摄像头参数失败！");
        ::close(video_fd);
        return -4;
    }

    /* 6.向内核申请内存 (VIDIOC_REQBUFS：个数、映射方式为mmap)
         将申请到的缓存加入内核队列 (VIDIOC_QBUF)
         将内核内存映射到用户空间 (mmap) */
    struct v4l2_requestbuffers requestbuffers;
    requestbuffers.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    requestbuffers.count = 4;    //申请缓存个数
    requestbuffers.memory = V4L2_MEMORY_MMAP;     //申请为物理连续的内存空间
    if(0 == ioctl(video_fd, VIDIOC_REQBUFS, &requestbuffers)){
        /* 申请到内存后 */
        for(__u32 i = 0; i < requestbuffers.count; i++){
            /* 将申请到的缓存加入内核队列 (VIDIOC_QBUF)              */
            struct v4l2_buffer buffer;
            buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buffer.index = i;
            buffer.memory = V4L2_MEMORY_MMAP;
            if(0 == ioctl(video_fd, VIDIOC_QBUF, &buffer)){
                /* 加入内核队列成功后，将内存映射到用户空间 (mmap) */
                userbuff[i] = (char *)mmap(NULL, buffer.length, PROT_READ | PROT_WRITE, MAP_SHARED, video_fd, buffer.m.offset);
                userbuff_length[i] = buffer.length;
            }
        }
    }
    else{
        perror("申请内存失败！");
        ::close(video_fd);
        return -5;
    }
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(0 > ioctl(video_fd, VIDIOC_STREAMON, &type)){
        perror("打开视频流失败！");
        return -6;
    }


}

int v4l2::v4l2_close()
{
    /* 8.停止采集，关闭视频流 (VIDIOC_STREAMOFF)
      关闭摄像头设备 & 关闭LCD设备 */
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(0 == ioctl(video_fd, VIDIOC_STREAMOFF, &type)){
        /* 9.释放映射 */
        for(int i = 0; i < 4; i++)
            munmap(userbuff[i], userbuff_length[i]);
        ::close(video_fd);
        printf("关闭相机成功!\n");
        return 0;
    }
    return -1;
}

/* 控制相机打开和关闭 */
void v4l2::on_pushButton_open_clicked()
{
    if(start == 0){
        // 使用v4l2打开 & 设置相机成功
        if(0 == v4l2_open()){
            printf("打开相机成功!\n");
            // 由于摄像头默认30帧每秒,虽然10ms定时执行一次,但实际上1秒内最多有30次可以执行成功
            // 其余都会在ioctl处阻塞
            timer->start(10);
            start = 1;
            pushButton_open->setText("关闭");
        }
    }else{
        if(0 == v4l2_close()){
            start = 0;
            timer->stop();
            pushButton_open->setText("打开");
        }
    }
}

/* 拍照 */
void v4l2::on_pushButton_take_clicked()
{
    QPixmap pix;
    // 格式化时间为字符串
    QString watermark = QString("%1-%2-%3 %4:%5:%6")
               .arg(rtc_v4l2.tm_year + 1900)
               .arg(rtc_v4l2.tm_mon + 1)
               .arg(rtc_v4l2.tm_mday)
               .arg(rtc_v4l2.tm_hour)
               .arg(rtc_v4l2.tm_min)
               .arg(rtc_v4l2.tm_sec);
//    //随机产生10个数字,作为照片的名字
    QString randomNumbers;
    for(int i=0; i<10; i++) {
        int randomNumber = QRandomGenerator::global()->bounded(10);
        randomNumbers.append(QString::number(randomNumber));
    }
    QString str = "./photo/photo_" + randomNumbers + ".jpg";

    /* 采集图片数据 */
    //定义结构体变量，用于获取内核队列数据
    struct v4l2_buffer buffer;
    buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    /* 从内核中捕获好的输出队列中取出一个 */
    if(0 == ioctl(video_fd, VIDIOC_DQBUF, &buffer)){
        //保存到本地
        pix.loadFromData((unsigned char *)userbuff[buffer.index], buffer.bytesused);
        pix.scaled(label->width(),label->height(),Qt::KeepAspectRatio);
        pix = addWatermark(pix, watermark);
        pix = addWatermark_centre(pix, "Fucking NUIST");
        pix.save(str, nullptr, -1);
//        int fd = open(str.toStdString().data(), O_RDWR | O_CREAT, 0777);   //打开并创建一个新文件
//        write(fd, userbuff[buffer.index], buffer.bytesused);
//        printf("%s\n", str.toStdString().data());
//        ::close(fd);
    }
    /* 将使用后的缓冲区放回到内核的输入队列中 (VIDIOC_QBUF) */
    if(0 > ioctl(video_fd, VIDIOC_QBUF, &buffer)){
        perror("返回队列失败！");
    }

    //清空label,相当于拍照效果提示
    label->clear();
}

/* 跳转到 showphoto ui界面 */
void v4l2::on_pushButton_photos_clicked()
{
    if(0 == v4l2_close()){
        timer->stop();
    }
    this->hide();
    this->photo_page->show();
}

// record
void v4l2::on_pushButton_record_clicked()
{
    printf("record\n");
}

// v4l2 to menu
void v4l2::back_clicked()
{
    emit cam_back_sig();
}

// photo_to_v4l2
void v4l2::photo_to_v4l2()
{
    this->photo_page->hide();
    this->show();
    camera_open();
}


void v4l2::bright_value_display(int val)
{
    bright_Slider->setSliderPosition(val);
    label->setText("value:"+QString::number(val));
}


int v4l2::bright_adapt(int delta)
{
    int brightness = 50;
    struct v4l2_queryctrl   qctrl;
    memset(&qctrl, 0, sizeof(qctrl));
    qctrl.id = V4L2_CID_BRIGHTNESS; // V4L2_CID_BASE+0;
    if (0 != ioctl(video_fd, VIDIOC_QUERYCTRL, &qctrl))
    {
        printf("can not query brightness\n");
        return -1;
    }

//    printf("brightness min = %d, max = %d\n", qctrl.minimum, qctrl.maximum);
    //delta = (qctrl.maximum - qctrl.minimum) / 10;

    struct v4l2_control ctl;
    ctl.id = V4L2_CID_BRIGHTNESS; // V4L2_CID_BASE+0;
    ioctl(video_fd, VIDIOC_G_CTRL, &ctl);

    if (ctl.value > qctrl.maximum)
        ctl.value = qctrl.maximum;
    if (ctl.value < qctrl.minimum)
        ctl.value = qctrl.minimum;
    else
        ctl.value = brightness + delta;

    ioctl(video_fd, VIDIOC_S_CTRL, &ctl);

    return -1;
}

void v4l2::camera_open()
{

    if(0 == v4l2_open()){
        printf("打开相机成功!\n");
        // 由于摄像头默认30帧每秒,虽然10ms定时执行一次,但实际上1秒内最多有30次可以执行成功
        // 其余都会在ioctl处阻塞
        timer->start(10);
        start = 1;
        pushButton_open->setText("关闭");
    }
}

QPixmap v4l2::addWatermark(const QPixmap &originalPixmap, const QString &watermarkText) {
    // 创建一个可绘制的 QPixmap，大小与原始图像相同
    QPixmap watermarkedPixmap = originalPixmap; // 复制原始 QPixmap
    QPainter painter(&watermarkedPixmap); // 创建 QPainter 对象

    // 设置字体和颜色
    QFont font("Arial", 20, QFont::Bold); // 设置字体为 Arial，大小为 20，加粗
    painter.setFont(font);
    painter.setPen(Qt::black); // 设置水印颜色为白色
    painter.setOpacity(0.5); // 设置透明度为 50%

    // 计算水印位置（右下角）
    int x = watermarkedPixmap.width() - painter.fontMetrics().width(watermarkText) - 10; // 右边距
    int y = watermarkedPixmap.height() - painter.fontMetrics().height() - 10; // 下边距

    // 在图像上绘制水印
    painter.drawText(x, y, watermarkText);

    painter.end(); // 结束绘制
    return watermarkedPixmap; // 返回带水印的 QPixmap
}

QPixmap v4l2::addWatermark_centre(const QPixmap &originalPixmap, const QString &watermarkText) {
    // 创建一个可绘制的 QPixmap，大小与原始图像相同
    QPixmap watermarkedPixmap = originalPixmap; // 复制原始 QPixmap
    QPainter painter(&watermarkedPixmap); // 创建 QPainter 对象

    // 设置字体和颜色
    QFont font("Arial", 20, QFont::Bold); // 设置字体为 Arial，大小为 20，加粗
    painter.setFont(font);
    painter.setPen(Qt::darkRed); // 设置水印颜色为白色
    painter.setOpacity(0.3); // 设置透明度为 50%

    // 计算水印位置（中间）
    int x = (watermarkedPixmap.width() - painter.fontMetrics().width(watermarkText)) / 2; // 水平居中
    int y = (watermarkedPixmap.height() + painter.fontMetrics().height()) / 2; // 垂直居中

    // 在图像上绘制水印
    painter.drawText(x, y, watermarkText);

    painter.end(); // 结束绘制
    return watermarkedPixmap; // 返回带水印的 QPixmap
}

RtcThread::~RtcThread()
{

}
int RtcThread::rtc_init()
{
    rtc_fd = open("/dev/rtc0", O_RDWR);
    if (rtc_fd < 0) {
        perror("Failed to open RTC device");
        return EXIT_FAILURE;
    }
}

rtc_time *RtcThread::rtc_time_get()
{
    // 读取当前 RTC 时间
    struct rtc_time *rtcTime;
    rtcTime =  &rtc;
    rtc_mutex.lock();
    if (ioctl(rtc_fd, RTC_RD_TIME, &rtc) < 0) {
        perror("Failed to read RTC time");
        ::close(rtc_fd);
    }
//    print_rtc_time();
    rtc_mutex.unlock();
    return rtcTime;


}

void RtcThread::print_rtc_time() {
    printf("RTC Time: %04d-%02d-%02d %02d:%02d:%02d\n",
           rtc.tm_year + 1900, rtc.tm_mon+1, rtc.tm_mday,
           rtc.tm_hour, rtc.tm_min, rtc.tm_sec);
}

void RtcThread::run()
{
    rtc_init();
    while(1)
    {
        rtc_time_get();
        msleep(50);
    }
}
void v4l2::stopRecording()
{

}

void v4l2::startRecording()
{

    // 构造 FFmpeg 命令
    const char *command = "ffmpeg -f v4l2 -i /dev/video0 -c:v libx264 -preset fast -crf 23 -t 10 output.mp4";

    // 执行命令
    int result = system(command);

    // 检查执行结果
    if (result == 0) {
        qDebug() << "录制完成，输出文件为 output.mp4\n";
    } else {
        qDebug() << "录制出错\n";
    }

}

