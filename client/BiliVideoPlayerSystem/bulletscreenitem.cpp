#include "bulletscreenitem.h"

#include <QHBoxLayout>

BulletScreenInfo::BulletScreenInfo(const QString &userId, int64_t playTime, const QString &text)
    : userId(userId)
    , playTime(playTime)
    , text(text)
{}

BulletScreenItem::BulletScreenItem(QWidget *parent)
    : QFrame{parent}
{
    setObjectName("bulletScreenWnd");
    setFixedHeight(38);
    QHBoxLayout* layout = new QHBoxLayout(this);

    // 头像
    imageLabel = new QLabel(this);
    imageLabel->setFixedSize(30, 30);
    imageLabel->setScaledContents(true);
    imageLabel->hide();

    // 文本
    text = new QLabel(this);
    text->setStyleSheet("font-family: 微软雅黑;"
                        "font-size: 18px;"
                        "color: #FFFFFF;"
                        "border: none;");
    text->adjustSize();

    // 元素添加到水平布局
    layout->addWidget(imageLabel);
    layout->addWidget(text);
    layout->setContentsMargins(10, 4, 19, 4);
    layout->setSpacing(8);
}

void BulletScreenItem::setBulletScreenText(const QString &content)
{
    text->setText(content);
    text->adjustSize();
}

void BulletScreenItem::setBulletScreenIcon(QPixmap &pixmap)
{
    imageLabel->setPixmap(pixmap);
    imageLabel->show();
    setStyleSheet("#bulletScreenWnd{"
                  "border: 1px solid #3ECEFE;"
                  "border-radius: 19px;}");
}

void BulletScreenItem::setBulletScreenAnimal(int x, int duration)
{
    animal = new QPropertyAnimation(this, "pos");
    animal->setDuration(duration);
    animal->setStartValue(QPoint(x, 0));
    animal->setEndValue(QPoint(0 - this->width(), 0));

    connect(animal, &QPropertyAnimation::finished, this, [&]{
        delete this;
    });
}

void BulletScreenItem::startAnimal()
{
    show();
    animal->start();
}
