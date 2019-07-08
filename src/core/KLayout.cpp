#include "KLayout.h"

QList<QObject*> initObjects(const QJsonObject& config)
{
    QList<QObject*> objects;
    if (!config.contains(TAG_ITEMS)) {
        return objects;
    }
    QJsonArray array = config.value(TAG_ITEMS).toArray();
    for (auto it = array.begin(); it != array.end(); ++it)
    {
        if (!(*it).isObject()) {
            continue;
        }
        QJsonObject objectConfig = (*it).toObject();

        QObject* object = kpfCore.createObject(QString(),
                                               objectConfig.value(Kpf::TAG_CLASS).toString(),
                                               objectConfig);
        if (!object->inherits("QWidget") || !object->inherits("QLayout"))
        {
            kpfCore.destroyObject(object->objectName());
            continue;
        }

        objects << object;
    }
    return objects;
}

bool initBoxLayout(QBoxLayout* layout, const QJsonObject& config)
{
    QList<QObject*> objects = initObjects(config);

    for (QObject* object : objects)
    {
        QWidget* widget = qobject_cast<QWidget*>(object);
        QLayout* subLayout = qobject_cast<QLayout*>(object);

        bool ok = false;
        int stretch = object->property(TAG_STRETCH.toUtf8().constData()).toInt(&ok);
        if (!ok) {
            continue;
        }
        if (widget) {
            layout->addWidget(widget, stretch);
        } else if (subLayout) {
            layout->addLayout(subLayout, stretch);
        }
    }

    return true;
}

KHBoxLayout::KHBoxLayout(QWidget* parent)
    : QHBoxLayout(parent)
{
}

bool KHBoxLayout::init(const QJsonObject& config)
{
    return initBoxLayout(this, config);
}

KVBoxLayout::KVBoxLayout(QWidget* parent)
    : QVBoxLayout(parent)
{
}

bool KVBoxLayout::init(const QJsonObject& config)
{
    return initBoxLayout(this, config);
}

KFormLayout::KFormLayout(QWidget* parent)
    : QFormLayout(parent)
{
}

bool KFormLayout::init(const QJsonObject& config)
{
    struct Object
    {
        struct Label
        {
            QString text;
            QWidget* widget = nullptr;
        };
        Label label;
        QWidget* widget = nullptr;
        QLayout* layout = nullptr;
    };

    QList<Object> objects;
    if (!config.contains(TAG_ITEMS)) {
        return true;
    }

    QJsonArray array = config.value(TAG_ITEMS).toArray();
    for (auto it = array.begin(); it != array.end(); ++it)
    {
        if (!(*it).isObject()) {
            continue;
        }
        QJsonObject child = (*it).toObject();

        if (!child.contains(TAG_FIELD)) {
            continue;
        }
        QJsonObject fieldConfig = child.value(TAG_FIELD).toObject();
        QObject* object = kpfCore.createObject(QString(),
                                               fieldConfig.value(Kpf::TAG_CLASS).toString(),
                                               fieldConfig);
        Object obj;
        obj.widget = qobject_cast<QWidget*>(object);
        obj.layout = qobject_cast<QLayout*>(object);
        if (!obj.widget || !obj.layout)
        {
            if (object) {
                kpfCore.destroyObject(object->objectName());
            }
            continue;
        }

        if (child.contains(TAG_LABEL))
        {
            QJsonValue labelValue = child.value(TAG_FIELD);
            switch (labelValue.type())
            {
            case QJsonValue::String:
                obj.label.text = labelValue.toString();
                break;

            case QJsonValue::Object:
            {
                QJsonObject labelObject = labelValue.toObject();
                QObject* labelObj = kpfCore.createObject(QString(),
                                                         fieldConfig.value(Kpf::TAG_CLASS).toString(),
                                                         fieldConfig);
                obj.label.widget = qobject_cast<QWidget*>(labelObj);
                if (!obj.label.widget)
                {
                    if (labelObj) {
                        kpfCore.destroyObject(labelObj->objectName());
                    }
                    break;
                }
            }
                break;

            default:
                break;
            }
        }

        objects.append(obj);
    }

    for (Object object : objects)
    {
        if (object.label.widget)
        {
            if (object.widget) {
                addRow(object.label.widget, object.widget);
            } else {
                addRow(object.label.widget, object.layout);
            }
        }
        else
        {
            if (object.widget) {
                addRow(object.label.text, object.widget);
            } else {
                addRow(object.label.text, object.layout);
            }
        }
    }

    return true;
}

KGridLayout::KGridLayout(QWidget* parent)
    : QGridLayout(parent)
{
}

bool KGridLayout::init(const QJsonObject& config)
{
    QList<QObject*> objects = initObjects(config);

    for (QObject* object : objects)
    {
        QWidget* widget = qobject_cast<QWidget*>(object);
        QLayout* layout = qobject_cast<QLayout*>(object);

        int row = object->property(TAG_ROW.toUtf8().constData()).toInt();
        int column = object->property(TAG_COLUMN.toUtf8().constData()).toInt();
        int rowSpan = object->property(TAG_ROWSPAN.toUtf8().constData()).toInt();
        rowSpan = std::max(1, rowSpan);
        int columnSpan = object->property(TAG_ROWSPAN.toUtf8().constData()).toInt();
        columnSpan = std::max(1, columnSpan);

        if (widget) {
            addWidget(widget, row, column, rowSpan, columnSpan);
        } else if (layout) {
            addLayout(layout, row, column, rowSpan, columnSpan);
        }
    }

    return true;
}
