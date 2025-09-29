#include "modifymyselfdialog.h"
#include "ui_modifymyselfdialog.h"

#include <QGraphicsDropShadowEffect>
#include "util.h"


ModifyMyselfDialog::ModifyMyselfDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ModifyMyselfDialog)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);
    ui->passwordWidget->hide();

    setWindowIcon(QIcon(":/images/homePage/logo.png"));

    QGraphicsDropShadowEffect* dropShadow = new QGraphicsDropShadowEffect(this);
    dropShadow->setColor(Qt::black);
    dropShadow->setBlurRadius(5);
    dropShadow->setOffset(0, 0);
    this->setGraphicsEffect(dropShadow);

    connect(ui->submitBtn, &QPushButton::clicked, this, &ModifyMyselfDialog::onSubmitBtnClicked);
    connect(ui->cancelBtn, &QPushButton::clicked, this, &ModifyMyselfDialog::onCancelBtnClicked);
    connect(ui->changePasswordBtn, &QPushButton::clicked, this, &ModifyMyselfDialog::showPasswordDlg);

}

ModifyMyselfDialog::~ModifyMyselfDialog()
{
    delete ui;
}

void ModifyMyselfDialog::onSubmitBtnClicked()
{
    LOG() << "提交";
}

void ModifyMyselfDialog::onCancelBtnClicked()
{
    this->close();
}

void ModifyMyselfDialog::showPasswordDlg()
{

}
