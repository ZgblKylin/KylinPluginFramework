#ifndef KWIDGETS_H
#define KWIDGETS_H

#include <QtGui/QtGui>
#include <QtWidgets/QtWidgets>
#include <Kpf/Kpf.h>
#include "ObjectImpl.h"

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
static const QString TAG_TOOLBAR = QStringLiteral("ToolBar");
static const QString TAG_CENTRALWIDGET = QStringLiteral("CentralWidget");
static const QString TAG_STATUSBAR = QStringLiteral("StatusBar");
static const QString TAG_INDEX = QStringLiteral("Index");
static const QString TAG_ICON = QStringLiteral("Icon");

class KHBoxLayout : public QHBoxLayout
{
    Q_OBJECT
public:
    explicit KHBoxLayout(QWidget* parent = nullptr);
    Q_SLOT bool init(const QJsonObject& config);
};
KPF_REGISTER_CLASS(KHBoxLayout, QHBoxLayout)

class KVBoxLayout : public QVBoxLayout
{
    Q_OBJECT
public:
    explicit KVBoxLayout(QWidget* parent = nullptr);
    Q_SLOT bool init(const QJsonObject& config);
};
KPF_REGISTER_CLASS(KVBoxLayout, QVBoxLayout)

class KFormLayout : public QFormLayout
{
    Q_OBJECT
public:
    explicit KFormLayout(QWidget* parent = nullptr);
    Q_SLOT bool init(const QJsonObject& config);
};
KPF_REGISTER_CLASS(KFormLayout, QFormLayout)

class KGridLayout : public QGridLayout
{
    Q_OBJECT
public:
    explicit KGridLayout(QWidget* parent = nullptr);
    Q_SLOT bool init(const QJsonObject& config);
};
KPF_REGISTER_CLASS(KGridLayout, QGridLayout)

class KStackedLayout : public QStackedLayout
{
    Q_OBJECT
public:
    explicit KStackedLayout(QWidget* parent = nullptr);
    Q_SLOT bool init(const QJsonObject& config);
};
KPF_REGISTER_CLASS(KStackedLayout, QStackedLayout)

class KStackedWidget : public QStackedWidget
{
    Q_OBJECT
public:
    explicit KStackedWidget(QWidget* parent = nullptr);
    Q_SLOT bool init(const QJsonObject& config);
};
KPF_REGISTER_CLASS(KStackedWidget, QStackedWidget)

class KTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit KTabWidget(QWidget* parent = nullptr);
    Q_SLOT bool init(const QJsonObject& config);
};
KPF_REGISTER_CLASS(KTabWidget, QTabWidget)

class KListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit KListWidget(QWidget* parent = nullptr);
    Q_SLOT bool init(const QJsonObject& config);
};
KPF_REGISTER_CLASS(KListWidget, QListWidget)

class KTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit KTableWidget(QWidget* parent = nullptr);
    Q_SLOT bool init(const QJsonObject& config);
};
KPF_REGISTER_CLASS(KTableWidget, QTableWidget)

class KAction : public QAction
{
    Q_OBJECT
public:
    explicit KAction(QObject* parent = nullptr);
    Q_SIGNAL void triggered(int);
    Q_SLOT bool init(const QJsonObject& config);
private:
    int index;
};
KPF_REGISTER_CLASS(KAction, QAction)

class KWidgetAction : public QWidgetAction
{
    Q_OBJECT
public:
    explicit KWidgetAction(QObject* parent = nullptr);
    Q_SLOT bool init(const QJsonObject& config);
};
KPF_REGISTER_CLASS(KWidgetAction, QWidgetAction)

class KMenuBar : public QMenuBar
{
    Q_OBJECT
public:
    explicit KMenuBar(QWidget* parent = nullptr);
    Q_SLOT bool init(const QJsonObject& config);
private:
    void addMenuItem(QObject* parent, const QJsonObject& config);
};
KPF_REGISTER_CLASS(KMenuBar, QMenuBar)

class KToolBar : public QToolBar
{
    Q_OBJECT
public:
    explicit KToolBar(QWidget* parent = nullptr);
    Q_SLOT bool init(const QJsonObject& config);
private:
    void addMenuItem(QObject* parent, const QJsonObject& config);
};
KPF_REGISTER_CLASS(KToolBar, QToolBar)

class KToolButton : public QToolButton
{
    Q_OBJECT
public:
    explicit KToolButton(QWidget* parent = nullptr);
    Q_SLOT bool init(const QJsonObject& config);
private:
    void addMenuItem(QObject* parent, const QJsonObject& config);
};
KPF_REGISTER_CLASS(KToolButton, QToolButton)

class KMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit KMainWindow(QWidget* parent = nullptr, Qt::WindowFlags flags = 0);
    Q_SLOT bool init(const QJsonObject& config);
    Q_SIGNAL void closed();
    // QWidget interface
protected:
    void closeEvent(QCloseEvent*event);
};
KPF_REGISTER_CLASS(KMainWindow, QMainWindow)

KPF_REGISTER(QStandardItemModel)
KPF_REGISTER(QOffscreenSurface)
KPF_REGISTER(QTextDocument)
KPF_REGISTER(QDoubleValidator)
KPF_REGISTER(QIntValidator)
KPF_REGISTER(QRegExpValidator)

KPF_REGISTER(QFileSystemModel)
KPF_REGISTER(QGraphicsBlurEffect)
KPF_REGISTER(QGraphicsColorizeEffect)
KPF_REGISTER(QGraphicsDropShadowEffect)
KPF_REGISTER(QGraphicsOpacityEffect)
KPF_REGISTER(QGraphicsTextItem)
KPF_REGISTER(QGraphicsProxyWidget)
KPF_REGISTER(QGraphicsScene)
KPF_REGISTER(QGraphicsRotation)
KPF_REGISTER(QGraphicsScale)
KPF_REGISTER(QGraphicsView)
KPF_REGISTER(QGraphicsWidget)
KPF_REGISTER(QColumnView)
KPF_REGISTER(QDataWidgetMapper)
KPF_REGISTER(QItemDelegate)
KPF_REGISTER(QListView)
KPF_REGISTER(QStyledItemDelegate)
KPF_REGISTER(QTableView)
KPF_REGISTER(QTreeView)
KPF_REGISTER(QTreeWidget)
KPF_REGISTER(QGesture)
KPF_REGISTER(QPanGesture)
KPF_REGISTER(QPinchGesture)
KPF_REGISTER(QSwipeGesture)
KPF_REGISTER(QTapAndHoldGesture)
KPF_REGISTER(QTapGesture)
KPF_REGISTER(QWidget)
KPF_REGISTER(QKeyEventTransition)
KPF_REGISTER(QMouseEventTransition)
KPF_REGISTER(QCommonStyle)
KPF_REGISTER(QProxyStyle)
KPF_REGISTER(QCompleter)
KPF_REGISTER(QSystemTrayIcon)
KPF_REGISTER(QUndoGroup)
KPF_REGISTER(QUndoStack)
KPF_REGISTER(QUndoView)
KPF_REGISTER(QAbstractScrollArea)
KPF_REGISTER(QAbstractSlider)
KPF_REGISTER(QAbstractSpinBox)
KPF_REGISTER(QButtonGroup)
KPF_REGISTER(QCalendarWidget)
KPF_REGISTER(QCheckBox)
KPF_REGISTER(QComboBox)
KPF_REGISTER(QCommandLinkButton)
KPF_REGISTER(QDateEdit)
KPF_REGISTER(QDateTimeEdit)
KPF_REGISTER(QTimeEdit)
KPF_REGISTER(QDial)
KPF_REGISTER(QDialogButtonBox)
KPF_REGISTER(QDockWidget)
KPF_REGISTER(QFocusFrame)
KPF_REGISTER(QFontComboBox)
KPF_REGISTER(QFrame)
KPF_REGISTER(QGroupBox)
KPF_REGISTER(QKeySequenceEdit)
KPF_REGISTER(QLabel)
KPF_REGISTER(QLCDNumber)
KPF_REGISTER(QLineEdit)
KPF_REGISTER(QMdiArea)
KPF_REGISTER(QMdiSubWindow)
KPF_REGISTER(QMenu)
KPF_REGISTER(QPlainTextEdit)
KPF_REGISTER(QProgressBar)
KPF_REGISTER(QPushButton)
KPF_REGISTER(QRadioButton)
KPF_REGISTER(QScrollArea)
KPF_REGISTER(QScrollBar)
KPF_REGISTER(QSlider)
KPF_REGISTER(QDoubleSpinBox)
KPF_REGISTER(QSpinBox)
KPF_REGISTER(QSplashScreen)
KPF_REGISTER(QSplitter)
KPF_REGISTER(QStatusBar)
KPF_REGISTER(QTabBar)
KPF_REGISTER(QTextBrowser)
KPF_REGISTER(QTextEdit)
KPF_REGISTER(QToolBox)
KPF_REGISTER(QColorDialog)
KPF_REGISTER(QDialog)
KPF_REGISTER(QErrorMessage)
KPF_REGISTER(QFileDialog)
KPF_REGISTER(QFontDialog)
KPF_REGISTER(QInputDialog)
KPF_REGISTER(QMessageBox)
KPF_REGISTER(QProgressDialog)
KPF_REGISTER(QWizard)
KPF_REGISTER(QWizardPage)

#endif // KWIDGETS_H
