#include "toast.h"

#include <QTimer>
#include <QVBoxLayout>
#include <QLabel>
#include <QScreen>
#include <QApplication>

Toast::Toast(const QString& text, QWidget *pWdidget)
{
    initUI(text);
    // 2秒之后关闭
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]{
        timer->stop();
        this->close();
        this->deleteLater();
        if(pWdidget) {
            pWdidget->show();
        }
    });
    timer->start(2000);
}

Toast::Toast(const QString &text)
{
    initUI(text);
    // 2秒之后关闭
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]{
        timer->stop();
        this->close();
        this->deleteLater();
    });
}

void Toast::showMessage(const QString &text)
{
    Toast* toast = new Toast(text);
    toast->show();
}

void Toast::showMessage(const QString &text, QWidget *pWidget)
{
    Toast* toast = new Toast(text, pWidget);
    toast->show();
}

void Toast::initUI(const QString &text)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);     // 去除窗口的标题栏
    setAttribute(Qt::WA_TranslucentBackground);             // 设置窗口背景透明
    setFixedSize(800, 60);

    // 实现圆角 文本居中
    QWidget* qWidget = new QWidget(this);
    qWidget->setFixedSize(800, 60);
    qWidget->setStyleSheet("background-color: rgba(102, 102, 102, 0.5);"
                           "border-raidus:4px;");
    QVBoxLayout* layout = new QVBoxLayout(qWidget);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    qWidget->setLayout(layout);

    // 显示文本的label
    QLabel* label = new QLabel();
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("font-family: 微软雅黑;"
                         "font-size: 14px;"
                         "color: #FFFFFF");
    label->setText(text);
    layout->addWidget(label);

    QScreen* screen = QApplication::primaryScreen();
    int width = screen->size().width();
    int height = screen->size().height();
    int x = (width - this->width()) / 2;
    int y = height - this->height() - 100;  // 100 为窗口底边距离屏幕底边的距离
    this->move(x, y);
}
