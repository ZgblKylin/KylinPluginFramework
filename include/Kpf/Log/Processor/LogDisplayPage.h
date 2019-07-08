#ifndef LOGDISPLAYPAGE_H
#define LOGDISPLAYPAGE_H

#include <QtWidgets/QtWidgets>
#include "../Log.h"
#include "LogDisplayBuffer.h"

class QMenu;

namespace Log {
namespace Impl {
class LogDisplayPagePrivate;
// widget for displaying log message of single category
class LOGDISPLAYWIDGET_SHARED_EXPORT LogDisplayPage : public QTableView
{
    Q_OBJECT

    Q_ENUMS(Column)
    // Category displayed
    Q_PROPERTY(QString category READ category WRITE setCategory)
    // max count to be shown(before filter), default is 10000
    Q_PROPERTY(int maxCount READ maxCount WRITE setMaxCount)
    // foreground color for log text
    Q_PROPERTY(QColor debugForeground READ debugForeground WRITE setDebugForeground)
    Q_PROPERTY(QColor informationForeground READ informationForeground WRITE setInformationForeground)
    Q_PROPERTY(QColor warningForeground READ warningForeground WRITE setWarningForeground)
    Q_PROPERTY(QColor criticalForeground READ criticalForeground WRITE setCriticalForeground)
    Q_PROPERTY(QColor fatalForeground READ fatalForeground WRITE setFatalForeground)
    // background color for log text
    Q_PROPERTY(QColor debugBackground READ debugBackground WRITE setDebugBackground)
    Q_PROPERTY(QColor informationBackground READ informationBackground WRITE setInformationBackground)
    Q_PROPERTY(QColor warningBackground READ warningBackground WRITE setWarningBackground)
    Q_PROPERTY(QColor criticalBackground READ criticalBackground WRITE setCriticalBackground)
    Q_PROPERTY(QColor fatalBackground READ fatalBackground WRITE setFatalBackground)

public:
    // Column indexes, correspond to LogMessage
    enum Column
    {
        DateTime = 0,
        Level,
        Process,
        ThreadId,
        ThreadPtr,
        FileLine,
        Function,
        Message
    };

    explicit LogDisplayPage(QWidget* parent = 0);
    virtual ~LogDisplayPage();

    void setBuffer(LogDisplayBuffer* buffer);

    bool columnEnabled(Column column) const;
    void setColumnEnabled(Column column, bool enabled);

    QString category() const;
    void setCategory(const QString& name);

    int maxCount() const;
    void setMaxCount(int value);

    QColor debugForeground() const;
    void setDebugForeground(QColor color);
    QColor informationForeground() const;
    void setInformationForeground(QColor color);
    QColor warningForeground() const;
    void setWarningForeground(QColor color);
    QColor criticalForeground() const;
    void setCriticalForeground(QColor color);
    QColor fatalForeground() const;
    void setFatalForeground(QColor color);

    QColor debugBackground() const;
    void setDebugBackground(QColor color);
    QColor informationBackground() const;
    void setInformationBackground(QColor color);
    QColor warningBackground() const;
    void setWarningBackground(QColor color);
    QColor criticalBackground() const;
    void setCriticalBackground(QColor color);
    QColor fatalBackground() const;
    void setFatalBackground(QColor color);

protected:
    Q_DECLARE_PRIVATE(LogDisplayPage)
    LogDisplayPagePrivate* d_ptr;
};
} // namespace LogDisplayPage
} // namespace Log

#endif // LOGDISPLAYPAGE_H
