#ifndef ADMINWIDGET_H
#define ADMINWIDGET_H

#include <QWidget>

namespace Ui {
class AdminWidget;
}

class AdminWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AdminWidget(QWidget *parent = nullptr);
    ~AdminWidget();
private slots:
    // 审核管理按钮点击
    void onCheckBtnClicked();
    // 角色管理按钮点击
    void onRoleBtnClicked();

private:
    Ui::AdminWidget *ui;

    // 标签页样式
    const QString inactiveTabStyle = "QPushButton { "
                                     "background-color: #FFFFFF;"
                                     "font-size: 14px;"
                                     "color: #666666;"
                                     "border:none;"
                                     "border-bottom: 2px solid #F5F6F8; }";
    const QString activeTabStyle = "QPushButton { "
                                   "background-color: #FFFFFF;"
                                   "font-size: 14px;"
                                   "color: #666666;"
                                   "border:none;"
                                   "border-bottom: 2px solid #FF6699; }";
};

#endif // ADMINWIDGET_H
