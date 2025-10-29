#include "data.h"

namespace model{

int KindAndTag::id = 10000;

KindAndTag::KindAndTag()
{
    QList<QString> kinds = {"历史", "美食", "游戏", "科技", "运动", "动物", "旅行", "电影"};
    for(auto& kind : kinds) {
        kindIds.insert(kind, id++);
    }

    QHash<QString, QList<QString>> kindsAndTags = {
        {"历史", {"中国史", "世界史", "历史人物", "艺术", "文化", "奇闻"}},
        {"美食", {"美食测评", "美食制作", "美食攻略", "美食记录", "探店", "水果", "海鲜"}},
        {"游戏", {"游戏攻略", "单机游戏", "电子竞技", "手机游戏", "网络游戏", "游戏赛事","桌游棋牌"}},
        {"科技", {"数码", "软件应用", "智能家居", "手机", "电脑", "人工智能", "基础设施"}},
        {"运动", {"篮球", "足球", "乒乓球", "羽毛球", "健身", "竞技体育", "运动装备"}},
        {"动物", {"喵星人", "汪星人", "宠物知识", "动物资讯", "野生动物", "动物世界", "萌宠"}},
        {"旅行", {"旅游攻略", "旅行Vlog", "自驾游", "交通", "环球旅行", "露营", "野外生存"}},
        {"电影", {"电影解说", "电影推荐", "电影剪辑", "搞笑", "吐槽", "悬疑", "经典"}}
    };

    // 构建分类下的各个标签以及对应id
    for(auto& kind : kinds) {
        QList<QString>& tags = kindsAndTags[kind];
        QHash<QString, int> tagIdsOfKind;
        for(auto& tag : tags) {
            tagIdsOfKind.insert(tag, id++);
        }
        tagIds.insert(kind, tagIdsOfKind);
    }
}

const QList<QString> KindAndTag::getAllKinds() const
{
    return kindIds.keys();
}

const QHash<QString, int> KindAndTag::getTagsByKind(QString kind) const
{
    return tagIds[kind];
}

int KindAndTag::getKindId(QString kind) const
{
    return kindIds[kind];
}

int KindAndTag::getTagId(QString kind, QString tag) const
{
    return tagIds[kind][tag];
}

const KindAndTag *KindAndTag::getKindAndTagsClassPtr()
{
    if(nullptr == kindsAndTags)
        kindsAndTags = new KindAndTag();
    return kindsAndTags;
}
}
