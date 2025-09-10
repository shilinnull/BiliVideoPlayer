#ifndef SEARCHLINEEDIT_H
#define SEARCHLINEEDIT_H

#include <QLineEdit>
#include <QEvent>
#include <QPushButton>
#include <QLabel>
#include <QWidget>


class searchLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit searchLineEdit(QWidget *parent = nullptr);

private slots:
    void searchBtnClicked();
protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
signals:

private:
    QLabel* searchImg;
    QPushButton* searchBtn;
};

#endif // SEARCHLINEEDIT_H
