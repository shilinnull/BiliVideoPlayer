#ifndef VIDEOBOX_H
#define VIDEOBOX_H

#include <QWidget>
#include "playerpage.h"
#include "model/data.h"

namespace Ui {
class VideoBox;
}

class VideoBox : public QWidget
{
    Q_OBJECT

public:
    explicit VideoBox(model::VideoInfo videoInfo, QWidget *parent = nullptr);
    ~VideoBox();

    bool eventFilter(QObject* watched, QEvent* event);
    void updateVideoInfoUI();
private:
    void onPlayBtnClicked();
    void setVideoDuration(int64_t duration);            // 设置视频时长
    void setVideoImage(const QString& photoFileId);     // 保存视频封面
protected:
    // 避免图片重叠
    void paintEvent(QPaintEvent *event) override;
private slots:
    void getVideoImageDone(const QString& imageId, QByteArray imageData);
private:
    Ui::VideoBox *ui;
    static PlayerPage* playPage;
    model::VideoInfo videoInfo;					// 保存视频信息
    QPixmap videoCoverImage;                    // 保存封面信息
};

#endif // VIDEOBOX_H
