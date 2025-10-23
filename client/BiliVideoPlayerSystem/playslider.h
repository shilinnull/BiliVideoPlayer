#ifndef PLAYSLIDER_H
#define PLAYSLIDER_H

#include <QWidget>
#include <QMouseEvent>

namespace Ui {
class PlaySlider;
}

class PlaySlider : public QWidget
{
    Q_OBJECT

public:
    explicit PlaySlider(QWidget *parent = nullptr);
    ~PlaySlider();

    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

    void setPlayStep(double stepRatio);
private:
    void moveSlider();

private:
    Ui::PlaySlider *ui;

    int playGrogress;       // 记录当前播放长度
};

#endif // PLAYSLIDER_H
