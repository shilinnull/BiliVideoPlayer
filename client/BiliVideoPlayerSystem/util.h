#ifndef UTIL_H
#define UTIL_H

#include <QDebug>
#include <QString>
#include <QFileInfo>


static inline QString getFileName(const QString& path) {
    QFileInfo fileinfo(path);
    return fileinfo.fileName();
}

#define TAG QString("[%1:%2]").arg(getFileName(__FILE__), QString::number(__LINE__))
#define LOG() qDebug().noquote() << TAG

#endif // UTIL_H
