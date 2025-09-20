#include "SearchLineEdit.h"
#include "util.h"

#include <QHBoxLayout>

SearchLineEdit::SearchLineEdit(QWidget *parent)
    : QLineEdit{parent}
{
    // 搜索框图标
    searchImg = new QLabel(this);
    searchImg->setFixedSize(16, 16);
    searchImg->setPixmap(QPixmap(":/images/homePage/sousuo.png"));
    // 搜索框按钮
    searchBtn = new QPushButton(this);
    searchBtn->setText("搜索");
    searchBtn->setFixedSize(62, 32);
    searchBtn->setCursor(QCursor(Qt::ArrowCursor));
    searchBtn->setStyleSheet("background-color: #FFECF1;" // 搜索按钮背景色
                             "border-radius: 16px;"
                             "font-family: 微软雅黑;"
                             "font-size: 14px;"
                             "color: #222222;"  // 搜索按钮字体颜色
                             "font-style: normal;");
    this->setPlaceholderText("请搜索你感兴趣的视频");
    this->setFixedSize(320, 36);
    this->setObjectName("search");
    this->setStyleSheet("#search{background-color: #FFFFFF;"  // 默认搜索框颜色
                        "border-radius: 18px;"
                        "border: 2px solid #EC5D85;" // 边框
                        "font-family: 微软雅黑;"
                        "font-size : 14px;"
                        "font-style : normal;}");
    this->setTextMargins(33, 0, 0, 0);          // 左上右下

    QHBoxLayout* hLayout = new QHBoxLayout(this);
    hLayout->addWidget(searchImg);
    hLayout->addStretch();
    hLayout->addWidget(searchBtn);
    hLayout->setContentsMargins(11, 0, 2, 0);   // 左上右下

    connect(searchBtn, &QPushButton::clicked, this, &SearchLineEdit::searchBtnClicked); // 按钮点击
    connect(this, &QLineEdit::returnPressed, this, &SearchLineEdit::searchBtnClicked);  // 输入框回车
}

void SearchLineEdit::searchBtnClicked()
{
    LOG() << "搜索按钮点击搜索视频";
}

void SearchLineEdit::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event);

    searchBtn->setStyleSheet("background-color: #EC5D85;" // 搜索按钮背景色
                             "border-radius: 16px;"
                             "font-family: 微软雅黑;"
                             "font-size: 14px;"
                             "color: #FFFFFF;"  // 搜索按钮字体颜色
                             "font-style: normal;");
}

void SearchLineEdit::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);

    searchBtn->setStyleSheet("background-color: #FFECF1;" // 搜索按钮背景色
                             "border-radius: 16px;"
                             "font-family: 微软雅黑;"
                             "font-size: 14px;"
                             "color: #222222;"  // 搜索按钮字体颜色
                             "font-style: normal;");
}
