#pragma once

#include <QString>
#include <QStringList>


//UI����
QString From8Bit(const QByteArray& str);

//�㷨
bool isContainsAnd(QString& data, QString& compare);
bool isContainsOr(QString& data, QString& compare);


QString getBrightString(QString& data, QString& compare);