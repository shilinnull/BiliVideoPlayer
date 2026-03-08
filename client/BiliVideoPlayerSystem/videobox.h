#ifndef VIDEOBOX_H
#define VIDEOBOX_H

#include <QByteArray>
#include <QEvent>
#include <QPaintEvent>
#include <QPixmap>
#include <QString>
#include <QWidget>

#include "model/data.h"

class PlayerPage;

namespace Ui {
class VideoBox;
}

class VideoBox : public QWidget
{
    Q_OBJECT

public:
    explicit VideoBox(model::VideoInfo videoInfo, QWidget *parent = nullptr);
    ~VideoBox();

    void updateVideoInfoUI();
    void showMoreBtn(bool isShow);      // 是否显示...按钮
    void setNicknameOfVideoUser(const QString& nickName);// 设置用户昵称后，videoBox中显示的昵称需要同步更改
private:
    void onPlayBtnClicked();
    void setVideoDuration(int64_t duration);            // 设置视频时长
    void setVideoImage(const QString& photoFileId);     // 设置视频封面
    void setUserIcon(const QString& userAvatarId);      // 设置用户头像
protected:
    // 避免图片重叠
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
private slots:
    void getVideoImageDone(const QString& imageId, QByteArray imageData);
    void getUserImageDone(const QString& imageId, QByteArray imageData);
    void getVideoBarrageSuccess(const QString& videoId);
    void onMoreBtnClicked();                        // 点击删除视频按钮
signals:
    void deleteVideo(const QString& videoId);       // 删除视频信号
private:
    Ui::VideoBox *ui;
    static PlayerPage* playPage;
    model::VideoInfo videoInfo;					// 保存视频信息
    QPixmap videoCoverImage;                    // 保存封面信息
    QPixmap userPixmap;                         // 保存用户头像信息
};

#endif // VIDEOBOX_H
