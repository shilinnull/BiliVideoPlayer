#ifndef ROLETABLE_H
#define ROLETABLE_H

#include <QWidget>

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
    // 初始化样式表
    void initStyleSheet();

    void updateRoleTable();
private:
    Ui::RoleTable *ui;

    // 保存按钮样式
    QMap<QString, QString> styleSheet;
};

#endif // ROLETABLE_H
