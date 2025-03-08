#ifndef SHOWPHOTO_H
#define SHOWPHOTO_H
#include <QFileSystemWatcher>
#include <QMainWindow>
#include <QDir>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QThread>
class showphoto : public QMainWindow
{
    Q_OBJECT

public:
    explicit showphoto(QWidget *parent = nullptr);
    ~showphoto();
    void run();
private slots:
    void on_pushButton_back_clicked();

    void on_pushButton_next_clicked();

    void on_pushButton_front_clicked();
    void updateFileList();
    void displayCurrentImage();
private:
    QFileSystemWatcher *fileWatcher;
    QPushButton *pushButton_back;
    QPushButton *pushButton_next;
    QPushButton *pushButton_front;
    QTimer *dir_timer;
    QLabel *label;
    QDir dir; // 目录对象
    QStringList fileList; // 存储文件列表
    int currentIndex = 0; // 当前显示的文件索引
signals:
    void photo_back_sig();
};

#endif // SHOWPHOTO_H
