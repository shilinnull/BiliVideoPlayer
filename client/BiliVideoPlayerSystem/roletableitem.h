#ifndef ROLETABLEITEM_H
#define ROLETABLEITEM_H

#include <QWidget>

namespace Ui {
class RoleTableItem;
}

class RoleTableItem : public QWidget
{
    Q_OBJECT

public:
    explicit RoleTableItem(QWidget *parent = nullptr, int seqNumber = 1);
    ~RoleTableItem();
    void updateUI(int seqNumber);
private slots:
    void onEditBtnClicked();        // 编辑按钮点击
private:
    Ui::RoleTableItem *ui;
};

#endif // ROLETABLEITEM_H
