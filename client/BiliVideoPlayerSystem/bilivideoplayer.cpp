#include "bilivideoplayer.h"
#include "ui_bilivideoplayer.h"

#include <QGraphicsDropShadowEffect>
#include <QIcon>

#include "model/datacenter.h"
#include "util.h"

BiliVideoPlayer* BiliVideoPlayer::instance = nullptr;

BiliVideoPlayer *BiliVideoPlayer::getInstance()
{
    if(instance == nullptr) {
        instance = new BiliVideoPlayer();
    }
    return instance;
}

void BiliVideoPlayer::showSystemPageBtn(bool isShow)
{
    if(isShow) {
        ui->sysPageBtn->show();
    } else {
        ui->sysPageBtn->hide();
    }
}

void BiliVideoPlayer::switchToUserInfoPage(const QString &userId)
{
    onSwitchPageUI(MyselfPage);
    // 加载其他用户个人信息
    ui->myPage->loadOtherUser(userId);
}

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

void BiliVideoPlayer::onSwitchPageUI(int pageId)
{
    ui->stackedWidget->setCurrentIndex(pageId);
    resetSwitchBtnInfo(pageId);
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

    // 是否显示系统按钮
    auto dataCenter = model::DataCenter::getInstance();
    auto myself = dataCenter->getMyselfInfo();
    if(myself->isBUser()) {
        ui->sysPageBtn->show();
    } else {
        ui->sysPageBtn->hide();
    }
}

void BiliVideoPlayer::connectSignalAndSlot()
{
    connect(ui->minBtn, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(ui->quitBtn, &QPushButton::clicked, this, [=]{
        auto dataCenter = model::DataCenter::getInstance();
        dataCenter->saveDataFile();     // 保存登录信息 session
        this->close();
    });

    connect(ui->homePageBtn, &PageSwitchButton::switchPage, this, &BiliVideoPlayer::onSwitchStackedWidgetPage);
    connect(ui->myPageBtn, &PageSwitchButton::switchPage, this, &BiliVideoPlayer::onSwitchStackedWidgetPage);
    connect(ui->sysPageBtn, &PageSwitchButton::switchPage, this, &BiliVideoPlayer::onSwitchStackedWidgetPage);

    // 我的页面切换到上传视频页面
    connect(ui->myPage, &MyselfWidget::switchUploadVideoPage, this, [=](int pageId){
        onSwitchStackedWidgetPage(pageId);
    });

    // 视频上传页面切换到我的页面
    connect(ui->uploadVideo, &UploadVideoPage::switchMySelfPage, this, [=](int pageId){
        BiliVideoPlayer::onSwitchStackedWidgetPage(pageId);
    });

    // 切换到上传视频页面
    connect(ui->myPage, &MyselfWidget::switchUploadVideoPage, this,
            [=](int pageId, const QString& videoFilePath){
        LOG() << "由我的页面切换到上传页面";
        onSwitchPageUI(pageId);
        ui->uploadVideo->setVideoTitle(videoFilePath);
    });
}

void BiliVideoPlayer::resetSwitchBtnInfo(int pageId)
{
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
    } else if(pageId == UploadPage){
        ui->homePageBtn->setImageAndText(":/images/homePage/shouye.png", "首页", HomePage);
        ui->myPageBtn->setImageAndText(":/images/homePage/wode.png", "我的", MyselfPage);
        ui->sysPageBtn->setImageAndText(":/images/homePage/admin.png", "系统", AdminPage);
    } else {
        LOG() << "暂时不支持页面";
    }

    // 重新设置左侧栏未选中按钮颜色
    ui->homePageBtn->setTextColor(pageId == HomePage ? "#FF6699" : "#666666");
    ui->myPageBtn->setTextColor(pageId == MyselfPage ? "#FF6699" : "#666666");
    ui->sysPageBtn->setTextColor(pageId == AdminPage ? "#FF6699" : "#666666");
}

void BiliVideoPlayer::onSwitchStackedWidgetPage(int pageId)
{
    ui->stackedWidget->setCurrentIndex(pageId);     // 切换页面
    resetSwitchBtnInfo(pageId);                     // 重新设置按钮上的背景和字体
    if(pageId == MyselfPage) {
        ui->myPage->loadMySelf();                   // 加载用户个人信息
    } else if(pageId == HomePage) {
        ui->homePage->onRefreshBtnClicked();        // 切换到我的页面需要重新添加视频
    }
    repaint();                                      // 保证页面可以立即刷新出来
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
