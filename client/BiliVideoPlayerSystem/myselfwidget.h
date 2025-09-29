#ifndef MYSELFWIDGET_H
#define MYSELFWIDGET_H

#include <QWidget>
#include "util.h"

namespace Ui {
class MyselfWidget;
}

class MyselfWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyselfWidget(QWidget *parent = nullptr);
    ~MyselfWidget();
private:
    void initUI();
    void uploadAvatarBtnClicked();  // 点击上传头像
    void settingBtnClicked();       // 点击设置按钮
private:
    Ui::MyselfWidget *ui;
};

#endif // MYSELFWIDGET_H
