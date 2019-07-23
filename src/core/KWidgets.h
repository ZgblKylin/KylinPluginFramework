#pragma once
#include <QtGui/QtGui>
#include <QtWidgets/QtWidgets>
#include <Kpf/Kpf.h>

static const QString TAG_ITEMS = QStringLiteral("Items");
static const QString TAG_MARGINS = QStringLiteral("Margins");
static const QString TAG_STRETCH = QStringLiteral("Stretch");
static const QString TAG_LABEL = QStringLiteral("Label");
static const QString TAG_FIELD = QStringLiteral("Field");
static const QString TAG_ROW = QStringLiteral("Row");
static const QString TAG_COLUMN = QStringLiteral("Column");
static const QString TAG_ROWSPAN = QStringLiteral("RowSpan");
static const QString TAG_COLUMNSPAN = QStringLiteral("ColumnSpan");
static const QString TAG_TAB = QStringLiteral("Tab");
static const QString TAG_MENU = QStringLiteral("Menu");
static const QString TAG_ACTION = QStringLiteral("Action");
static const QString TAG_SEPARATOR = QStringLiteral("Separator");
static const QString TAG_WIDGET = QStringLiteral("Widget");
static const QString TAG_MENUBAR = QStringLiteral("MenuBar");
static const QString TAG_TOOLBARS = QStringLiteral("ToolBars");
static const QString TAG_CENTRALWIDGET = QStringLiteral("CentralWidget");
static const QString TAG_STATUSBAR = QStringLiteral("StatusBar");
static const QString TAG_INDEX = QStringLiteral("Index");
static const QString TAG_ICON = QStringLiteral("Icon");

class KHBoxLayout : public QHBoxLayout
{
    Q_OBJECT
public:
    explicit KHBoxLayout(QWidget* parent = nullptr);
    Q_SLOT bool init(const QDomElement& config);
};
KPF_REGISTER_CLASS(KHBoxLayout, QHBoxLayout)

class KVBoxLayout : public QVBoxLayout
{
    Q_OBJECT
public:
    explicit KVBoxLayout(QWidget* parent = nullptr);
    Q_SLOT bool init(const QDomElement& config);
};
KPF_REGISTER_CLASS(KVBoxLayout, QVBoxLayout)

class KFormLayout : public QFormLayout
{
    Q_OBJECT
public:
    explicit KFormLayout(QWidget* parent = nullptr);
    Q_SLOT bool init(QDomElement config);
};
KPF_REGISTER_CLASS(KFormLayout, QFormLayout)

class KGridLayout : public QGridLayout
{
    Q_OBJECT
public:
    explicit KGridLayout(QWidget* parent = nullptr);
    Q_SLOT bool init(const QDomElement& config);
};
KPF_REGISTER_CLASS(KGridLayout, QGridLayout)

class KStackedLayout : public QStackedLayout
{
    Q_OBJECT
public:
    explicit KStackedLayout(QWidget* parent = nullptr);
    Q_SLOT bool init(const QDomElement& config);
};
KPF_REGISTER_CLASS(KStackedLayout, QStackedLayout)

class KStackedWidget : public QStackedWidget
{
    Q_OBJECT
public:
    explicit KStackedWidget(QWidget* parent = nullptr);
    Q_SLOT bool init(const QDomElement& config);
};
KPF_REGISTER_CLASS(KStackedWidget, QStackedWidget)

class KTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit KTabWidget(QWidget* parent = nullptr);
    Q_SLOT bool init(const QDomElement& config);
};
KPF_REGISTER_CLASS(KTabWidget, QTabWidget)

class KListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit KListWidget(QWidget* parent = nullptr);
    Q_SLOT bool init(const QDomElement& config);
};
KPF_REGISTER_CLASS(KListWidget, QListWidget)

class KTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit KTableWidget(QWidget* parent = nullptr);
    Q_SLOT bool init(const QDomElement& config);
};
KPF_REGISTER_CLASS(KTableWidget, QTableWidget)

class KAction : public QAction
{
    Q_OBJECT
public:
    explicit KAction(QObject* parent = nullptr);
    Q_SLOT bool init(const QDomElement& config);
};
KPF_REGISTER_CLASS(KAction, QAction)

class KWidgetAction : public QWidgetAction
{
    Q_OBJECT
public:
    explicit KWidgetAction(QObject* parent = nullptr);
    Q_SLOT bool init(QDomElement config);
};
KPF_REGISTER_CLASS(KWidgetAction, QWidgetAction)

class KMenuBar : public QMenuBar
{
    Q_OBJECT
public:
    explicit KMenuBar(QWidget* parent = nullptr);
    Q_SLOT bool init(const QDomElement& config);
};
KPF_REGISTER_CLASS(KMenuBar, QMenuBar)

class KToolBar : public QToolBar
{
    Q_OBJECT
public:
    explicit KToolBar(QWidget* parent = nullptr);
    Q_SLOT bool init(const QDomElement& config);
};
KPF_REGISTER_CLASS(KToolBar, QToolBar)

class KToolButton : public QToolButton
{
    Q_OBJECT
public:
    explicit KToolButton(QWidget* parent = nullptr);
    Q_SLOT bool init(const QDomElement& config);
};
KPF_REGISTER_CLASS(KToolButton, QToolButton)

class KMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit KMainWindow(QWidget* parent = nullptr, Qt::WindowFlags flags = nullptr);
    Q_SLOT bool init(QDomElement config);
    Q_SIGNAL void closed();
    // QWidget interface
protected:
    void closeEvent(QCloseEvent*event);
};
KPF_REGISTER_CLASS(KMainWindow, QMainWindow)
