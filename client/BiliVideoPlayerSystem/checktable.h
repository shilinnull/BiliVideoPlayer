#ifndef CHECKTABLE_H
#define CHECKTABLE_H

#include <QWidget>
#include "paginator.h"

namespace Ui {
class CheckTable;
}

class CheckTable : public QWidget
{
    Q_OBJECT

public:
    explicit CheckTable(QWidget *parent = nullptr);
    ~CheckTable();
private slots:
    // 重置按钮点击槽函数
    void onResetBtnClicked();
    // 查询按钮点击槽函数
    void onQueryBtnClicked();

private:
    void updateCheckTable();            // 更新审核页面
private:
    Ui::CheckTable *ui;
    Paginator* paginator = nullptr;     // 分页器指针
};

#endif // CHECKTABLE_H
