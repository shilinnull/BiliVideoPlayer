#ifndef ROLETABLE_H
#define ROLETABLE_H

#include <QMap>
#include <QString>
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
    void onInsertBtnClicked();      // 插入按钮点击
    void updateAdminListUI();       // 更新管理员列表信息到界面
    void getAdminList(int page);    // 获取管理员列表
    void resetPaginator(int pageCount); // 重置分页器
private:
    Ui::RoleTable *ui;

    // 保存按钮样式
    QMap<QString, QString> styleSheet;
    Paginator* paginator = nullptr;  // 分页器指针
    int page = 1;                   // 记录分页器页数
};

#endif // ROLETABLE_H
