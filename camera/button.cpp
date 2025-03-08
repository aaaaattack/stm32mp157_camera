#include "button.h"
#include <QDebug>


ButtonThread::~ButtonThread()
{
    if (button_fd == -1) {
        close(button_fd);
    }
}



int ButtonThread::checkButtonStatus()
{
    button_fd = open("/dev/button0", O_RDWR);
    if (button_fd == -1)
    {
        printf("can not open file /dev/button0\n");

    }



    mutexButtonVal.lock();


    ssize_t bytesRead = read(button_fd, &button_val, 1);
    if (bytesRead == -1) {
        perror("read failed");

    }

    printf("get button : %d\n", button_val);

    if((button_val == 0) && (pressFlag == 1))
    {
        msleep(5);
        emit buttonPressed();




    }
    mutexButtonVal.unlock();

}


void ButtonThread::run()
{
    qDebug() << "Button Thread Is Running \n";
//在这里打开设备,读取设备一次以后,会停在上一次读取的位置
//比如,上一次打开读取为0,就会一直为0
    button_val = 1;// 有时候会打印一个很大的值, 故先强制初始化为0
    pressFlag = 1;
//pressflag初始化为1,当按键按下时,会发送信号
    while(1)
    {
        checkButtonStatus();//
        qDebug() << "running....\n";
        msleep(50);
        pressFlag = button_val;
        //按键按下后 松开前,pressflag会被赋值为0,这样即使读取按键值为0,也不会进入信号发射函数
        //当按键松开,pressflag又变为1,故可以进入发射函数
    }

}
