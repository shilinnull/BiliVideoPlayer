#ifndef HOMEPAGEWIDGET_H
#define HOMEPAGEWIDGET_H

#include <QWidget>

namespace Ui {
class homePageWidget;
}

class homePageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit homePageWidget(QWidget *parent = nullptr);
    ~homePageWidget();

private:
    Ui::homePageWidget *ui;
};

#endif // HOMEPAGEWIDGET_H
