#include "showphoto.h"
#include <v4l2.h>
#include <QDebug>

showphoto::showphoto(QWidget *parent) :
    QMainWindow(parent)
{
    this->resize(1024, 600);
    this->setMinimumSize(1024, 600);
    printf("showphoto ui运行\n");
    // back
    pushButton_back = new QPushButton("back", this);
    pushButton_back->setGeometry(920,440,80,40);
    connect(pushButton_back, SIGNAL(clicked()), this, SLOT(on_pushButton_back_clicked()));

    // next
    pushButton_next = new QPushButton("next",this);
    pushButton_next->setGeometry(920,0,80,40);
    connect(pushButton_next, SIGNAL(clicked()), this, SLOT(on_pushButton_next_clicked()));
    // front
    pushButton_front = new QPushButton("front",this);
    pushButton_front->setGeometry(920,50,80,40);
    connect(pushButton_front, SIGNAL(clicked()), this, SLOT(on_pushButton_front_clicked()));

    label = new QLabel(this);
    label->setGeometry(0,0,900,600);
// 1.定时器方法检查文件是否变化
    dir_timer = new QTimer(this);
    dir.setPath("./photo/"); // 设置目录路径

    connect(dir_timer, &QTimer::timeout, this, [this](){
        dir.setFilter(QDir::Files | QDir::NoDotAndDotDot); // 过滤条件，只获取文件
        dir.setNameFilters(QStringList() << "*.jpg"); // 文件名过滤，只获取jpg文件
        dir.setSorting(QDir::Time | QDir::Reversed);  // 根据修改时间进行排序，逆序排列

        fileList = dir.entryList(); // 获取文件列表
    });
    dir_timer->start(100);
    //先显示一张图片
    if(fileList.length() > 0){
        QString filePath = dir.filePath(fileList[currentIndex]); // 获取文件路径
        QPixmap pixmap(filePath); // 创建图片对象
        label->setPixmap(pixmap); // 在控件上显示图片
        currentIndex++; // 更新当前显示的文件索引
        if (currentIndex >= fileList.size()) {
            currentIndex = 0;
        }
    }
// 2.QFileSystemWatcher方法,bug:不take直接进入image不显示照片
//    dir_timer = new QTimer(this);
//    dir.setPath("./photo/");
//    fileWatcher = new QFileSystemWatcher(this);
//    fileWatcher->addPath(dir.path()); // 监视目录变化

//    connect(fileWatcher, &QFileSystemWatcher::directoryChanged, this, [this]() {
//        updateFileList(); // 更新文件列表
//    });

//    connect(dir_timer, &QTimer::timeout, this, [this](){
//        // 这里可以选择是否需要定时检查
//         updateFileList(); // 可选
//    });
//    dir_timer->start(1000); // 1秒

}

showphoto::~showphoto()
{

}

/* 回到上一个界面 */
void showphoto::on_pushButton_back_clicked()
{
    emit photo_back_sig();
}

/* 下一张图片 */
void showphoto::on_pushButton_next_clicked()
{
    qDebug() << "next" << endl;
    if(fileList.length() > 0){
        QString filePath = dir.filePath(fileList[currentIndex]); // 获取文件路径
        QPixmap pixmap(filePath); // 创建图片对象
        label->setPixmap(pixmap); // 在控件上显示图片
        currentIndex++; // 更新当前显示的文件索引
        if (currentIndex >= fileList.size()) {
            currentIndex = 0;
        }
    }
}

/* 上一张图片 */
void showphoto::on_pushButton_front_clicked()
{
    qDebug() << "front" << endl;
    if(fileList.length() > 0){
        QString filePath = dir.filePath(fileList[currentIndex]); // 获取文件路径
        QPixmap pixmap(filePath); // 创建图片对象
        label->setPixmap(pixmap); // 在控件上显示图片
        currentIndex--; // 更新当前显示的文件索引
        if (currentIndex < 0) {
            currentIndex = fileList.size()-1;
        }
    }
}

// 更新文件列表的函数
void showphoto::updateFileList() {
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    dir.setNameFilters(QStringList() << "*.jpg");
    dir.setSorting(QDir::Time | QDir::Reversed);
    fileList = dir.entryList();
    currentIndex = 0; // 重置索引
    displayCurrentImage(); // 显示当前图片
}
// 显示当前图片的函数
void showphoto::displayCurrentImage() {
    if(fileList.length() > 0) {
        QString filePath = dir.filePath(fileList[currentIndex]);
        QPixmap pixmap(filePath);
        label->setPixmap(pixmap);
        currentIndex = (currentIndex + 1) % fileList.size(); // 循环显示
    }
}
