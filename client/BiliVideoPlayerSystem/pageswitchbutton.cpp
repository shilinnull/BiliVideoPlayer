#include "pageswitchbutton.h"

pageSwitchButton::pageSwitchButton(QWidget *parent)
    : QPushButton{parent}
{
    setFixedSize(QSize(48, 46));

    btnImage = new QLabel(this);
    btnImage->setGeometry((48 - 24) / 2, 0, 24, 24);

    btnTittle = new QLabel(this);
    btnTittle->setGeometry(0, 30, 48, 16);

    setStyleSheet("border:none;");
}

void pageSwitchButton::setImageAndText(const QString &imagePath, const QString &text, int pageId)
{
    btnImage->setPixmap(QPixmap(imagePath));
    btnTittle->setText(text);
    this->pageId = pageId;

    btnTittle->move((48 - text.size() * 12) / 2, 30);
}

void pageSwitchButton::setImage(const QString &imagePath)
{
    btnImage->setPixmap(QPixmap(imagePath));
}

void pageSwitchButton::setTextColor(const QString &textColor)
{
    btnTittle->setStyleSheet("font-family: 微软雅黑;"
                            "font-size: 12px;"
                            "font-style: normal;"
                            "font-weight: bold;"
                            "color: " + textColor + ";");

}

int pageSwitchButton::getPageId() const
{
    return this->pageId;
}

void pageSwitchButton::mousePressEvent(QMouseEvent *event)
{
    (void)event;
    emit switchPage(pageId);         // 切换页面
    LOG() << "切换" << pageId << "按钮" << btnTittle->text();
}
