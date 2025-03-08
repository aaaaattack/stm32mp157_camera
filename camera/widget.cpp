/******************************************************************
Copyright © Deng Zhimao Co., Ltd. 1990-2021. All rights reserved.
* @projectName   appdemo
* @brief         appdemo.cpp
* @author        Deng Zhimao
* @email         1252699831@qq.com
* @net           www.openedv.com
* @date          2021-06-10
*******************************************************************/
#include "widget.h"
#include <QFile>


AppDemo::AppDemo(QWidget *parent)
    : QWidget(parent)
{
    this->resize(1024, 600);
    this->setMinimumSize(1024, 600);

    this->camera_page = new v4l2;
    this->other_page = new other;
    this->priner_page = new printer;
    hotplug_thread = new WorkerThread(this);
    hotplug_thread->start();


//    this->setAttribute(Qt::WA_TranslucentBackground, true); //

    mainWidget = new QWidget(this);
    mainHLayout = new QHBoxLayout(); //水平布局




    for (int i = 0; i < 3; i++) {
        top_pushButton[i] = new QPushButton();
        top_pushButton[i]->setObjectName(tr("top_pushButton%1").arg(QString::number(i)));
        top_pushButton[i]->setFocusPolicy(Qt::NoFocus);
        }







    mainWidget->setLayout(mainHLayout);

    mainHLayout->setAlignment(Qt::AlignCenter);
    mainHLayout->setContentsMargins(0, 0, 0, 0);



    /* 顶部布局 */


    mainHLayout->addWidget(top_pushButton[0]);
    top_pushButton[0]->setFixedSize(300, 500);
    top_pushButton[0]->setText("相机");
    // camera slots
    connect(top_pushButton[0], SIGNAL(clicked()), this, SLOT(camera_clicked()));
    connect(this->camera_page, SIGNAL(cam_back_sig()), this, SLOT(camera_to_menu()));

    mainHLayout->addWidget(top_pushButton[1]);
    top_pushButton[1]->setFixedSize(300, 500);
    top_pushButton[1]->setText("打印机");
    connect(top_pushButton[1], SIGNAL(clicked()), this, SLOT(menu_to_printer()));
    connect(this->priner_page, SIGNAL(printer_back_sig()), this, SLOT(printer_to_menu()));

    mainHLayout->addWidget(top_pushButton[2]);
    top_pushButton[2]->setFixedSize(300, 500);
    top_pushButton[2]->setText("其他");
    connect(top_pushButton[2], SIGNAL(clicked()), this, SLOT(menu_to_other()));
    connect(this->other_page, SIGNAL(other_back_sig()), this, SLOT(other_to_menu()));


    mainHLayout->setContentsMargins(0, 0, 0, 0);

    mainWidget->setLayout(mainHLayout);





}

AppDemo::~AppDemo()
{

}

void AppDemo::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    mainWidget->resize(this->size());
}



void AppDemo::camera_clicked()
{
    this->hide();
    this->camera_page->show();
}

void AppDemo::camera_to_menu()
{
    this->camera_page->hide();
    this->show();
}

void AppDemo::other_to_menu()
{
    this->other_page->hide();
    this->show();
}

void AppDemo::menu_to_other()
{
    this->hide();
    this->other_page->show();
}
void AppDemo::printer_to_menu()
{
    this->priner_page->hide();
    this->show();
}
void AppDemo::menu_to_printer()
{

    this->hide();
    this->priner_page->show();
}
