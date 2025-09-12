#ifndef VIDEOBOX_H
#define VIDEOBOX_H

#include <QWidget>

namespace Ui {
class VideoBox;
}

class VideoBox : public QWidget
{
    Q_OBJECT

public:
    explicit VideoBox(QWidget *parent = nullptr);
    ~VideoBox();

private:
    Ui::VideoBox *ui;
};

#endif // VIDEOBOX_H
