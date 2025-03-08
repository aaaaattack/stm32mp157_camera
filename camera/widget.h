/******************************************************************
Copyright © Deng Zhimao Co., Ltd. 1990-2021. All rights reserved.
* @projectName   appdemo
* @brief         appdemo.h
* @author        Deng Zhimao
* @email         1252699831@qq.com
* @net           www.openedv.com
* @date          2021-06-10
*******************************************************************/
#ifndef APPDEMO_H
#define APPDEMO_H


#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include "v4l2.h"
#include "other.h"
#include "printer.h"
#include "hotplug.h"

class AppDemo : public QWidget
{
    Q_OBJECT

public:
    AppDemo(QWidget *parent = nullptr);
    ~AppDemo();

private:
    /* 主widget */
    QWidget *mainWidget;



    /* 主布局 */
    QHBoxLayout *mainHLayout;





    /* 顶部按钮，从左到右 */
    QPushButton *top_pushButton[3];



    /* 重设大小 */
    void resizeEvent(QResizeEvent *event);


    v4l2 *camera_page;
    other *other_page;
    printer *priner_page;
    WorkerThread *hotplug_thread;


private slots:

    void camera_clicked();
    void camera_to_menu();
    void menu_to_other();
    void other_to_menu();
    void printer_to_menu();
    void menu_to_printer();
};
#endif // APPDEMO_H
