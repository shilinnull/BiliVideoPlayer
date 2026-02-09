#ifndef ROLETABLEITEM_H
#define ROLETABLEITEM_H

#include <QWidget>
#include "model/datacenter.h"

namespace Ui {
class RoleTableItem;
}

class RoleTableItem : public QWidget
{
    Q_OBJECT

public:
    explicit RoleTableItem(QWidget *parent, model::AdminInfo& adminInfo, int seqNumber = 1);
    ~RoleTableItem();
private:
    // 初始化样式表
    void initStyleSheet();
    // 更新界面
    void updateUI(int seqNumber);
    // 检测当前登录用户是否是管理员自己
    bool logUserisAdminMyself(const QString& text);

private slots:
    void onEditBtnClicked();        // 编辑按钮点击
    void onStatusBtnClicked();      // 状态按钮点击
    void onDelBtnClicked();         // 删除按钮点击
private:
    Ui::RoleTableItem *ui;
    // 样式表映射
    QMap<QString, QString> styleSheet;
    // 单个管理员信息
    model::AdminInfo adminInfo;
};

#endif // ROLETABLEITEM_H
