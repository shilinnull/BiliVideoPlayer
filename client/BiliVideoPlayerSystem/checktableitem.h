#ifndef CHECKTABLEITEM_H
#define CHECKTABLEITEM_H

#include <QWidget>

namespace Ui {
class CheckTableItem;
}

class CheckTableItem : public QWidget
{
    Q_OBJECT

public:
    explicit CheckTableItem(QWidget *parent = nullptr);
    ~CheckTableItem();

private:
    Ui::CheckTableItem *ui;
};

#endif // CHECKTABLEITEM_H
