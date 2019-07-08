#ifndef ILOGCOMMON_H
#define ILOGCOMMON_H

#include <QtCore/QtCore>
#include <QtXml/QtXml>
#include "Log.h"

namespace Log {
namespace Impl {
template<typename T>
void initCategoryFilter(const QDomElement& config, T* object)
{
    for (QDomElement child = config.firstChildElement(QStringLiteral("Category"));
         !child.isNull();
         child = child.nextSiblingElement(QStringLiteral("Category")))
    {
        object->setCategoryFilter(child.attribute(QStringLiteral("name")),
                                  child.attribute(QStringLiteral("filter")).toInt());
    }
}
} // namespace Impl
} // namespace Log

#endif // ILOGCOMMON_H
