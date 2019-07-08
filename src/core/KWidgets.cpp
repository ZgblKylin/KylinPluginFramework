#include "KWidgets.h"
#include "KpfPrivate.h"

void initMargins(QLayout* layout, const QJsonObject& config)
{
    QJsonValue value = config.value(TAG_MARGINS);
    if (value.isDouble())
    {
        int margin = value.toInt();
        layout->setContentsMargins(margin, margin, margin, margin);
        return;
    }

    QStringList margins = value.toString().split(',');
    switch (margins.count())
    {
    case 1:
    {
        int margin = margins.at(0).trimmed().toInt();
        layout->setContentsMargins(margin, margin, margin, margin);
    }
        break;
    case 4:
    {
        int left = margins.at(0).trimmed().toInt();
        int top = margins.at(1).trimmed().toInt();
        int right = margins.at(2).trimmed().toInt();
        int bottom = margins.at(3).trimmed().toInt();
        layout->setContentsMargins(left, top, right, bottom);
    }
        break;
    default:
        break;
    }
}

QList<Kpf::Object*> initObjects(QObject* parent, const QJsonObject& config)
{
    QList<Kpf::Object*> objects;
    if (!config.contains(TAG_ITEMS)) {
        return objects;
    }
    QJsonArray array = config.value(TAG_ITEMS).toArray();
    for (auto it = array.begin(); it != array.end(); ++it)
    {
        if (!(*it).isObject()) {
            continue;
        }
        QJsonObject objectConfig = (*it).toObject();

        QSharedPointer<Kpf::Object> object = kpfObject
                                             .createObject(objectConfig, parent)
                                             .toStrongRef();
        if (!object) {
            continue;
        }
        if (!object->object->inherits("QWidget")
            && !object->object->inherits("QLayout"))
        {
            kpfObject.destroyObject(object->name);
            continue;
        }

        objects << object.data();
    }
    return objects;
}

bool initBoxLayout(QBoxLayout* layout, const QJsonObject& config)
{
    QWidget* wParent = qobject_cast<QWidget*>(layout->parent());
    QLayout* lParent = qobject_cast<QLayout*>(layout->parent());
    if (!wParent && !lParent) {
        return false;
    }
    layout->setParent(nullptr);
    if (wParent) {
        wParent->setLayout(layout);
    } else {
        lParent->addItem(layout);
    }

    initMargins(layout, config);

    QList<Kpf::Object*> objects = initObjects(layout, config);

    for (Kpf::Object* object : objects)
    {
        QWidget* widget = qobject_cast<QWidget*>(object->object);
        QLayout* subLayout = qobject_cast<QLayout*>(object->object);

        int stretch = object->object->property(TAG_STRETCH.toUtf8().constData()).toInt();

        if (widget) {
            layout->addWidget(widget, stretch);
        } else if (subLayout) {
            subLayout->setParent(nullptr);
            layout->addLayout(subLayout, stretch);
        } else {
            kpfObject.destroyObject(object->name);
        }
    }

    return true;
}

KHBoxLayout::KHBoxLayout(QWidget* parent)
    : QHBoxLayout(parent)
{
}

bool KHBoxLayout::init(const QJsonObject& config)
{
    return initBoxLayout(this, config);
}

KVBoxLayout::KVBoxLayout(QWidget* parent)
    : QVBoxLayout(parent)
{
}

bool KVBoxLayout::init(const QJsonObject& config)
{
    return initBoxLayout(this, config);
}

KFormLayout::KFormLayout(QWidget* parent)
    : QFormLayout(parent)
{
}

bool KFormLayout::init(const QJsonObject& config)
{
    QWidget* wParent = qobject_cast<QWidget*>(parent());
    QLayout* lParent = qobject_cast<QLayout*>(parent());
    if (!wParent && !lParent) {
        return false;
    }
    setParent(nullptr);
    if (wParent) {
        wParent->setLayout(this);
    } else {
        lParent->addItem(this);
    }

    initMargins(this, config);

    struct Object
    {
        struct Label
        {
            QString text;
            QWidget* widget = nullptr;
        };
        Label label;
        QWidget* widget = nullptr;
        QLayout* layout = nullptr;
    };

    QList<Object> objects;
    if (!config.contains(TAG_ITEMS)) {
        return true;
    }

    QJsonArray array = config.value(TAG_ITEMS).toArray();
    for (auto it = array.begin(); it != array.end(); ++it)
    {
        if (!(*it).isObject()) {
            continue;
        }
        QJsonObject child = (*it).toObject();

        if (!child.contains(TAG_FIELD)) {
            continue;
        }
        QJsonObject fieldConfig = child.value(TAG_FIELD).toObject();
        QSharedPointer<Kpf::Object> object = kpfObject
                                             .createObject(fieldConfig, this)
                                             .toStrongRef();
        if (!object) {
            continue;
        }
        Object obj;
        obj.widget = qobject_cast<QWidget*>(object->object);
        obj.layout = qobject_cast<QLayout*>(object->object);
        if (obj.layout) {
            obj.layout->setParent(nullptr);
        }
        if (!obj.widget || !obj.layout)
        {
            kpfObject.destroyObject(object->name);
            continue;
        }

        if (child.contains(TAG_LABEL))
        {
            QJsonValue labelValue = child.value(TAG_FIELD);
            obj.label.text = labelValue.toString();
            QJsonObject labelConfig = labelValue.toObject();
            obj.widget = kpfObject.createObject<QWidget>(labelConfig, this);
        }

        objects.append(obj);
    }

    for (Object object : objects)
    {
        if (object.label.widget)
        {
            if (object.widget) {
                addRow(object.label.widget, object.widget);
            } else {
                addRow(object.label.widget, object.layout);
            }
        }
        else
        {
            if (object.widget) {
                addRow(object.label.text, object.widget);
            } else {
                addRow(object.label.text, object.layout);
            }
        }
    }

    return true;
}

KGridLayout::KGridLayout(QWidget* parent)
    : QGridLayout(parent)
{
}

bool KGridLayout::init(const QJsonObject& config)
{
    QWidget* wParent = qobject_cast<QWidget*>(parent());
    QLayout* lParent = qobject_cast<QLayout*>(parent());
    if (!wParent && !lParent) {
        return false;
    }
    setParent(nullptr);
    if (wParent) {
        wParent->setLayout(this);
    } else {
        lParent->addItem(this);
    }

    initMargins(this, config);

    QList<Kpf::Object*> objects = initObjects(this, config);

    for (Kpf::Object* object : objects)
    {
        QWidget* widget = qobject_cast<QWidget*>(object->object);
        QLayout* layout = qobject_cast<QLayout*>(object->object);

        int row = object->object->property(TAG_ROW.toUtf8().constData()).toInt();
        int column = object->object->property(TAG_COLUMN.toUtf8().constData()).toInt();
        int rowSpan = object->object->property(TAG_ROWSPAN.toUtf8().constData()).toInt();
        rowSpan = std::max(1, rowSpan);
        int columnSpan = object->object->property(TAG_ROWSPAN.toUtf8().constData()).toInt();
        columnSpan = std::max(1, columnSpan);

        if (widget) {
            addWidget(widget, row, column, rowSpan, columnSpan);
        } else if (layout) {
            layout->setParent(nullptr);
            addLayout(layout, row, column, rowSpan, columnSpan);
        }
    }

    return true;
}

KStackedLayout::KStackedLayout(QWidget* parent)
    : QStackedLayout(parent)
{
}

bool KStackedLayout::init(const QJsonObject& config)
{
    QWidget* wParent = qobject_cast<QWidget*>(parent());
    QLayout* lParent = qobject_cast<QLayout*>(parent());
    if (!wParent && !lParent) {
        return false;
    }
    setParent(nullptr);
    if (wParent) {
        wParent->setLayout(this);
    } else {
        lParent->addItem(this);
    }

    initMargins(this, config);

    QList<Kpf::Object*> objects = initObjects(this, config);

    for (Kpf::Object* object : objects)
    {
        QWidget* widget = qobject_cast<QWidget*>(object->object);
        if (widget) {
            addWidget(widget);
        } else {
            kpfObject.destroyObject(object->name);
        }
    }

    return true;
}

KStackedWidget::KStackedWidget(QWidget* parent)
    : QStackedWidget(parent)
{
}

bool KStackedWidget::init(const QJsonObject& config)
{
    QList<Kpf::Object*> objects = initObjects(this, config);

    for (Kpf::Object* object : objects)
    {
        QWidget* widget = qobject_cast<QWidget*>(object->object);
        if (widget) {
            addWidget(widget);
        } else {
            kpfObject.destroyObject(object->name);
        }
    }

    return true;
}

KTabWidget::KTabWidget(QWidget* parent)
    : QTabWidget(parent)
{
}

bool KTabWidget::init(const QJsonObject& config)
{
    QList<Kpf::Object*> objects = initObjects(this, config);

    for (Kpf::Object* object : objects)
    {
        QWidget* widget = qobject_cast<QWidget*>(object->object);
        if (widget)
        {
            QString title = widget->property(TAG_TAB.toUtf8().constData()).toString();
            addTab(widget, title);
        }
        else
        {
            kpfObject.destroyObject(object->name);
        }
    }

    return true;
}

KListWidget::KListWidget(QWidget* parent)
    : QListWidget(parent)
{
}

bool KListWidget::init(const QJsonObject& config)
{
    QJsonArray array = config.value(TAG_ITEMS).toArray();
    for (auto it = array.constBegin(); it != array.constEnd(); ++it)
    {
        if (!(*it).isObject()) {
            continue;
        }
        QJsonObject objectConfig = (*it).toObject();
        QWidget* widget = kpfObject.createObject<QWidget>(objectConfig, this);
        if (!widget) {
            continue;
        }
        QListWidgetItem* item = new QListWidgetItem;
        addItem(item);
        setItemWidget(item, widget);
    }

    return true;
}

KTableWidget::KTableWidget(QWidget* parent)
    : QTableWidget(parent)
{
}

bool KTableWidget::init(const QJsonObject& config)
{
    QJsonArray array = config.value(TAG_ITEMS).toArray();
    for (auto it = array.begin(); it != array.end(); ++it)
    {
        if (!(*it).isObject()) {
            continue;
        }
        QJsonObject objectConfig = (*it).toObject();

        int row = objectConfig.value(TAG_ROW).toInt();
        int col = objectConfig.value(TAG_COLUMN).toInt();

        QWidget* widget = kpfObject.createObject<QWidget>(objectConfig, this);
        if (!widget) {
            continue;
        }
        setRowCount(std::max(row+1, rowCount()));
        setColumnCount(std::max(col+1, columnCount()));
        setCellWidget(row, col, widget);
    }

    return true;
}

KAction::KAction(QObject* parent)
    : QAction(parent)
{
}

bool KAction::init(const QJsonObject &config)
{
    QIcon icon = QIcon(QApplication::applicationDirPath() + QDir::separator() + config.value(TAG_ICON).toString());
    if(!icon.isNull()){
        setIcon(icon);
    }

    index = config.value(TAG_INDEX).toInt();
    connect(this, &QAction::triggered, [this](){
        emit triggered(index);
    });
    return true;
}

KMenuBar::KMenuBar(QWidget* parent)
    : QMenuBar(parent)
{
}

bool KMenuBar::init(const QJsonObject& config)
{
    QJsonArray array = config.value(TAG_ITEMS).toArray();
    for (auto it = array.begin(); it != array.end(); ++it)
    {
        if ((*it).isObject()) {
            addMenuItem(this, (*it).toObject());
        }
    }
    return true;
}

void KMenuBar::addMenuItem(QObject* parent, const QJsonObject& config)
{
    QMenuBar* menuBar = qobject_cast<QMenuBar*>(parent);
    QAction* action = qobject_cast<QAction*>(parent);
    QMenu* menu = qobject_cast<QMenu*>(parent);
    if (!menuBar && !action && !menu) {
        return;
    }

    // QMenu子节点，允许嵌套在QMenuBar、QAction、QMenu下
    if (config.contains(TAG_MENU))
    {
        QMenu* subMenu = kpfObject.createObject<QMenu>(QString(),
                                                       QStringLiteral("QMenu"),
                                                       config,
                                                       parent);
        if (!subMenu) {
            return;
        }

        QString text = config.value(TAG_MENU).toVariant().toString();
        subMenu->setTitle(text);

        if (menuBar) { // menuBar
            menuBar->addMenu(subMenu);
        } else if (action) { // action
            action->setMenu(subMenu);
        } else { // menu
            menu->addMenu(subMenu);
        }

        QJsonArray array = config.value(TAG_ITEMS).toArray();
        for (auto it = array.begin(); it != array.end(); ++it)
        {
            if ((*it).isObject()) {
                addMenuItem(subMenu, (*it).toObject());
            }
        }
    }
    // QAction子节点，允许嵌套在QMenuBar、QMenu下
    else if (config.contains(TAG_ACTION))
    {
        if (!menuBar && !menu) {
            return;
        }

        QAction* subAction = kpfObject.createObject<QAction>(QString(),
                                                             QStringLiteral("QAction"),
                                                             config,
                                                             parent);
        if (!subAction) {
            return;
        }

        QString text = config.value(TAG_ACTION).toVariant().toString();
        subAction->setText(text);

        if (menuBar) { // menuBar
            menuBar->addAction(subAction);
        } else { // menu
            menu->addAction(subAction);
        }

        QJsonArray array = config.value(TAG_ITEMS).toArray();
        for (auto it = array.begin(); it != array.end(); ++it)
        {
            if ((*it).isObject()) {
                addMenuItem(subAction, (*it).toObject());
            }
        }
    }
    // Separator子节点，允许嵌套在QMenuBar、QMenu下
    else if (config.contains(TAG_SEPARATOR))
    {
        if (menuBar) { // menuBar
            menuBar->addSeparator();
        } else if (menu) { // menu
            menu->addSeparator();
        } else { // action
            return;
        }
    }
    // Widget子节点，允许嵌套在QMenuBar、QMenu下
    else if (config.contains(TAG_WIDGET))
    {
        if (!menuBar || !menu) {
            return;
        }

        QString name = config.value(Kpf::TAG_NAME).toString();
        QAction* widgetAction = kpfObject.createObject<QAction>(
                                    QStringLiteral("widgetAction_") + name,
                                    QStringLiteral("QWidgetAction"),
                                    config,
                                    parent);
        if (!widgetAction) {
            return;
        }

        if (menuBar) { // menuBar
            menuBar->addAction(widgetAction);
        } else { // menu
            menu->addAction(widgetAction);
        }
    }
}

KToolBar::KToolBar(QWidget* parent)
    : QToolBar(parent)
{
}

bool KToolBar::init(const QJsonObject& config)
{
    QJsonArray array = config.value(TAG_ITEMS).toArray();
    for (auto it = array.begin(); it != array.end(); ++it)
    {
        if ((*it).isObject()) {
            addMenuItem(this, (*it).toObject());
        }
    }
    return true;
}

void KToolBar::addMenuItem(QObject* parent, const QJsonObject& config)
{
    QToolBar* toolBar = qobject_cast<QToolBar*>(parent);
    QAction* action = qobject_cast<QAction*>(parent);
    QMenu* menu = qobject_cast<QMenu*>(parent);
    if (!toolBar && !action && !menu) {
        return;
    }

    // QMenu子节点，允许嵌套在QToolBar、QAction、QMenu下
    if (config.contains(TAG_MENU))
    {
        if (!action && !menu) {
            return;
        }

        QMenu* subMenu = kpfObject.createObject<QMenu>(QString(),
                                                       QStringLiteral("QMenu"),
                                                       config,
                                                       parent);
        if (!subMenu) {
            return;
        }

        QString text = config.value(TAG_MENU).toVariant().toString();
        subMenu->setTitle(text);

        if (action) { // action
            action->setMenu(subMenu);
        } else { // menu
            menu->addMenu(subMenu);
        }

        QJsonArray array = config.value(TAG_ITEMS).toArray();
        for (auto it = array.begin(); it != array.end(); ++it)
        {
            if ((*it).isObject()) {
                addMenuItem(subMenu, (*it).toObject());
            }
        }
    }
    // QAction子节点，允许嵌套在QToolBar、QMenu下
    else if (config.contains(TAG_ACTION))
    {
        if (!toolBar && !menu) {
            return;
        }

        QAction* subAction = kpfObject.createObject<QAction>(QString(),
                                                             QStringLiteral("QAction"),
                                                             config,
                                                             parent);
        if (!subAction) {
            return;
        }

        QString text = config.value(TAG_ACTION).toVariant().toString();
        subAction->setText(text);

        if (toolBar) { // toolbar
            toolBar->addAction(subAction);
        } else { // menu
            menu->addAction(subAction);
        }

        QJsonArray array = config.value(TAG_ITEMS).toArray();
        for (auto it = array.begin(); it != array.end(); ++it)
        {
            if ((*it).isObject()) {
                addMenuItem(subAction, (*it).toObject());
            }
        }
    }
    // Separator子节点，允许嵌套在QToolBar、QMenu下
    else if (config.contains(TAG_SEPARATOR))
    {
        if (!toolBar && !menu) {
            return;
        }

        if (toolBar) { // toolbar
            toolBar->addSeparator();
        } else { // menu
            menu->addSeparator();
        }
    }
    // Widget子节点，允许嵌套在QToolBar、QMenu下
    else if (config.contains(TAG_WIDGET))
    {
        if (!toolBar && !menu) {
            return;
        }

        QString name = config.value(Kpf::TAG_NAME).toString();
        QAction* widgetAction = kpfObject.createObject<QAction>(
                                    QStringLiteral("widgetAction_") + name,
                                    QStringLiteral("QWidgetAction"),
                                    config,
                                    parent);
        if (!widgetAction) {
            return;
        }

        if (toolBar) { // toolBar
            toolBar->addAction(widgetAction);
        } else { // menu
            menu->addAction(widgetAction);
        }
    }
}

KToolButton::KToolButton(QWidget* parent)
    : QToolButton(parent)
{
}

bool KToolButton::init(const QJsonObject& config)
{
    QJsonArray array = config.value(TAG_ITEMS).toArray();
    for (auto it = array.begin(); it != array.end(); ++it)
    {
        if ((*it).isObject()) {
            addMenuItem(this, (*it).toObject());
        }
    }
    return true;
}

void KToolButton::addMenuItem(QObject* parent, const QJsonObject& config)
{
    QToolButton* toolButton = qobject_cast<QToolButton*>(parent);
    QAction* action = qobject_cast<QAction*>(parent);
    QMenu* menu = qobject_cast<QMenu*>(parent);
    if (!toolButton && !action && !menu) {
        return;
    }

    // QMenu子节点，允许嵌套在QToolButton、QAction、QMenu下
    if (config.contains(TAG_MENU))
    {
        QMenu* subMenu = kpfObject.createObject<QMenu>(QString(),
                                                       QStringLiteral("QMenu"),
                                                       config,
                                                       parent);
        if (!subMenu) {
            return;
        }

        QString text = config.value(TAG_MENU).toVariant().toString();
        subMenu->setTitle(text);

        if (toolButton) { // QToolButton
            toolButton->setMenu(subMenu);
        } else if (action) { // action
            action->setMenu(subMenu);
        } else { // menu
            menu->addMenu(subMenu);
        }

        QJsonArray array = config.value(TAG_ITEMS).toArray();
        for (auto it = array.begin(); it != array.end(); ++it)
        {
            if ((*it).isObject()) {
                addMenuItem(subMenu, (*it).toObject());
            }
        }
    }
    // QAction子节点，允许嵌套在QToolButton、QMenu下
    else if (config.contains(TAG_ACTION))
    {
        if (!toolButton && !menu) {
            return;
        }

        QAction* subAction = kpfObject.createObject<QAction>(QString(),
                                                             QStringLiteral("QAction"),
                                                             config,
                                                             parent);
        if (!subAction) {
            return;
        }

        QString text = config.value(TAG_ACTION).toVariant().toString();
        subAction->setText(text);

        if (toolButton) { // toolbar
            toolButton->addAction(subAction);
        } else { // menu
            menu->addAction(subAction);
        }

        QJsonArray array = config.value(TAG_ITEMS).toArray();
        for (auto it = array.begin(); it != array.end(); ++it)
        {
            if ((*it).isObject()) {
                addMenuItem(subAction, (*it).toObject());
            }
        }
    }
    // Separator子节点，允许嵌套在QMenu下
    else if (config.contains(TAG_SEPARATOR))
    {
        if (!menu) {
            return;
        }
        menu->addSeparator();
    }
    // Widget子节点，允许嵌套在QToolButton、QMenu下
    else if (config.contains(TAG_WIDGET))
    {
        if (!toolButton && !menu) {
            return;
        }

        QString name = config.value(Kpf::TAG_NAME).toString();
        QAction* widgetAction = kpfObject.createObject<QAction>(
                                    QStringLiteral("widgetAction_") + name,
                                    QStringLiteral("QWidgetAction"),
                                    config,
                                    parent);
        if (!widgetAction) {
            return;
        }

        if (toolButton) { // QToolButton
            toolButton->addAction(widgetAction);
        } else { // menu
            menu->addAction(widgetAction);
        }
    }
}

KWidgetAction::KWidgetAction(QObject* parent)
    : QWidgetAction(parent)
{
}

bool KWidgetAction::init(const QJsonObject& config)
{
    QWidget* widget = kpfObject.createObject<QWidget>(config, this);
    if (!widget) {
        return false;
    }

    setDefaultWidget(widget);
    return true;

}

KMainWindow::KMainWindow(QWidget* parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
{
}

bool KMainWindow::init(const QJsonObject& config)
{
    QJsonObject menuBarConfig = config.value(TAG_MENUBAR).toObject();
    QMenuBar* menuBar = kpfObject.createObject<QMenuBar>(QString(),
                                                         QStringLiteral("QMenuBar"),
                                                         menuBarConfig,
                                                         this);
    if (menuBar) {
        setMenuBar(menuBar);
    }

    QJsonArray toolBarConfig = config.value(TAG_TOOLBAR).toArray();
    for (auto it = toolBarConfig.begin(); it != toolBarConfig.end(); ++it)
    {
        QToolBar* toolBar = kpfObject.createObject<QToolBar>(QString(),
                                                             QStringLiteral("QToolBar"),
                                                             (*it).toObject(),
                                                             this);
        if (toolBar) {
            addToolBar(toolBar);
        }
    }

    QJsonObject centralWidgetConfig = config.value(TAG_CENTRALWIDGET).toObject();
    QWidget* centralWidget = kpfObject.createObject<QWidget>(centralWidgetConfig, this);
    if (centralWidget) {
        setCentralWidget(centralWidget);
    }

    QJsonObject statusBarConfig = config.value(TAG_STATUSBAR).toObject();
    QStatusBar* statusBar = kpfObject.createObject<QStatusBar>(statusBarConfig, this);
    if (statusBar) {
        setStatusBar(statusBar);
    }

    return true;
}

void KMainWindow::closeEvent(QCloseEvent* event)
{
    QMainWindow::closeEvent(event);
    emit closed();
}
