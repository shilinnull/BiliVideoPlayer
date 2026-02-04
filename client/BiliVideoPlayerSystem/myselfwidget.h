#ifndef MYSELFWIDGET_H
#define MYSELFWIDGET_H

#include <QWidget>
#include "util.h"

namespace Ui {
class MyselfWidget;
}

class MyselfWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyselfWidget(QWidget *parent = nullptr);
    ~MyselfWidget();

    void uploadViewBtnClicked();        // 上传视频
    void getMyselfInfo();               // 获取自己的信息
    void loadMySelf();                  // 加载当前用户信息

private:
    void initUI();
    void connectSignalAndSlots();
    void hideWidget(bool isHide);              // 隐藏界面元素
    void getUserVideoList(const QString& userId, int pageIndex);    // 获取自己视频列表
    void clearVideoList();                      // 清空视频列表
private slots:
    void uploadAvatarBtnClicked();  // 点击上传头像
    void settingBtnClicked();       // 点击设置按钮
    void getMyselfInfoDone();       // 用户信息获取成功
    void getAvatarDone(const QString& fileId, const QByteArray& data);  // 获取用户头像
    void uploadAvatarDone1(const QString& fileId);  // 上传图片
    void uploadAvatarDone2();                       // 修改图片
    void getUserVideoListDone(const QString& userId);   // 获取用户视频列表成功
    void onSCrollAreaValueChanged(int value);        // 当QSCrollArea到底部的时候获取下一页视频
signals:
    void switchUploadVideoPage(int pageIndex);
private:
    Ui::MyselfWidget *ui;
};

#endif // MYSELFWIDGET_H
