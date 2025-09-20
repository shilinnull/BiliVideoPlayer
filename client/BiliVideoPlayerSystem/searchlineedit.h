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
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
signals:

private:
    QLabel* searchImg;
    QPushButton* searchBtn;
};

#endif // SearchLineEdit_H
