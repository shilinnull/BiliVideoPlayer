#ifndef PAGESWITCHBUTTON_H
#define PAGESWITCHBUTTON_H

#include <QPushButton>
#include <QLabel>

#include "util.h"

class pageSwitchButton : public QPushButton
{
    Q_OBJECT
public:
    explicit pageSwitchButton(QWidget *parent = nullptr);

    void setImageAndText(const QString &imagePath, const QString &text, int pageId);
    void setImage(const QString& imagePath);
    void setTextColor(const QString& textColor);
    int getPageId() const ;

protected:
    void mousePressEvent(QMouseEvent* event) override ;
private:
    QLabel* btnImage;
    QLabel* btnTittle;

    int pageId;

signals:
    void switchPage(int pageId);
};

#endif // PAGESWITCHBUTTON_H
