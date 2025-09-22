#ifndef PLAYSPEED_H
#define PLAYSPEED_H

#include <QWidget>

namespace Ui {
class PlaySpeed;
}

class PlaySpeed : public QWidget
{
    Q_OBJECT

public:
    explicit PlaySpeed(QWidget *parent = nullptr);
    ~PlaySpeed();

private slots:
    void onPlay05Speed();
    void onPlay10Speed();
    void onPlay15Speed();
    void onPlay20Speed();

private:
    Ui::PlaySpeed *ui;
};

#endif // PLAYSPEED_H
