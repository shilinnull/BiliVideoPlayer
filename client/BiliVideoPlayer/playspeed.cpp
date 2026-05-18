#include "playspeed.h"
#include "ui_playspeed.h"

PlaySpeed::PlaySpeed(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlaySpeed)
    , speedGroup(new QButtonGroup(this))
{
    ui->setupUi(this);
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    speedGroup->setExclusive(true);
    speedGroup->addButton(ui->speed05, 5);
    speedGroup->addButton(ui->speed10, 10);
    speedGroup->addButton(ui->speed15, 15);
    speedGroup->addButton(ui->speed20, 20);

    ui->speed05->setCheckable(true);
    ui->speed10->setCheckable(true);
    ui->speed15->setCheckable(true);
    ui->speed20->setCheckable(true);
    ui->speed10->setChecked(true);

    connect(speedGroup, &QButtonGroup::idClicked, this, &PlaySpeed::onSpeedChanged);
}

PlaySpeed::~PlaySpeed()
{
    delete ui;
}

double PlaySpeed::speed() const
{
    int id = speedGroup->checkedId();
    if(id < 0) return 1.0;
    return id / 10.0;
}

void PlaySpeed::setCurrentSpeed(double speed)
{
    int id = qRound(speed * 10);
    auto* btn = speedGroup->button(id);
    if(btn) {
        btn->setChecked(true);
    }
}

void PlaySpeed::onSpeedChanged(int id)
{
    emit setPlaySpeed(id / 10.0);
}
