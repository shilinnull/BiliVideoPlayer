#include "PageSwitchButton.h"
#include "util.h"

PageSwitchButton::PageSwitchButton(QWidget *parent)
    : QPushButton{parent}
{
    setFixedSize(QSize(48, 46));

    btnImage = new QLabel(this);
    btnImage->setGeometry((48 - 24) / 2, 0, 24, 24);

    btnTittle = new QLabel(this);
    btnTittle->setGeometry(0, 30, 48, 16);

    setStyleSheet("border:none;");
}

void PageSwitchButton::setImageAndText(const QString &imagePath, const QString &text, int pageId)
{
    btnImage->setPixmap(QPixmap(imagePath));
    btnTittle->setText(text);
    this->pageId = pageId;

    btnTittle->move((48 - text.size() * 12) / 2, 30);
}

void PageSwitchButton::setImage(const QString &imagePath)
{
    btnImage->setPixmap(QPixmap(imagePath));
}

void PageSwitchButton::setTextColor(const QString &textColor)
{
    btnTittle->setStyleSheet("font-family: 微软雅黑;"
                            "font-size: 12px;"
                            "font-style: normal;"
                            "font-weight: bold;"
                            "color: " + textColor + ";");

}

int PageSwitchButton::getPageId() const
{
    return this->pageId;
}

void PageSwitchButton::mousePressEvent(QMouseEvent *event)
{
    (void)event;
    emit switchPage(pageId);         // 切换页面
    LOG() << "切换" << pageId << "按钮" << btnTittle->text();
}
