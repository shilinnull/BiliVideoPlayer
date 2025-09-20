#ifndef SearchLineEdit_H
#define SearchLineEdit_H

#include <QLineEdit>
#include <QEvent>
#include <QPushButton>
#include <QLabel>
#include <QWidget>


class SearchLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit SearchLineEdit(QWidget *parent = nullptr);

private slots:
    void searchBtnClicked();
protected:
    bool eventFilter(QObject *watched, QEvent *event);
signals:

private:
    QLabel* searchImg;
    QPushButton* searchBtn;
};

#endif // SearchLineEdit_H
