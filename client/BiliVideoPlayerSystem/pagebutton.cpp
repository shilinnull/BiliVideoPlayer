#include "pagebutton.h"

PageButton::PageButton(int page, QWidget *parent)
    : QPushButton{parent}
{
    this->page = page;
    setIconSize(QSize(12, 12));
    setFixedSize(QSize(32, 32));
    if(isFoldedBtn) {
        setText("...");
    } else {
        setText(QString::number(page));
    }
    setActive(isActiveBtn);
}

void PageButton::setActive(bool active)
{
    isActiveBtn = active;
    if(active) {
        setStyleSheet("QPushButton{"
                      "color: #FFFFFF;"
                      "background-color: #FF6699;"
                      "border: 1px solid #FF6699;"
                      "border-radius: 2px;}");
    } else {
        setStyleSheet("QPushButton {"
                      "color: #000000; "
                      "background-color: #FFFFFF; "
                      "border: 1px solid #D9D9D9; "
                      "border-radius: 2px;}");
    }
}

void PageButton::setFolded(bool folded)
{
    isFoldedBtn = folded;
    if(isFoldedBtn) {
        setText("...");
    } else {
        setText(QString::number(page));
    }
}

void PageButton::setPage(int page)
{
    this->page = page;
    setText(QString::number(page));
}

int PageButton::getPage() const
{
    return page;
}

bool PageButton::isActived() const
{
    return isFoldedBtn;
}

bool PageButton::isFolded() const
{
    return isActiveBtn;
}
