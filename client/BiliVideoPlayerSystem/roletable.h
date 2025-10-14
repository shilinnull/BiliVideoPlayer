#ifndef ROLETABLE_H
#define ROLETABLE_H

#include <QWidget>
#include "paginator.h"

namespace Ui {
class RoleTable;
}

class RoleTable : public QWidget
{
    Q_OBJECT

public:
    explicit RoleTable(QWidget *parent = nullptr);
    ~RoleTable();
private slots:
    // 重置按钮点击
    void onResetBtnClicked();
    // 查询按钮点击
    void onQueryBtnClicked();
private:
    void initStyleSheet();          // 初始化样式表
    void updateRoleTable();         // 角色管理项
    void onInsertBtnClicked();      // 插入按钮点击
private:
    Ui::RoleTable *ui;

    // 保存按钮样式
    QMap<QString, QString> styleSheet;
    Paginator* paginator = nullptr;  // 分页器指针
};

#endif // ROLETABLE_H
