#ifndef LOGDISPLAYWIDGET_H
#define LOGDISPLAYWIDGET_H

#include <QtCore/QtCore>
#include <QtWidgets/QtWidgets>
#include "LogDisplayBuffer.h"
#include "LogDisplayPage.h"

namespace Log {
namespace Impl {

class LogDisplayWidgetPrivate;
/**
 * @brief 日志显示组件
 */
class LOGDISPLAYWIDGET_SHARED_EXPORT LogDisplayWidget : public QGroupBox
{
    Q_OBJECT
    /* ======== All properties will be set to all pages, replacing their own value ======== */
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
    explicit LogDisplayWidget(QWidget* parent = nullptr);
    virtual ~LogDisplayWidget();

    void setBuffer(LogDisplayBuffer* buffer);

    bool columnEnabled(LogDisplayPage::Column column) const;
    void setColumnEnabled(LogDisplayPage::Column column, bool enabled);

    bool columnEnabled(const QString& category, LogDisplayPage::Column column) const;
    void setColumnEnabled(const QString& category, LogDisplayPage::Column column, bool enabled);

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
    virtual bool event(QEvent* event) override;

    Q_DECLARE_PRIVATE(LogDisplayWidget)
    LogDisplayWidgetPrivate* d_ptr;
};
} // namespace Log
} // namespace Impl

#endif // LOGDISPLAYWIDGET_H
