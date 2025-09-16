#ifndef PLAYERPAGE_H
#define PLAYERPAGE_H

#include <QWidget>

namespace Ui {
class PlayerPage;
}

class PlayerPage : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerPage(QWidget *parent = nullptr);
    ~PlayerPage();

private:
    Ui::PlayerPage *ui;
};

#endif // PLAYERPAGE_H
