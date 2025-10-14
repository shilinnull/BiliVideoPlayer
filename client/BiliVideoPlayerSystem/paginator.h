#ifndef PAGINATOR_H
#define PAGINATOR_H

#include <QWidget>
#include <QLineEdit>
#include "pagebutton.h"

class Paginator : public QWidget
{
    Q_OBJECT
public:
    explicit Paginator(int count, QWidget *parent = nullptr);
private:
    void setBtnStyle(QPushButton* btn);
    void initSignalSlot();
    // 跳转到page页
    void jumpToPage(int page);
    // page <= 5 的情况
    void jumpToPageCase1(int page);
    // page >= pageCount - 4 的情况
    void jumpToPageCase2(int page);
    // page 在 5 到 pageCount - 4 之间
    void jumpToPageCase3(int page);
private slots:
    void prevPage();
    void nextPage();
    void clickPageBtn();
signals:
    void pageChanged(int page);
private:
    int pageCount = 7;      // 总页数
    int currentPage = 1;    // 当前页
    const int pageSize = 20;// 每页显示的条目数

    // 上一页和下一页按钮
    QPushButton* prevPageBtn;
    QPushButton* nextPageBtn;
    QList<PageButton*> pages;   // 页面切换按钮集
    QLineEdit* pageEdit;        // 跳转到置顶页面
};

#endif // PAGINATOR_H
