#ifndef UPLOADVIDEOPAGE_H
#define UPLOADVIDEOPAGE_H

#include <QWidget>

namespace Ui {
class UploadVideoPage;
}

class UploadVideoPage : public QWidget
{
    Q_OBJECT

public:
    explicit UploadVideoPage(QWidget *parent = nullptr);
    ~UploadVideoPage();
private:
    void onCommitBtnClicked();                  // 提交按钮

signals:
    void switchMySelfPage(int pageIndex);       // 切换到我的页面
private:
    Ui::UploadVideoPage *ui;
};

#endif // UPLOADVIDEOPAGE_H
