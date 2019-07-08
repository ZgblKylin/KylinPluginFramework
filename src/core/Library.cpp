#include "Library.h"
#include "ClassImpl.h"
#include "ObjectImpl.h"

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
    kpfCInformation("Kpf") << "Library"
                           << fileInfo.absoluteFilePath()
                           << "unloaded";
}
