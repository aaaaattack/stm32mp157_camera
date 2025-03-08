#include "hotplug.h"


int WorkerThread::check_hotplug_sock()
{
    //创建netlink套接字
    hotplug_sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_KOBJECT_UEVENT);
    if(hotplug_sock < 0)
    {
        perror("hotplug_sock create err \n");
        return -1;
    }

    //初始化地址结构
    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    addr.nl_pid    = getgid();
    addr.nl_groups = 1;

    //绑定套接字
    if(bind(hotplug_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("bind hotplug_sock err \n");
        return -2;
    }

    qDebug() << "listening for hotplug events......\n";

    while (true)
    {
        //接收事件
        len = recv(hotplug_sock, buf, sizeof (buf), 0);
        if(len < 0)
        {
            perror("recv hotplug events ing \n");
            continue;
        }
        buf[len] = '\0';
        handle_event(buf);
    }

    close(hotplug_sock);
    return 0;
}






void WorkerThread::handle_event(char *buf)
{

    QString result = buf;
    while(result.contains("add")||result.contains("remove"))
    {
        result = buf;
        if(result.contains("add"))
        {
            if (result.contains("sda4"))
            {
                 system("mount /dev/sda4 /mnt/gyy");
                 sync();
                 printf("sda4.....mount ok!!...........\n");
            }
            else if (result.contains("sdb4"))
            {
                 system("mount /dev/sdb4 /mnt/gyy");
                 sync();
                 printf("sdb4......mount ok!!.............\n");
            }
            else if (result.contains("sdc4"))
            {
                 system("mount /dev/sdc1 /mnt/gyy");
                 sync();
                 printf("sdc1....mount ok!!................\n");
            }
        }
        else if(result.contains("remove"))
        {
            if (result.contains("sda4"))
            {
                system("umount /dev/sda4");
                sync();
                printf("-----------------------umount sda1-------------------------------\n");
            }
            else if (result.contains("sdb4"))
            {
                system("umount /dev/sdb4");
                sync();
                printf("-----------------------umount sdb4-------------------------------\n");
            }
            else if (result.contains("sdc1"))
            {
                system("umount /dev/sdc1");
                sync();
                printf("-----------------------umount sdc1-------------------------------\n");
            }
        }
        memset(buf, 0, sizeof(*buf));
    }
}

void WorkerThread::run()
{
     QString result = "start OK!!";
     int hotplug_retval;
     hotplug_retval= check_hotplug_sock();
     while(1)
     {
        sleep(1);
        qDebug()<<"thread is Runing!!!!!";
      //  emit resultReady(result);
     }

}


WorkerThread::~WorkerThread()
{
    // 如果线程正在运行，先停止它
    if (isRunning()) {
        requestInterruption();
        quit();
        wait();
    }
}

