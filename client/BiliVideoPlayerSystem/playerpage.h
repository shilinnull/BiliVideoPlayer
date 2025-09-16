#ifndef PLAYERPAGE_H
#define PLAYERPAGE_H

#include <QWidget>
#include <QMouseEvent>

namespace Ui {
class PlayerPage;
}

class PlayerPage : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerPage(QWidget *parent = nullptr);
    ~PlayerPage();
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    Ui::PlayerPage *ui;
    QPoint dragPos;
};

#endif // PLAYERPAGE_H
