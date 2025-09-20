#ifndef PLAYERPAGE_H
#define PLAYERPAGE_H

#include <QWidget>
#include <QMouseEvent>

#include "volume.h"

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
    void onVolumeBtnClicked();

private:
    Ui::PlayerPage *ui;
    QPoint dragPos;

    Volume* volume;     // 音量调节窗口
};

#endif // PLAYERPAGE_H
