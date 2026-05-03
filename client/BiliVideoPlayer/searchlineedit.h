#ifndef SearchLineEdit_H
#define SearchLineEdit_H

#include <QEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QString>


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
    // 发送搜索信号给首页
    void searchVideos(const QString& searchText);
private:
    QLabel* searchImg;
    QPushButton* searchBtn;
};

#endif // SearchLineEdit_H
