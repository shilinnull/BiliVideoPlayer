#include "playerpage.h"
#include "ui_playerpage.h"

#include "util.h"
#include "login.h"
#include "toast.h"
#include "volume.h"
#include <QShortcut>
#include <QKeySequence>

PlayerPage::PlayerPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlayerPage)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Bili视频播放器");
    setWindowIcon(QIcon(":/images/homePage/logo.png"));

    volume = new Volume(this);						// 音量
    playSpeed = new PlaySpeed(this);				// 倍速
    mpvPlayer = new MpvPlayer(ui->screen, this);	// 视频

    initBarrageArea();								// 初始化弹幕布局

    // 设置快捷键空格
    QShortcut* shortCut = new QShortcut(ui->playBtn);
    QKeySequence keySqeuence(" ");
    shortCut->setKey(keySqeuence);
    connect(shortCut, &QShortcut::activated, this, [=]{
        ui->playBtn->animateClick();
    });

    connect(ui->minBtn, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(ui->quitBtn, &QPushButton::clicked, this, &QWidget::close);
    connect(ui->volumeBtn, &QPushButton::clicked, this, &PlayerPage::onVolumeBtnClicked);
    connect(ui->speedBtn, &QPushButton::clicked, this, &PlayerPage::onSpeedBtnClicked);
    connect(ui->likeImageBtn, &QPushButton::clicked, this, &PlayerPage::onLikeImageBtnClicked);
    connect(ui->playBtn, &QPushButton::clicked, this, &PlayerPage::onplayBtnClicked);
    connect(playSpeed, &PlaySpeed::setPlaySpeed, this, &PlayerPage::onPlaySpeedChanged);
    connect(volume, &Volume::setVolume, this, &PlayerPage::setVolume);
    connect(ui->videoSlider, &PlaySlider::setPlayProgress, this, &PlayerPage::setPlayProgress);
    connect(mpvPlayer, &MpvPlayer::playPositionChanged, this, &PlayerPage::onPlayPositionChanged);
    connect(ui->bulletScreenBtn, &QPushButton::clicked, this, &PlayerPage::onBulletScreenClicked);
    connect(ui->bulletScreenText, &BarrageEdit::onSendScreenBtn, this, &PlayerPage::onSendBulletScreenBtnClicked);

    loadBulletScreenData();
}

PlayerPage::~PlayerPage()
{
    delete ui;
    delete mpvPlayer;
}

void PlayerPage::moveWindows(const QPoint &point)
{
    // 13 是音量调节窗口右边界和PlayerPage右边界的距离
    QPoint newPoint = point + QPoint(this->width() - volume->width() - 13, 533);
    volume->move(newPoint);

    newPoint = point + QPoint(this->width() - playSpeed->width() - 72, 563);
    playSpeed->move(newPoint);
}

void PlayerPage::startPlaying(const QString &videoFilePath)
{
    this->videoFilePath = videoFilePath;
    mpvPlayer->startPlay(videoFilePath);	// 加载视频

    // 视频加载后会立即播放，默认需要先暂停，让用户点击播放按钮进行播放
    mpvPlayer->pause();
}

void PlayerPage::loadBulletScreenData()
{
    QList<BulletScreenInfo> bulletScreenList;
    for(int i = 0; i < 3; i++) {
        BulletScreenInfo bsItem("10001", i + 1, "我是弹幕" + QString::number(i));
        bulletScreenList.append(bsItem);
        bulletScreenLists.insert(bsItem.playTime, bulletScreenList);
        bulletScreenList.clear();
    }
    for(int i = 0; i < 4; i++) {
        BulletScreenInfo bsItem("10001", 5, "我是弹幕" + QString::number(i));
        bulletScreenList.append(bsItem);
    }
    bulletScreenLists.insert(bulletScreenList[0].playTime, bulletScreenList);
}

void PlayerPage::showBulletScreen()
{
    if(!isStartBS) return ;

    // 通过时间获取弹幕数据
    QList<BulletScreenInfo> bulletScreenList = bulletScreenLists.value(playTime);
    // 显示弹幕
    int xTop, xMid, xBottom;
    xTop = xMid = xBottom = top->width();
    BulletScreenItem* bs = nullptr;
    for(int i = 0; i < bulletScreenList.size(); i++) {
        BulletScreenInfo& bsInfo = bulletScreenList[i];
        if(0 == i % 3) {
            // 显示第一行
            bs = new BulletScreenItem(top);
            bs->setBulletScreenText(bsInfo.text);
            int duration = 10000 * xTop / (double)(30 * 18 + 1450);
            bs->setBulletScreenAnimal(xTop, duration);
            xTop += bs->width() + 18 * 4;		// 同一行隔四个汉字，18为每个汉字的大小
        } else if(1 == i % 3) {
            // 显示第二行
            bs = new BulletScreenItem(middle);
            bs->setBulletScreenText(bsInfo.text);
            int duration = 10000 * xMid/ (double)(30 * 18 + 1450);
            bs->setBulletScreenAnimal(xMid, duration);
            xMid += bs->width() + 18 * 4;		// 同一行隔四个汉字，18为每个汉字的大小
        } else {
            // 显示第三行
            bs = new BulletScreenItem(bottom);
            bs->setBulletScreenText(bsInfo.text);
            int duration = 10000 * xBottom / (double)(30 * 18 + 1450);
            bs->setBulletScreenAnimal(xBottom + 2 * 18, duration);
            xBottom += bs->width() + 18 * 4;		// 同一行隔四个汉字，18为每个汉字的大小
        }
        bs->startAnimal();
    }
}

void PlayerPage::mousePressEvent(QMouseEvent *event)
{
    QPoint point = event->position().toPoint();
    if(ui->playHead->geometry().contains(point)) {
        if(event->button() == Qt::LeftButton) {
            dragPos = event->globalPosition().toPoint() - this->geometry().topLeft();
            return ;
        }
    }
    return QWidget::mousePressEvent(event);
}

void PlayerPage::mouseMoveEvent(QMouseEvent *event)
{
    QPoint point = event->position().toPoint();
    if(ui->playHead->geometry().contains(point)) {
        if(event->buttons() == Qt::LeftButton) {
            this->move(event->globalPosition().toPoint() - dragPos);
            QPoint point = geometry().topLeft();
            point.setY(point.ry() + 60);
            barrageArea->move(point);
            return ;
        }
    }
    return QWidget::mouseMoveEvent(event);
}

void PlayerPage::onVolumeBtnClicked()
{
    moveWindows(mapToGlobal(QPoint(0, 0)));
    volume->show();
}

void PlayerPage::onSpeedBtnClicked()
{
    moveWindows(mapToGlobal(QPoint(0, 0)));
    playSpeed->show();
}

void PlayerPage::onLikeImageBtnClicked()
{
    // 检测用户是否登录，登录才能点赞
    Login* login = new Login();
    // login->show();
    Toast::showMessage("先登录，登录后才能点赞", login);
}

void PlayerPage::onplayBtnClicked()
{
    isPlay = !isPlay;
    if(isPlay) {
        // 播放
        ui->playBtn->setStyleSheet("border-image: url(:/images/PlayPage/bofang.png)");
        if(mpvPlayer) mpvPlayer->play();
        barrageArea->show();	// 弹幕显示
    } else {
        // 暂停
        ui->playBtn->setStyleSheet("border-image: url(:/images/PlayPage/zanting.png)");
        if(mpvPlayer) mpvPlayer->pause();
        barrageArea->hide();	// 弹幕隐藏
    }

    // 播放完毕后再次点击播放重新开始播放
    if(playTime >= 10 && isPlay) {
        this->playTime = 0;
        startPlaying(videoFilePath);
        mpvPlayer->play();
    }
}

void PlayerPage::onPlaySpeedChanged(double speed)
{
    if(mpvPlayer) mpvPlayer->setPlaySpeed(speed);
}

void PlayerPage::setVolume(int volumeRatio)
{
    if(mpvPlayer) mpvPlayer->setVolume(volumeRatio);
}

void PlayerPage::onPlayPositionChanged(int64_t playTime)
{
    LOG() << playTime;
    // 先测试10秒的视频时长 后续需要到服务器进行获取
    this->playTime = playTime;
    QString curPlayTime = secondToTime(this->playTime);
    QString totalTime = secondToTime(10);
    ui->videoDuration->setText(curPlayTime + "/" + totalTime);

    // 修改进度条
    ui->videoSlider->setPlayStep((double)this->playTime / 10);

    if(this->playTime == 10) {
        isPlay = false;
        ui->playBtn->setStyleSheet("border-image: url(:/images/PlayPage/zanting.png)");
    }

    showBulletScreen();		// 需要随着播放时间的继续，需要实时更新弹幕
}

void PlayerPage::setPlayProgress(double playRatio)
{
    // 更新播放时间
    playTime = 10 * playRatio;
    mpvPlayer->setCurrentPlayPositon(playTime);
}

void PlayerPage::onBulletScreenClicked()
{
    isStartBS = !isStartBS;
    if(isStartBS) {
        ui->bulletScreenBtn->setStyleSheet("border-image: url(:/images/PlayPage/danmu.png);");
        barrageArea->show();
    } else {
        ui->bulletScreenBtn->setStyleSheet("border-image: url(:/images/PlayPage/danmuguan.png);");
        barrageArea->hide();
    }
}

void PlayerPage::onSendBulletScreenBtnClicked(const QString &text)
{
    // 如果用户没有登录需要先登录才能发评论
    // Toast::showMessage("请先登录才能发评论");
    if(!isStartBS) return ;

    BulletScreenItem* bs = new BulletScreenItem(top);	// 显示到top栏上
    QPixmap pixmap(":/images/homePage/touxiang.png");
    bs->setBulletScreenIcon(pixmap);
    bs->setBulletScreenText(text);
    int duration = 10000 * width() / (double)(30 * 18 + 1450);
    bs->setBulletScreenAnimal(top->width(), duration);
    bs->startAnimal();
}

QString PlayerPage::secondToTime(int64_t second)
{
    QString time;
    time.reserve(20);
    // 小时
    if(second / 60 / 60) {
        time += QString::asprintf("%02lld:", second / 60 / 60);
    }
    // 分钟
    time += QString::asprintf("%02lld:%02lld", second / 60, second % 60);
    return time;
}

void PlayerPage::initBarrageArea()
{
    // 创建弹幕显示区域的对话框，没有边框，背景透明
    barrageArea = new QDialog(this);
    barrageArea->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    barrageArea->setAttribute(Qt::WA_TranslucentBackground);
    barrageArea->setMinimumSize(this->width(), 38 * 3);

    // 垂直布局
    QVBoxLayout* layout = new QVBoxLayout(barrageArea);
    barrageArea->setLayout(layout);

    // 在弹幕区域添加用来显示三行弹幕的控件
    top = new QFrame(this);
    top->setFixedSize(this->width(), 38);

    middle = new QFrame(this);
    middle->setFixedSize(this->width(), 38);

    bottom = new QFrame(this);
    bottom->setFixedSize(this->width(), 38);

    layout->addWidget(top);
    layout->addWidget(middle);
    layout->addWidget(bottom);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // 移动弹幕窗口到head下
    QPoint point = mapToGlobal(QPoint(0, 0));
    point.setY(point.y() + ui->playHead->height());
    barrageArea->move(point);
    barrageArea->show();
}
