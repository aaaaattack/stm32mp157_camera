#ifndef THREAD_H
#define THREAD_H


#include <QWidget>
#include <QThread>
#include <QDebug>
#include <QPushButton>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <errno.h>
#include <unistd.h>


class WorkerThread : public QThread
{
    /* 用到信号槽即需要此宏定义 */
    Q_OBJECT

public:
    WorkerThread(QObject *parent = nullptr) {
        Q_UNUSED(parent);
    }

    ~WorkerThread();
    int hotplug_sock;
    struct sockaddr_nl addr;
    char buf[2048];
    int len;
    /* 重写run方法，继承QThread的类，只有run方法是在新的线程里 */
    void run();
    void handle_event(char *buf);
    int check_hotplug_sock(void);
signals:
    /* 声明一个信号，译结果准确好的信号 */
    void resultReady(const QString &s);
};
#endif // THREAD_H
