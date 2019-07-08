#ifndef LIBRARY_H
#define LIBRARY_H

#include <Kpf/Kpf.h>

namespace Kpf {
struct MetaClassImpl;
struct ObjectImpl;
struct Library
{
    explicit Library(const QFileInfo& fileInfo);
    virtual ~Library();

    QFileInfo fileInfo;
    QLibrary library;
    QMap<QString, QWeakPointer<MetaClassImpl>> classes;
    QMap<QString, QWeakPointer<ObjectImpl>> objects;
};
} // namespace Kpf

#endif // LIBRARY_H
