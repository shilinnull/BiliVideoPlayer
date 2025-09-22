#ifndef PLAYERPAGE_H
#define PLAYERPAGE_H

#include <QWidget>
#include <QMouseEvent>

#include "volume.h"
#include "playspeed.h"

namespace Ui {
class PlayerPage;
}

class PlayerPage : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerPage(QWidget *parent = nullptr);
    ~PlayerPage();

    void moveWindows(const QPoint& point);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private slots:
    void onVolumeBtnClicked();      // 音量
    void onSpeedBtnClicked();       // 倍速

private:
    Ui::PlayerPage *ui;
    QPoint dragPos;

    Volume* volume;                 // 音量调节
    PlaySpeed* playSpeed;           // 播放速度
};

#endif // PLAYERPAGE_H
