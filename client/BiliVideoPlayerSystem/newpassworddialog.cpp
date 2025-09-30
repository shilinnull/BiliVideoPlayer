#include "newpassworddialog.h"
#include "ui_newpassworddialog.h"

NewPasswordDialog::NewPasswordDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NewPasswordDialog)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);

    connect(ui->cancelBtn, &QPushButton::clicked, this, &NewPasswordDialog::close);
}

NewPasswordDialog::~NewPasswordDialog()
{
    delete ui;
}
