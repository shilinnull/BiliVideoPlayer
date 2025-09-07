#include "bilivideoplayer.h"
#include "ui_bilivideoplayer.h"

BiliVideoPlayer::BiliVideoPlayer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BiliVideoPlayer)
{
    ui->setupUi(this);

    // 初始化UI
    initUI();
    // 初始化信号和槽
    connectSignalAndSlot();

}

BiliVideoPlayer::~BiliVideoPlayer()
{
    delete ui;
}

void BiliVideoPlayer::initUI()
{
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowIcon(QIcon(":/images/homePage/logo.png"));

    QGraphicsDropShadowEffect* dropShadow = new QGraphicsDropShadowEffect(this);
    dropShadow->setColor(Qt::black);
    dropShadow->setBlurRadius(5);
    dropShadow->setOffset(0, 0);
    // this->setGraphicsEffect(dropShadow); // 这样设置会在程序输出窗口出现参数错误
    ui->biliPlayerBg->setGraphicsEffect(dropShadow); // 应该设置到背景上

    // 初始化按钮状态 && 首页按钮为选中状态
    onSwitchStackedWidgetPage(HomePage);
}

void BiliVideoPlayer::connectSignalAndSlot()
{
    connect(ui->minBtn, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(ui->quitBtn, &QPushButton::clicked, this, &QWidget::close);

    connect(ui->homePageBtn, &pageSwitchButton::switchPage, this, &BiliVideoPlayer::onSwitchStackedWidgetPage);
    connect(ui->myPageBtn, &pageSwitchButton::switchPage, this, &BiliVideoPlayer::onSwitchStackedWidgetPage);
    connect(ui->sysPageBtn, &pageSwitchButton::switchPage, this, &BiliVideoPlayer::onSwitchStackedWidgetPage);
}

void BiliVideoPlayer::onSwitchStackedWidgetPage(int pageId)
{
    ui->stackedWidget->setCurrentIndex(pageId);     // 切换页面
    resetSwitchBtnInfo(pageId);                     // 重新设置按钮上的背景和字体
    repaint();                                      // 保证页面可以立即刷新出来
}

void BiliVideoPlayer::resetSwitchBtnInfo(int pageId)
{
    // 重新设置左侧栏未选中按钮颜⾊
    QList<pageSwitchButton*> switchBtns = findChildren<pageSwitchButton*>();
    for(auto &switchBtn : switchBtns) {
        if(switchBtn->getPageId() != pageId)
            switchBtn->setTextColor("#666666");
        else
            switchBtn->setTextColor("#FF6699"); // 设置选中按钮的文本颜色
    }

    // 根据当前页面ID设置按钮图像
    if (pageId == HomePage) {
        ui->homePageBtn->setImageAndText(":/images/homePage/shouyexuan.png", "首页", HomePage);
        ui->myPageBtn->setImageAndText(":/images/homePage/wode.png", "我的", MyselfPage);
        ui->sysPageBtn->setImageAndText(":/images/homePage/admin.png", "系统", AdminPage);
    } else if (pageId == MyselfPage) {
        ui->homePageBtn->setImageAndText(":/images/homePage/shouye.png", "首页", HomePage);
        ui->myPageBtn->setImageAndText(":/images/homePage/wodexuan.png", "我的", MyselfPage);
        ui->sysPageBtn->setImageAndText(":/images/homePage/admin.png", "系统", AdminPage);
    } else if (pageId == AdminPage) {
        ui->homePageBtn->setImageAndText(":/images/homePage/shouye.png", "首页", HomePage);
        ui->myPageBtn->setImageAndText(":/images/homePage/wode.png", "我的", MyselfPage);
        ui->sysPageBtn->setImageAndText(":/images/homePage/adminxuan.png", "系统", AdminPage);
    } else {
        qDebug() << "暂时不支持页面";
    }
}

void BiliVideoPlayer::mousePressEvent(QMouseEvent *event)
{
    // 检查是否在head区域
    QPoint point = event->position().toPoint();
    if(ui->head->geometry().contains(point)) {
        if(event->button() == Qt::LeftButton) {
            // 记录鼠标按下位置和窗口左上角的相对距离
            dragPos = event->globalPosition().toPoint() - geometry().topLeft();
            return ;
        }
    }
    return QWidget::mousePressEvent(event);
}

void BiliVideoPlayer::mouseMoveEvent(QMouseEvent *event)
{
    QPoint point = event->position().toPoint();
    if(ui->head->geometry().contains(point)) {
        if(event->buttons() == Qt::LeftButton) {
            move(event->globalPosition().toPoint() - dragPos);
            return ;
        }
    }
    return QWidget::mouseMoveEvent(event);
}
