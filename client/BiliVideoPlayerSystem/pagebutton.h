#ifndef PAGEBUTTON_H
#define PAGEBUTTON_H

#include <QWidget>
#include <QPushButton>

class PageButton : public QPushButton
{
    Q_OBJECT
public:
    explicit PageButton(int page, QWidget *parent = nullptr);

};

#endif // PAGEBUTTON_H
