#include "playspeed.h"
#include "ui_playspeed.h"

#include "util.h"

PlaySpeed::PlaySpeed(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlaySpeed)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    connect(ui->speed05, &QPushButton::clicked, this, &PlaySpeed::onPlay05Speed);
    connect(ui->speed10, &QPushButton::clicked, this, &PlaySpeed::onPlay10Speed);
    connect(ui->speed15, &QPushButton::clicked, this, &PlaySpeed::onPlay15Speed);
    connect(ui->speed20, &QPushButton::clicked, this, &PlaySpeed::onPlay20Speed);
}

PlaySpeed::~PlaySpeed()
{
    delete ui;
}

void PlaySpeed::onPlay05Speed()
{
    LOG() << "0.5 倍速";
}

void PlaySpeed::onPlay10Speed()
{
    LOG() << "1.0 倍速";
}

void PlaySpeed::onPlay15Speed()
{
    LOG() << "1.5 倍速";
}

void PlaySpeed::onPlay20Speed()
{
    LOG() << "2.0 倍速";
}
