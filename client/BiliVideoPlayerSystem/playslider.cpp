#include "playslider.h"
#include "ui_playslider.h"

#include <QRect>
#include"util.h"

PlaySlider::PlaySlider(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlaySlider)
{
    ui->setupUi(this);

    playGrogress = 0;
    moveSlider();
}

PlaySlider::~PlaySlider()
{
    delete ui;
}

void PlaySlider::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        playGrogress = event->pos().x();
        moveSlider();
        return ;
    }
    return QWidget::mousePressEvent(event);
}

void PlaySlider::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        playGrogress = event->pos().x();
        moveSlider();
        return ;
    }
    return QWidget::mouseReleaseEvent(event);
}

void PlaySlider::mouseMoveEvent(QMouseEvent *event)
{
    QRect playSliderRect = QRect(0, 0, this->width(), this->height());
    if(!playSliderRect.contains(event->pos())) {
        LOG() << playSliderRect << "<-->" << event->pos();
        return ;
    }
    if(event->buttons() == Qt::LeftButton) {
        playGrogress = event->pos().x();
        // 播放进度不能为负数
        if(playGrogress < 0) {
            playGrogress = 0;
        }
        // 播放进度不能为outLine的最大宽度
        int maxWith = this->width();
        if(playGrogress > maxWith) {
            playGrogress = maxWith;
        }
        moveSlider();
        return ;
    }
    return QWidget::mouseMoveEvent(event);
}

void PlaySlider::moveSlider()
{
    ui->outLine->setGeometry(ui->outLine->x(), ui->outLine->y(), playGrogress, ui->outLine->height());
}
