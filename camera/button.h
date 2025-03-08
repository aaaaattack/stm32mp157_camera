#ifndef BUTTON_H
#define BUTTON_H


#include <QThread>
#include <QMutex>

extern "C" {
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <stdio.h>
    #include <string.h>
}


class ButtonThread : public QThread
{
    /* 用到信号槽即需要此宏定义 */
    Q_OBJECT

public:
    ButtonThread(QThread *parent = nullptr){
        Q_UNUSED(parent);
    };

    ~ButtonThread();

    /* 重写run方法，继承QThread的类，只有run方法是在新的线程里 */
    void run();
    int checkButtonStatus();


private:
    QMutex mutexButtonVal;
//    QMutex onceLock;
//    void initializeSharedResource();
//    int initFlag = 0;
    int button_fd;
    int button_val;
    int pressFlag;

signals:
    /* 声明一个信号，译结果准确好的信号 */
    void buttonPressed();
};


#endif // BUTTON_H
