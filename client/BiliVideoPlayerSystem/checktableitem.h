#ifndef CHECKTABLEITEM_H
#define CHECKTABLEITEM_H

#include <QMap>
#include <QPixmap>
#include <QWidget>

#include "model/datacenter.h"
#include "playerpage.h"

namespace Ui {
class CheckTableItem;
}

class CheckTableItem : public QWidget
{
    Q_OBJECT

public:
    explicit CheckTableItem(QWidget *parent, const model::VideoInfo& videoInfo);
    ~CheckTableItem();
private:
    void initStyleSheet();
private slots:
    // 视频按钮点击
    void onVideoBtnClicked();
    // 操作按钮点击
    void onOperatorBtnClicked();
    // 驳回按钮点击
    void onOperatorBtn2Clicked();
    // 获取用户头像成功
    void downloadUserAvatarSuccess(const QString& userId, QByteArray avatarData);

private:
    Ui::CheckTableItem *ui;
    model::VideoInfo videoInfo;             // 保存视频信息
    QMap<QString, QString> styleSheet;      // 保存样式

    PlayerPage* playerPage = nullptr;
    QPixmap userAvatar;
};

#endif // CHECKTABLEITEM_H
