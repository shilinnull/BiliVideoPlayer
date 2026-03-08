#include "playspeed.h"
#include "ui_playspeed.h"

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

    ui->speed10->setStyleSheet("background-color: rgba(255,255,255,0.41);");
}

PlaySpeed::~PlaySpeed()
{
    delete ui;
}

void PlaySpeed::onPlay05Speed()
{
    resetStyle(5);
    emit setPlaySpeed(0.5);
}

void PlaySpeed::onPlay10Speed()
{
    resetStyle(10);
    emit setPlaySpeed(1.0);
}

void PlaySpeed::onPlay15Speed()
{
    resetStyle(15);
    emit setPlaySpeed(1.5);
}

void PlaySpeed::onPlay20Speed()
{
    resetStyle(20);
    emit setPlaySpeed(2.0);
}

void PlaySpeed::resetStyle(int speed)
{
    ui->speed05->setStyleSheet("");
    ui->speed10->setStyleSheet("");
    ui->speed15->setStyleSheet("");
    ui->speed20->setStyleSheet("");
    switch (speed) {
    case 20:
        ui->speed20->setStyleSheet("background-color : rgba(255, 255, 255, 0.41);");
        break;
    case 15:
        ui->speed15->setStyleSheet("background-color : rgba(255, 255, 255, 0.41);");
        break;
    case 10:
        ui->speed10->setStyleSheet("background-color : rgba(255, 255, 255, 0.41);");
        break;
    case 5:
        ui->speed05->setStyleSheet("background-color : rgba(255, 255, 255, 0.41);");
        break;
    default:
        break;
    }
}
