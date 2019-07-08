#ifndef KLAYOUT_H
#define KLAYOUT_H

#include <QtCore>
#include <QtWidgets>
#include <Kpf.h>

static const QString TAG_ITEMS = QStringLiteral("Items");
static const QString TAG_STRETCH = QStringLiteral("Stretch");
static const QString TAG_LABEL = QStringLiteral("Label");
static const QString TAG_FIELD = QStringLiteral("Field");
static const QString TAG_ROW = QStringLiteral("Row");
static const QString TAG_COLUMN = QStringLiteral("Column");
static const QString TAG_ROWSPAN = QStringLiteral("RowSpan");
static const QString TAG_COLUMNSPAN = QStringLiteral("ColumnSpan");

/*
{
    "Class": "QHBoxLayout",
    "Items": [
        {
            "Class": "MainWindow",
            "Stretch": 1
        }
    ]
},
{
    "Class": "QFormLayout",
    "Items": [
        {
            "Stretch": 1,
            "Field": { "Class": "MainWindow" },
            "Label": "Test"
        }
    ]
},
{
    "Class": "QGridLayout",
    "Items": [
        {
            "Row": 0,
            "Column": 0,
            "Class": "MainWindow"
        }
    ]
}*/

class KHBoxLayout : public QHBoxLayout
{
    Q_OBJECT
public:
    explicit KHBoxLayout(QWidget* parent = nullptr);
    Q_SLOT bool init(const QJsonObject& config);
};
KPF_REGISTER_CLASS(KHBoxLayout, "QHBoxLayout")

class KVBoxLayout : public QVBoxLayout
{
    Q_OBJECT
public:
    explicit KVBoxLayout(QWidget* parent = nullptr);
    Q_SLOT bool init(const QJsonObject& config);
};
KPF_REGISTER_CLASS(KVBoxLayout, "QVBoxLayout")

class KFormLayout : public QFormLayout
{
    Q_OBJECT
public:
    explicit KFormLayout(QWidget* parent = nullptr);
    Q_SLOT bool init(const QJsonObject& config);
};
KPF_REGISTER_CLASS(KFormLayout, "QFormLayout")

class KGridLayout : public QGridLayout
{
    Q_OBJECT
public:
    explicit KGridLayout(QWidget* parent = nullptr);
    Q_SLOT bool init(const QJsonObject& config);
};
KPF_REGISTER_CLASS(KGridLayout, "QGridLayout")

#endif // KLAYOUT_H
