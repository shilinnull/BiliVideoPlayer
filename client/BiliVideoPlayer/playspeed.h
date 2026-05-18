#ifndef PLAYSPEED_H
#define PLAYSPEED_H

#include <QWidget>
#include <QButtonGroup>

namespace Ui {
class PlaySpeed;
}

class PlaySpeed : public QWidget
{
    Q_OBJECT

public:
    explicit PlaySpeed(QWidget *parent = nullptr);
    ~PlaySpeed();
    double speed() const;
    void setCurrentSpeed(double speed);

private slots:
    void onSpeedChanged(int id);
signals:
    void setPlaySpeed(double speed);
private:
    Ui::PlaySpeed *ui;
    QButtonGroup* speedGroup;
};

#endif // PLAYSPEED_H
