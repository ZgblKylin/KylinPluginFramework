#include "CommonPrivate.h"
#include "KpfPrivate.h"
#include "KpfCoreImpl.h"
#include "RegisterQtClasses.h"

const QLoggingCategory& kpf()
{
    static const QLoggingCategory category("kpf", QtWarningMsg);
    return category;
}

QString normalizedSignature(QString signature)
{
    if (signature.isEmpty()) {
        return {};
    }

    signature = QMetaObject::normalizedSignature(signature.toUtf8().constData());
    int index = signature.indexOf(QLatin1Char('('));
    QString ret = signature.left(index + 1);

    signature = signature.mid(index + 1);
    signature.chop(1);

    std::function<void(QString& ret, QByteArray&)> appendNormalizeType =
            [](QString& ret, QByteArray& typeName){
        int typeId = QMetaType::type(typeName.constData());
        typeName = QMetaType::typeName(typeId);
        ret += QString::fromUtf8(typeName);
        ret += ',';
        typeName.clear();
    };

    int brackets = 0;
    int parentheses = 0;
    int pointyBrackets = 0;
    QByteArray typeName;
    for (QChar ch : signature)
    {
        char c = ch.toLatin1();
        switch (c)
        {
        case '[':
            brackets += 1;
            break;

        case ']':
            brackets -= 1;
            break;

        case '(':
            parentheses += 1;
            break;

        case ')':
            parentheses -= 1;
            break;

        case '<':
            pointyBrackets += 1;
            break;

        case '>':
            pointyBrackets -= 1;
            break;

        case ',':
        {
            if ((brackets != 0)
                || (parentheses != 0)
                || (pointyBrackets != 0))
            {
                break;
            }

            appendNormalizeType(ret, typeName);
            continue;
        }

        default:
            break;
        }

        typeName += c;
    }
    appendNormalizeType(ret, typeName);

    if (ret.endsWith(',')) {
        ret.chop(1);
    }
    ret += ')';
    ret = QMetaObject::normalizedSignature(ret.toUtf8().constData());
    return ret;
}

QByteArray convertSignalName(QByteArray signal)
{
    signal.prepend(QByteArray::number(QSIGNAL_CODE));
#ifndef QT_NO_DEBUG
    const char* location = QLOCATION;
    size_t locationLen = 1 + strlen(&(location[1]));
    QByteArray l(location, int(locationLen));
    signal += l;
#endif
    return signal;
}

QByteArray convertSlotName(QByteArray slot)
{
    const char* location =
#ifdef QT_NO_DEBUG
            nullptr;
#else
            QLOCATION;
#endif
    size_t locationLen = 1 + strlen(&(location[1]));
    QByteArray l(location, int(locationLen));
    slot.prepend(QByteArray::number(QSLOT_CODE));
    slot += l;
    return slot;
}

QMutex* kpfMutex()
{
    return kpfCoreImpl.mutex();
}
