#include "Library.h"
#include "KpfPrivate.h"

Kpf::Library::Library(const QFileInfo& info)
    : fileInfo(info),
      library(info.absoluteFilePath())
{
    library.setLoadHints(QLibrary::ResolveAllSymbolsHint
                         | QLibrary::ExportExternalSymbolsHint);
}

Kpf::Library::~Library()
{
    library.unload();
    qCInfo(kpf) << "Library"
                << fileInfo.absoluteFilePath()
                << "unloaded";
}
