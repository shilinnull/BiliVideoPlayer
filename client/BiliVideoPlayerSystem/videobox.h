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
    void setVideoDuration(int64_t duration);	// 设置视频时长

private:
    Ui::VideoBox *ui;
    static PlayerPage* playPage;
    model::VideoInfo videoInfo;					// 保存视频信息
};

#endif // VIDEOBOX_H
