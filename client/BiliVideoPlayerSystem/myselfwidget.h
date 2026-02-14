#ifndef MYSELFWIDGET_H
#define MYSELFWIDGET_H

#include <QPushButton>
#include <QByteArray>
#include <QString>
#include <QWidget>

#include "login.h"

namespace Ui {
class MyselfWidget;
}

class AttentionButton : public QPushButton {
public:
    explicit AttentionButton(QWidget* parent = nullptr);
    void changeStatus(bool isAttention);
    bool isAttentioned()const;
private:
    bool isAttentionStatus = false; // 是否被关注
};


class MyselfWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyselfWidget(QWidget *parent = nullptr);
    ~MyselfWidget();

    void uploadViewBtnClicked();        // 上传视频
    void getMyselfInfo();               // 获取自己的信息
    void getOtherUserInfo(const QString& userId);    // 获取其他用户信息
    void loadMySelf();                  // 加载当前用户信息
    void loadOtherUser(const QString& userId);  // 加载其他用户的信息

private:
    void initUI();
    void connectSignalAndSlots();
    void hideWidget(bool isHide);              // 隐藏界面元素
    void getUserVideoList(const QString& userId, int pageIndex);    // 获取自己视频列表
    void clearVideoList();                      // 清空视频列表
private slots:
    void onUploadAvatarBtnClicked();// 点击上传头像
    void onNicknameBtnClicked();    // 点击用户昵称按钮
    void onSetNickNameDone(const QString& nickName);       // 昵称修改成功
    void onQuitBtnClicked();        // 退出按钮点击
    void settingBtnClicked();       // 点击设置按钮
    void getMyselfInfoDone();       // 用户信息获取成功
    void getOtherUserInfoDone();    // 获取其他用户信息成功
    void getAvatarDone(const QString& fileId, const QByteArray& data);  // 获取用户头像
    void uploadAvatarDone1(const QString& fileId, QWidget* wndPtr);     // 上传图片
    void uploadAvatarDone2();                       // 修改图片
    void getUserVideoListDone(const QString& userId);   // 获取用户视频列表成功
    void onSCrollAreaValueChanged(int value);           // 当QSCrollArea到底部的时候获取下一页视频
    void deleteVideo(const QString& videoId);           // 删除视频操作
    void deleteVideoDone(const QString& videoId);       // 删除视频成功信号槽
    void onAttentionBtnClicked();                       // 关注按钮点击
    void newAttentionDone(const QString& userId);       // 请求关注响应槽函数
    void delAttentionDone(const QString& userId);       // 取消关注响应槽函数
    void logoutDone();                                  // 退出登录成功
signals:
    void switchUploadVideoPage(int pageIndex, const QString& fileName);
private:
    Ui::MyselfWidget *ui;
    Login* login;
    QString userId;     // 标记我的页面是当前用户还是其他用户
};

#endif // MYSELFWIDGET_H
