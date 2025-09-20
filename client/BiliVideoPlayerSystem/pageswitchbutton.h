#ifndef PageSwitchButton_H
#define PageSwitchButton_H

#include <QPushButton>
#include <QLabel>

class PageSwitchButton : public QPushButton
{
    Q_OBJECT
public:
    explicit PageSwitchButton(QWidget *parent = nullptr);

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

#endif // PageSwitchButton_H
