#ifndef TESTJSON_H
#define TESTJSON_H

#include <QJsonArray>
#include <QJsonObject>
#include <QByteArray>
#include <QJsonDocument>
#include "../util.h"


QByteArray serialize() {
    QJsonObject student;
    student.insert("name", "张三");
    student.insert("age", 18);
    student["gender"] = "男";

    // 创建hobby的json数组
    QJsonArray hobby;
    hobby.append("篮球");
    hobby.append("足球");
    hobby.append("编程");
    student.insert("hobby", hobby);

    // 构造score的json对象
    QJsonObject score;
    score.insert("C语言", 82);
    score.insert("C++", 76);
    score.insert("数据结构", 80);
    student.insert("score", score);

    LOG()<<"student Json对象中键值对的个数："<<student.count();
    LOG()<<student;

    QJsonDocument jsonDoc(student);
    return jsonDoc.toJson();
}

void Deserialize(QByteArray studentArray) {
    QJsonDocument jsonDoc = QJsonDocument::fromJson(studentArray);
    QJsonObject student = jsonDoc.object();
    LOG() << student;

    LOG() << "name: " << student["name"].toString();
    LOG() << "age: " << student["age"].toInt();
    LOG() << "gender: " << student["gender"].toString();

    QJsonArray hobby = student["hobby"].toArray();
    LOG() << "Array: ";
    for(int i = 0; i < hobby.count(); i++) {
        LOG() << hobby[i].toString();
    }

    QJsonObject score = student["score"].toObject();
    LOG() << "score: ";
    LOG() << "C语言: " << score["C语言"].toInt();
    LOG() << "C++: " << score["C++"].toInt();
    LOG() << "数据结构: " << score["数据结构"].toInt();
}

#endif // TESTJSON_H
