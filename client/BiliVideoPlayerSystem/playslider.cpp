#include "playslider.h"
#include "ui_playslider.h"

#include <QRect>
#include "util.h"

PlaySlider::PlaySlider(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlaySlider)
{
    ui->setupUi(this);

    playProgress = 0;
    moveSlider();
}

PlaySlider::~PlaySlider()
{
    delete ui;
}

void PlaySlider::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        playProgress = event->pos().x();
        moveSlider();
        return ;
    }
    return QWidget::mousePressEvent(event);
}

void PlaySlider::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        playProgress = event->pos().x();
        moveSlider();
        emit setPlayProgress((double)playProgress / ui->inLine->width());
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
        playProgress = event->pos().x();
        // 播放进度不能为负数
        if(playProgress < 0) {
            playProgress = 0;
        }
        // 播放进度不能为outLine的最大宽度
        int maxWith = this->width();
        if(playProgress > maxWith) {
            playProgress = maxWith;
        }
        moveSlider();
        return ;
    }
    return QWidget::mouseMoveEvent(event);
}

void PlaySlider::setPlayStep(double stepRatio)
{
    playProgress = stepRatio * ui->inLine->width();
    LOG() << "播放进度：" << playProgress;
    moveSlider();
}

void PlaySlider::moveSlider()
{
    ui->outLine->setGeometry(ui->outLine->x(), ui->outLine->y(), playProgress, ui->outLine->height());
}
