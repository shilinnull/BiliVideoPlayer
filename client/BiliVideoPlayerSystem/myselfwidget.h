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
private slots:
    void uploadAvatarBtnClicked();  // 点击上传头像
    void settingBtnClicked();       // 点击设置按钮
    void getMyselfInfoDone();

signals:
    void switchUploadVideoPage(int pageIndex);
private:
    Ui::MyselfWidget *ui;
};

#endif // MYSELFWIDGET_H
