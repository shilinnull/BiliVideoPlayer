#ifndef CHECKTABLE_H
#define CHECKTABLE_H

#include <QWidget>

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
    Ui::CheckTable *ui;
};

#endif // CHECKTABLE_H
