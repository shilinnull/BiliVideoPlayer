#ifndef DATA_H
#define DATA_H

#include <QString>
#include <QList>
#include <QHash>

namespace model{


class KindAndTag {
public:
    KindAndTag();
    const QList<QString> getAllKinds() const; 						// 获取所有分类
    const QHash<QString, int> getTagsByKind(QString kind) const;	// 获取一个分类下的所有id
    int getKindId(QString kind) const;								// 获取分类的id
    int getTagId(QString kind, QString tag) const;					// 获取kind分类下包含的tagid
    const KindAndTag* getKindAndTagsClassPtr();						// 获取所有分类

private:
    QHash<QString, int> kindIds;						// 分类名称, 分类id
    QHash<QString, QHash<QString, int>> tagIds;			// 分类名称, <标签名称, 标签id>
    KindAndTag* kindsAndTags = nullptr;					// 分类和标签实例指针
    static int id;
};

};

#endif // DATA_H
