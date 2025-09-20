#include "homepagewidget.h"
#include "ui_homepagewidget.h"
#include "util.h"
#include "videobox.h"

#include <QVBoxLayout>

HomePageWidget::HomePageWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HomePageWidget)
{
    ui->setupUi(this);

    initKindsAndTags();     // 初始化分类和标签按钮
    initRefreshAndTop();    // 初始化刷新和置顶按钮
    initVideos();           // 初始化视频列表
}

HomePageWidget::~HomePageWidget()
{
    delete ui;
}

void HomePageWidget::initKindsAndTags()
{
    // 创建分类按钮
    QPushButton* kindBtn = buildSelectBtn(ui->classifys, clicked_text_color, "分类");
    ui->classifyHLayout->addWidget(kindBtn);

    QList<QString> kinds = {"历史", "美食", "游戏", "科技", "运动", "动物", "旅行", "电影"};
    for(auto &kind : kinds)
    {
        QPushButton* kindBtn = buildSelectBtn(ui->classifys, default_text_color, kind);
        ui->classifyHLayout->addWidget(kindBtn);
        connect(kindBtn, &QPushButton::clicked, this, [=](){
            onKindBtnClicked(kindBtn);      // 分类按钮点击
        });
    }
    ui->classifyHLayout->setSpacing(8);


    // 分类和该分类下所有标签映射
    tags = {
        {"历史", {"中国史", "世界史", "历史人物", "艺术", "文化", "奇闻"}},
        {"美食", {"美食测评", "美食制作", "美食攻略", "美食记录", "探店", "水果", "海鲜"}},
        {"游戏", {"游戏攻略", "单机游戏", "电子竞技", "手机游戏", "网络游戏", "游戏赛事","桌游棋牌"}},
        {"科技", {"数码", "软件应用", "智能家居", "手机", "电脑", "人工智能", "基础设施"}},
        {"运动", {"篮球", "足球", "乒乓球", "羽毛球", "健身", "竞技体育", "运动装备"}},
        {"动物", {"喵星人", "汪星人", "宠物知识", "动物资讯", "野生动物", "动物世界", "萌宠"}},
        {"旅行", {"旅游攻略", "旅行Vlog", "自驾游", "交通", "环球旅行", "露营", "野外生存"}},
        {"电影", {"电影解说", "电影推荐", "电影剪辑", "搞笑", "吐槽", "悬疑", "经典"}}
    };

    // 默认显示历史标签
    resetTags(tags["历史"]);
    // 默认选中第一个标签
    QList<QPushButton*> kindBtns = ui->classifys->findChildren<QPushButton*>();
    onKindBtnClicked(kindBtns[1]);
}

void HomePageWidget::initRefreshAndTop()
{
    // 先创建一个QWidget
    QWidget* refreshTopWidget = new QWidget(this);
    refreshTopWidget->setFixedSize(42, 94);
    refreshTopWidget->setStyleSheet("QPushButton{"
                                    "border-radius: 21px;"
                                    "border: none;"
                                    "background-color: #FFECF1;}"
                                    "QPushButton:hover{background-color: #EC5D85;}");

    QVBoxLayout* layout = new QVBoxLayout(refreshTopWidget);
    QPushButton* topBtn = new QPushButton(refreshTopWidget);
    topBtn->setFixedSize(42, 42);
    topBtn->setStyleSheet("border-image : url(:/images/homePage/zhiding.png);");
    layout->addWidget(topBtn);

    QPushButton* refreshBtn = new QPushButton(refreshTopWidget);
    refreshBtn->setFixedSize(42, 42);
    refreshBtn->setStyleSheet("border-image : url(:/images/homePage/shuaxin.png);");

    layout->addWidget(refreshBtn);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);

    refreshTopWidget->move(1278, 618);

    connect(topBtn, &QPushButton::clicked, this, &HomePageWidget::onTopBtnClicked);
    connect(refreshBtn, &QPushButton::clicked, this, &HomePageWidget::onRefreshBtnClicked);
}

void HomePageWidget::initVideos()
{
    for(int i = 0; i < 16; i++)
    {
        VideoBox* video = new VideoBox();
        ui->videoGLayout->addWidget(video, i / 4, i % 4);
    }
}

QPushButton *HomePageWidget::buildSelectBtn(QWidget *parent, const QString &color, const QString &text)
{
    QPushButton* btn = new QPushButton(text, parent);
    btn->setMinimumHeight(26);
    btn->setFixedWidth(text.size() * 16 + 18 + 18);
    btn->setStyleSheet("color: " + color);
    return btn;
}

void HomePageWidget::resetTags(const QList<QString> &tags)
{
    // 创建标签
    QPushButton* tag = buildSelectBtn(ui->labels, clicked_text_color, "标签");
    ui->labelHLayout->addWidget(tag);

    // 创建具体标签的按钮
    for(auto &tagText : tags)
    {
        QPushButton* labelBtn = buildSelectBtn(ui->labels, default_text_color, tagText);
        ui->labelHLayout->addWidget(labelBtn);
        connect(labelBtn, &QPushButton::clicked, this, [=](){
            onTagBtnClicked(labelBtn);      // 标签按钮点击
        });
    }
    ui->labelHLayout->setSpacing(4);
}

void HomePageWidget::onKindBtnClicked(QPushButton *clickKindBtn)
{

    clickKindBtn->setStyleSheet("background-color: " + clicked_background_color + ";"
                                "color: " + clicked_text_color +";");

    // 清除旧颜色
    QList<QPushButton*> kindBtns = ui->classifys->findChildren<QPushButton*>();
    for(auto &kindBtn : kindBtns)
    {
        if(kindBtn != clickKindBtn && kindBtn->text() != "分类")
            kindBtn->setStyleSheet("color: " + default_text_color);
    }

    // 销毁之前的标签
    QList<QPushButton*> labels = ui->labels->findChildren<QPushButton*>();
    for(auto &label : labels)
    {
        ui->labelHLayout->removeWidget(label);
        delete label;
    }

    // 选中了该分类重新添加标签
    resetTags(tags[clickKindBtn->text()]);
}

void HomePageWidget::onTagBtnClicked(QPushButton *clickLabelBtn)
{

    clickLabelBtn->setStyleSheet("background-color: " + clicked_background_color + ";"
                                "color: " + clicked_text_color +";");
    // 清除旧颜色
    QList<QPushButton*> tagBtns = ui->labels->findChildren<QPushButton*>();
    for(auto &tagBtn : tagBtns)
    {
        if(tagBtn != clickLabelBtn && tagBtn->text() != "标签")
            tagBtn->setStyleSheet("color: " + default_text_color);
    }
}

void HomePageWidget::onRefreshBtnClicked()
{
    LOG() << "刷新按钮点击了";
}

void HomePageWidget::onTopBtnClicked()
{
    LOG() << "置顶按钮点击了";
}
