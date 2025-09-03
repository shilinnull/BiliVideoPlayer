#ifndef STARTUPPAGE_H
#define STARTUPPAGE_H

#include <QDialog>
#include <QLabel>
#include <QTimer>

class StartupPage : public QDialog
{
    Q_OBJECT
public:
    explicit StartupPage(QDialog *parent = nullptr);

    void startTimer();
};

#endif // STARTUPPAGE_H
