#include <QtCore/QtGlobal>
#include <Windows.h>
#include <DbgHelp.h>
#include <TlHelp32.h>
#ifdef Q_CC_MINGW
#  include <cxxabi.h>
#endif
#include "Dump.h"

inline void displayError()
{
    DWORD dwErrorCode = GetLastError();
    LPTSTR lpMsgBuf = NULL;
    QString msg = QStringLiteral("Error code: %1").arg(dwErrorCode);
    if(FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                      NULL,
                      dwErrorCode,
                      0,
                      LPTSTR(&lpMsgBuf),
                      0,
                      NULL) != 0)
    {
        msg += QStringLiteral(" - ");
        msg += QString::fromWCharArray(lpMsgBuf).remove('\r').remove('\n');
    }
    qDebug() << msg;
}

char* unDecorate(const char* symbol)
{
#ifdef Q_CC_MINGW // demangle by MinGW
    int status = 0;
    QString gccSymbol = QStringLiteral("_%1").arg(symbol);
    char* demangled = ::abi::__cxa_demangle(gccSymbol.toUtf8().constData(),
                                            nullptr, nullptr, &status);
    if (status == 0) {
        return demangled;
    } else {
        return nullptr;
    }
#elif defined(Q_CC_MSVC) // undecorate by MSVC
    PCHAR strName = new CHAR[65536];
    memset(strName, 0, 65536);
    DWORD dwNameLen = ::UnDecorateSymbolName(symbol,
                                             strName,
                                             65536,
                                             UNDNAME_COMPLETE);
    if((dwNameLen == 0) || (strncmp(symbol, strName, dwNameLen) == 0)) {
        return nullptr;
    } else {
        return strName;
    }
#else
    return nullptr;
#endif
}

QStringList stackWalk(HANDLE hProcess, HANDLE hThread, DWORD dwThreadId, HANDLE hCurrentThread)
{
    static const quint32 MaxRecursionCount = 256;

    QStringList ret;

    CONTEXT Context;
    memset(&Context, 0, sizeof(Context));
    Context.ContextFlags = CONTEXT_FULL;

    if(hThread == hCurrentThread)
        RtlCaptureContext(&Context);
    else
    {
        SuspendThread(hThread);
        if (GetThreadContext(hThread, &Context) == FALSE)
        {
            qDebug() << "Cannot get context for thread" << dwThreadId;
            displayError();
            ResumeThread(hThread);
            return ret;
        }
    }

    STACKFRAME StackFrame;
    memset(&StackFrame, 0, sizeof(STACKFRAME));

    DWORD MachineType;
#if defined(Q_PROCESSOR_X86_32)
    MachineType = IMAGE_FILE_MACHINE_I386;
    StackFrame.AddrPC.Offset = Context.Eip;
    StackFrame.AddrPC.Mode = AddrModeFlat;
    StackFrame.AddrFrame.Offset = Context.Ebp;
    StackFrame.AddrFrame.Mode = AddrModeFlat;
    StackFrame.AddrStack.Offset = Context.Esp;
    StackFrame.AddrStack.Mode = AddrModeFlat;
#elif defined(Q_PROCESSOR_X86_64)
    MachineType = IMAGE_FILE_MACHINE_AMD64;
    StackFrame.AddrPC.Offset = Context.Rip;
    StackFrame.AddrPC.Mode = AddrModeFlat;
    StackFrame.AddrFrame.Offset = Context.Rsp;
    StackFrame.AddrFrame.Mode = AddrModeFlat;
    StackFrame.AddrStack.Offset = Context.Rsp;
    StackFrame.AddrStack.Mode = AddrModeFlat;
#elif defined(Q_PROCESSOR_IA64)
    MachineType = IMAGE_FILE_MACHINE_IA64;
    StackFrame.AddrPC.Offset = Context.StIIP;
    StackFrame.AddrPC.Mode = AddrModeFlat;
    StackFrame.AddrFrame.Offset = Context.IntSp;
    StackFrame.AddrFrame.Mode = AddrModeFlat;
    StackFrame.AddrBStore.Offset = Context.RsBSP;
    StackFrame.AddrBStore.Mode = AddrModeFlat;
    StackFrame.AddrStack.Offset = Context.IntSp;
    StackFrame.AddrStack.Mode = AddrModeFlat;
#else
    ret << "Platform not supported!";
    return ret;
#endif

    quint32 curRecursionCount = 0;
    bool recursion = false;
    while(StackWalk(MachineType,
                    hProcess,
                    hThread,
                    &StackFrame,
                    &Context,
                    PREAD_PROCESS_MEMORY_ROUTINE(ReadProcessMemory),
                    SymFunctionTableAccess,
                    SymGetModuleBase,
                    NULL) == TRUE)
    {
        if(StackFrame.AddrFrame.Offset == 0)
            break;

        if(StackFrame.AddrPC.Offset == StackFrame.AddrReturn.Offset)
        {
            if(recursion && (curRecursionCount > MaxRecursionCount))
            {
                recursion = false;
                qDebug() << QStringLiteral("Endless callstack at 0x%1!")
                            .arg(StackFrame.AddrPC.Offset, 0, 16, QLatin1Char('0'));
                break;
            }
            recursion = true;
            curRecursionCount++;
        }

        BYTE Symbol[sizeof(IMAGEHLP_SYMBOL) + 1024];
        memset(Symbol, 0, sizeof(Symbol));
        PIMAGEHLP_SYMBOL pSymbol = reinterpret_cast<PIMAGEHLP_SYMBOL>(Symbol);
        pSymbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
        pSymbol->MaxNameLength = 1024;

#ifdef Q_OS_WIN64
        DWORD64 dwOffsetFromSymbol = 0;
#else
        DWORD dwOffsetFromSymbol = 0;
#endif
        if(SymGetSymFromAddr(hProcess,
                             StackFrame.AddrPC.Offset,
                             &dwOffsetFromSymbol,
                             pSymbol) == FALSE) {
            ret << QStringLiteral("Cannot get function symbol from AddrPC 0x%1")
                   .arg(StackFrame.AddrPC.Offset, 0, 16, QLatin1Char('0'));
            displayError();
            continue;
        }

        char* symbolName = unDecorate(pSymbol->Name);

        IMAGEHLP_LINE Line;
        memset(&Line, 0, sizeof(Line));
        Line.SizeOfStruct = sizeof(IMAGEHLP_LINE);
        DWORD dwOffsetFromLine = 0;

        bool bLine = true;
        if(SymGetLineFromAddr(hProcess,
                              StackFrame.AddrPC.Offset,
                              &dwOffsetFromLine,
                              &Line) == FALSE) {
//            qDebug("Cannot get line info from addr 0x%lX", StackFrame.AddrPC.Offset);
//            ts << QStringLiteral("Cannot get line info from addr 0x%1")
//                  .arg(StackFrame.AddrPC.Offset, 0, 16, QLatin1Char('0'))
//               << endl;
//            displayError();
            bLine = false;
        }

        if(bLine)
        {
            ret << QStringLiteral("%1:%2 - %3")
                   .arg(Line.FileName)
                   .arg(Line.LineNumber)
                   .arg(symbolName?symbolName:pSymbol->Name);
        }
        else
        {
            ret << QString(symbolName?symbolName:pSymbol->Name);
        }
        free(symbolName);
    }

    if(hThread != hCurrentThread)
        ResumeThread(hThread);

    return ret;
}

QString dump(const QDir& dir)
{
    QProcess process ;
    process.setProgram(QStringLiteral("procdump.exe"));
    process.setArguments({
                             QStringLiteral("-ma"),
                             QString::number(qApp->applicationPid()),
                             QStringLiteral("-accepteula")
                         });
    process.setWorkingDirectory(dir.absolutePath());
    process.start();
    process.waitForFinished(-1);
    return QString::fromUtf8(process.readAll());
}

QStringList stackWalk()
{
    QStringList ret;

    HANDLE hProcess = GetCurrentProcess();
    HANDLE hCurrentThread = GetCurrentThread();

    if(SymInitialize(hProcess,
                     NULL,
                     TRUE) == FALSE)
    {
        ret << "Cannot initialize symbol";
        displayError();
        return ret;
    }

    ret += stackWalk(hProcess,
                     hCurrentThread,
                     GetCurrentThreadId(),
                     hCurrentThread);

    if(SymCleanup(hProcess) == FALSE)
    {
        qDebug() << "SymCleanup failed symbol";
        displayError();
        return ret;
    }
    return ret;
}
