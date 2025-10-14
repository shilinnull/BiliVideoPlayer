#ifndef PAGEBUTTON_H
#define PAGEBUTTON_H

#include <QWidget>
#include <QPushButton>

class PageButton : public QPushButton
{
    Q_OBJECT
public:
    explicit PageButton(int page, QWidget *parent = nullptr);
    void setActive(bool active);
    void setFolded(bool folded);
    void setPage(int page);
    int getPage() const ;

    // 按钮是否在点击状态
    bool isActived() const;
    bool isFolded() const;
private:
    int page;   // 按钮上页号
    bool isActiveBtn = false;
    bool isFoldedBtn = false;

};

#endif // PAGEBUTTON_H
