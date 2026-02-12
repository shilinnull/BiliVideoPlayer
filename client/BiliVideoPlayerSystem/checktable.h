#ifndef CHECKTABLE_H
#define CHECKTABLE_H

#include <QString>
#include <QWidget>

#include "paginator.h"
#include "model/data.h"

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
    // 获取视频列表
    void getVideoList(int page);
    // 重置视频列表
    void resetPaginator(int pageCount);

private:
    void updateCheckTable(const QString& userId, const QString& whichPage); // 更新审核页面
private:
    Ui::CheckTable *ui;
    Paginator* paginator = nullptr;     // 分页器指针
    int page = 1;                       // 获取当前第几页
    model::VideoStatus videoStatus;     // 点击查询按钮时视频状态
};

#endif // CHECKTABLE_H
