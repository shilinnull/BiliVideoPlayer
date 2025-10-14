#include "paginator.h"

#include <QHBoxLayout>
#include <QLabel>
#include "pagebutton.h"

Paginator::Paginator(int count, QWidget *parent)
    : QWidget{parent}
{
    pageCount = count;
    this->setMinimumSize(1270, 32);
    QHBoxLayout* layout = new QHBoxLayout();
    this->setLayout(layout);
    layout->setContentsMargins(0, 0, 8, 0);
    layout->setSpacing(8);

    // 创建上一页按钮
    prevPageBtn = new QPushButton();
    prevPageBtn->setIcon(QIcon(":/images/admin/arrow-left.png"));
    setBtnStyle(prevPageBtn);

    // 创建下一页按钮
    nextPageBtn = new QPushButton();
    nextPageBtn->setIcon(QIcon(":/images/admin/arrow-right.png"));
    setBtnStyle(nextPageBtn);

    if(pageCount < 7) {
        for(int i = 1; i <= pageCount; i++) {
            PageButton *pageBtn = new PageButton(i);
            if(i == 1) {
                pageBtn->setActive(true);
            }
            pages.push_back(pageBtn);
        }
    } else {
        // 页面超过7页，前5个页面按钮上显示1~5数字
        // 第6个按钮为折叠按钮，第7个按钮上显示最后一页
        // 默认显示第1页
        for(int i = 1; i <= 5; i++) {
            PageButton* pageBtn = new PageButton(i);
            if(i == 1) {
                pageBtn->setActive(true);
            }
            pages.push_back(pageBtn);
        }
        // 添加...按钮
        PageButton* foldedBtn = new PageButton(0);
        foldedBtn->setFolded(true);
        pages.push_back(foldedBtn);

        // 添加最后一页按钮
        PageButton* lastBtn = new PageButton(pageCount);
        pages.push_back(lastBtn);
    }
    // 创建输入框
    pageEdit = new QLineEdit();
    pageEdit->setFixedSize(QSize(48, 32));
    pageEdit->setAlignment(Qt::AlignLeft);
    pageEdit->setStyleSheet("QLineEdit{"
                            "background-color: #FFFFFF;"
                            "border: 1px solid #D9D9D9;"
                            "border-radius: 2px;"
                            "padding-left: 12px; }");

    // 创建两个 QLabel 显示文字
    QLabel* jumpToLabel = new QLabel("跳至");
    QLabel* pageLabel = new QLabel("页");
    layout->addStretch();   // 增加空白区域
    layout->addWidget(prevPageBtn);
    for(int i = 0; i < pages.size(); i++) {
        layout->addWidget(pages[i]);
    }
    layout->addWidget(nextPageBtn);
    layout->addWidget(jumpToLabel);
    layout->addWidget(pageEdit);
    layout->addWidget(pageLabel);

    initSignalSlot();
}

void Paginator::setBtnStyle(QPushButton *btn)
{
    btn->setIconSize(QSize(12, 12));
    btn->setFixedSize(QSize(32, 32));
    btn->setStyleSheet("QPushButton {"
                       "color: #000000; "
                       "background-color: #FFFFFF; "
                       "border: 1px solid #D9D9D9; "
                       "border-radius: 2px;}");
}

void Paginator::initSignalSlot()
{
    connect(prevPageBtn, &QPushButton::clicked, this, &Paginator::prevPage);
    connect(nextPageBtn, &QPushButton::clicked, this, &Paginator::nextPage);

    // 处理所有页码按钮
    for(auto* pageBtn : pages) {
        connect(pageBtn, &QPushButton::clicked, this, &Paginator::clickPageBtn);
    }
    // 处理输入框
    connect(pageEdit, &QLineEdit::returnPressed, this, [=]{
        int page = pageEdit->text().toInt();
        if(page < 1)
            page = 1;
        if(page > pageCount)
            page = pageCount;
        this->jumpToPage(page);
    });
}

void Paginator::jumpToPage(int page)
{
    if (page < 1 || page > pageCount) {
        return;
    }
    if (pageCount <= 7) {
        // 页数不超过 7 页，直接设置选中指定按钮
        currentPage = page;
        for (auto* pageBtn : pages) {
            pageBtn->setActive(false);
        }
        pages[currentPage - 1]->setActive(true);
    } else {
        // 页数超过 7 个
        if (page <= 5) {
            jumpToPageCase1(page); // 查看前5页
        } else if (page >= pageCount - 4) {
            jumpToPageCase2(page); // 查看后5页
        } else {
            jumpToPageCase3(page); // 查看中间页
        }
        // 更新当前页数
        currentPage = page;
    }
    // 发送信号
    emit this->pageChanged(currentPage);
}

void Paginator::jumpToPageCase1(int page)
{
    // page <= 5 的情况
    // 固定 7 个按钮, 前 5 个按钮显示数字,
    // 第 6 个按钮时折叠按钮，最后一个按钮显示最后一页
    pages[0]->setPage(1);
    pages[1]->setPage(2);
    pages[2]->setPage(3);
    pages[3]->setPage(4);
    pages[4]->setPage(5);
    pages[5]->setPage(6);
    pages[6]->setPage(pageCount);
    // 处理激活状态和折叠状态
    for (int i = 0; i < pages.size(); i++) {
        if (pages[i]->getPage() == page) {
            pages[i]->setActive(true);
        } else {
            pages[i]->setActive(false);
        }
        if (i == 5) {
            pages[i]->setFolded(true);
        } else {
            pages[i]->setFolded(false);
        }
    }
}

void Paginator::jumpToPageCase2(int page)
{
    // page >= pageCount - 4 的情况
    // 1) 固定 7 个按钮, 第 1 个显示第1页 第 2 个为折叠按钮
    // 其余按钮显示最后5页
    pages[0]->setPage(1);
    pages[1]->setPage(pageCount - 5);
    pages[2]->setPage(pageCount - 4);
    pages[3]->setPage(pageCount - 3);
    pages[4]->setPage(pageCount - 2);
    pages[5]->setPage(pageCount - 1);
    pages[6]->setPage(pageCount);
    // 2) 处理激活状态和折叠状态
    for (int i = 0; i < pages.size(); i++) {
        if (pages[i]->getPage() == page) {
            pages[i]->setActive(true);
        } else {
            pages[i]->setActive(false);
        }
        if (i == 1) {
            pages[i]->setFolded(true);
        } else {
            pages[i]->setFolded(false);
        }
    }
}

void Paginator::jumpToPageCase3(int page)
{
    // 2个折叠按钮
    // 1) 用中间按钮, 下标 3 表示当前页
    pages[0]->setPage(1);
    pages[1]->setPage(page - 2);
    pages[2]->setPage(page - 1);
    pages[3]->setPage(page);
    pages[4]->setPage(page + 1);
    pages[5]->setPage(page + 2);
    pages[6]->setPage(pageCount);
    // 2) 处理激活状态和折叠状态
    for (int i = 0; i < pages.size(); i++) {
        // 处理激活状态, 中间按钮激活
        if (i == 3) {
            pages[i]->setActive(true);
        } else {
            pages[i]->setActive(false);
        }
        // 处理折叠状态, 下标为 1 和 5 的按钮折叠
        if (i == 1 || i == 5) {
            pages[i]->setFolded(true);
        } else {
            pages[i]->setFolded(false);
        }
    }
}

void Paginator::prevPage()
{
    if (currentPage == 1) {
        return;
    }
    jumpToPage(currentPage - 1);
}

void Paginator::nextPage()
{
    if (currentPage == pageCount) {
        return;
    }
    jumpToPage(currentPage + 1);
}

void Paginator::clickPageBtn()
{
    // sender()：返回触发该槽函数对应的信号发射者
    int page = static_cast<PageButton*>(sender())->getPage();
    jumpToPage(page);
}
