#ifndef TOAST_H
#define TOAST_H

#include <QDialog>

class Toast : public QDialog
{
    Q_OBJECT
public:
    Toast(const QString& text, QWidget *pWdidget);
    Toast(const QString& text);

    static void showMessage(const QString& text);
    static void showMessage(const QString& text, QWidget* pWidget);

private:
    void initUI(const QString& text);
};

#endif // TOAST_H
