#include "KWidgets.h"
#include "KpfPrivate.h"

void initMargins(QLayout* layout, const QDomElement& config)
{
    QString value = config.attribute(TAG_MARGINS);
    if (value.isEmpty()) {
        return;
    }

    bool ok = false;
    int margin = value.toInt(&ok, 10);
    if (ok)
    {
        layout->setContentsMargins(margin, margin, margin, margin);
        return;
    }

    QStringList margins = value.split(',');
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

QList<Kpf::Object*> initObjects(QObject* parent, QDomElement config)
{
    QList<Kpf::Object*> objects;
    for (QDomElement child = config.firstChildElement(Kpf::TAG_CLASS);
         !child.isNull();
         child = child.nextSiblingElement(Kpf::TAG_CLASS))
    {
        QSharedPointer<Kpf::Object> object = kpfObject
                                             .createObject(child, parent)
                                             .toStrongRef();
        if (!object) {
            continue;
        }
        config.setAttribute(Kpf::KEY_ALREADYEXIST, object->name);
        object->object->setProperty(Kpf::KEY_ALREADYEXIST.toUtf8().constData(),
                                    object->name);
        if (!object->object->inherits("QWidget")
            && !object->object->inherits("QLayout"))
        {
            continue;
        }

        objects << object.data();
    }
    return objects;
}

bool initLayoutParent(QLayout* layout)
{
    QWidget* wParent = qobject_cast<QWidget*>(layout->parent());
    QLayout* lParent = qobject_cast<QLayout*>(layout->parent());
    if (wParent) {
        wParent->setLayout(layout);
    } else if (lParent) {
        lParent->addItem(layout);
    } else {
        return false;
    }
    return true;
}
bool initBoxLayout(QBoxLayout* layout, const QDomElement& config)
{
    if (!initLayoutParent(layout)) {
        return false;
    }

    initMargins(layout, config);

    QList<Kpf::Object*> objects = initObjects(layout, config);
    for (Kpf::Object* object : objects)
    {
        QWidget* widget = qobject_cast<QWidget*>(object->object);
        QLayout* subLayout = qobject_cast<QLayout*>(object->object);

        int stretch = object->object->property(TAG_STRETCH.toUtf8().constData()).toInt();

        if (widget)
        {
            layout->addWidget(widget, stretch);
        }
        else if (subLayout)
        {
            subLayout->setParent(nullptr);
            layout->addLayout(subLayout, stretch);
        }
    }

    return true;
}

void addMenuItem(QObject* parent, QDomElement config)
{
    QMenuBar* menuBar = qobject_cast<QMenuBar*>(parent);
    QToolBar* toolBar = qobject_cast<QToolBar*>(parent);
    QToolButton* toolButton = qobject_cast<QToolButton*>(parent);
    QAction* action = qobject_cast<QAction*>(parent);
    QMenu* menu = qobject_cast<QMenu*>(parent);
    if (!menuBar && !toolBar && !toolButton && !action && !menu) {
        return;
    }

    // QMenu子节点，允许嵌套在QMenuBar、QToolButton、QAction、QMenu下
    if (config.tagName() == TAG_MENU)
    {
        if (!menuBar && !toolButton && !action && !menu) {
            return;
        }

        QMenu* subMenu = kpfObject.createObject<QMenu>(config, parent);
        if (!subMenu) {
            return;
        }
        config.setAttribute(Kpf::KEY_ALREADYEXIST, subMenu->objectName());
        subMenu->setProperty(Kpf::KEY_ALREADYEXIST.toUtf8().constData(),
                             subMenu->objectName());

        QString text = config.attribute(Kpf::KEY_TEXT);
        subMenu->setTitle(text);

        if (menuBar) { // menuBar
            menuBar->addMenu(subMenu);
        } else if (toolButton) { // toolButton
            toolButton->setMenu(subMenu);
        } else if (action) { // action
            action->setMenu(subMenu);
        } else { // menu
            menu->addMenu(subMenu);
        }

        for (QDomElement item = config.firstChildElement();
             !item.isNull();
             item = item.nextSiblingElement())
        {
            addMenuItem(subMenu, item);
        }
    }
    // QAction子节点，允许嵌套在QMenuBar、QToolBar、QToolButton、QMenu下
    else if (config.tagName() == TAG_ACTION)
    {
        if (!menuBar && !toolBar && !toolButton && !menu) {
            return;
        }

        QAction* subAction = kpfObject.createObject<QAction>(config, parent);
        if (!subAction) {
            return;
        }
        config.setAttribute(Kpf::KEY_ALREADYEXIST, subAction->objectName());
        subAction->setProperty(Kpf::KEY_ALREADYEXIST.toUtf8().constData(),
                               subAction->objectName());

        QString text = config.attribute(Kpf::KEY_TEXT);
        subAction->setText(text);

        if (menuBar) { // menuBar
            menuBar->addAction(subAction);
        } else if (toolBar) { // toolBar
            toolBar->addAction(subAction);
        } else if (toolButton) { // toolButton
            toolButton->addAction(subAction);
        } else { // menu
            menu->addAction(subAction);
        }


        for (QDomElement item = config.firstChildElement();
             !item.isNull();
             item = item.nextSiblingElement())
        {
            addMenuItem(subAction, item);
        }
    }
    // Separator子节点，允许嵌套在QMenuBar、QToolBar、QMenu下
    else if (config.tagName() == TAG_SEPARATOR)
    {
        if (menuBar) { // menuBar
            menuBar->addSeparator();
        } else if (toolBar) { // toolBar
            toolBar->addSeparator();
        } else if (menu) { // menu
            menu->addSeparator();
        }
    }
    // Widget子节点，允许嵌套在QMenuBar、QToolBar、QToolButton、QMenu下
    else if (config.tagName() == TAG_WIDGET)
    {
        if (!menuBar && !toolBar && !toolButton && !menu) {
            return;
        }

        QString name = config.attribute(Kpf::KEY_NAME);
        QWidgetAction* widgetAction = kpfObject.createObject<QWidgetAction>(
                                          QStringLiteral("widgetAction_")
                                          + name,
                                          QStringLiteral("QWidgetAction"),
                                          config,
                                          parent);
        if (!widgetAction) {
            return;
        }

        if (menuBar) { // menuBar
            menuBar->addAction(widgetAction);
        } else if (toolBar) { // toolBar
            toolBar->addAction(widgetAction);
        } else if (toolButton) { // toolButton
            toolButton->addAction(widgetAction);
        } else { // menu
            menu->addAction(widgetAction);
        }
    }
}

void initMenu(QObject* parent, const QDomElement& config)
{
    for (QDomElement item = config.firstChildElement();
         !item.isNull();
         item = item.nextSiblingElement())
    {
        addMenuItem(parent, item);
    }
}

KHBoxLayout::KHBoxLayout(QWidget* parent)
    : QHBoxLayout(parent)
{
}

bool KHBoxLayout::init(const QDomElement& config)
{
    return initBoxLayout(this, config);
}

KVBoxLayout::KVBoxLayout(QWidget* parent)
    : QVBoxLayout(parent)
{
}

bool KVBoxLayout::init(const QDomElement& config)
{
    return initBoxLayout(this, config);
}

KFormLayout::KFormLayout(QWidget* parent)
    : QFormLayout(parent)
{
}

bool KFormLayout::init(QDomElement config)
{
    if (!initLayoutParent(this)) {
        return false;
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
    for (QDomElement child = config.firstChildElement(TAG_FIELD);
         !child.isNull();
         child = child.nextSiblingElement(TAG_FIELD))
    {
        QDomElement fieldConfig = child.firstChildElement(Kpf::TAG_CLASS);
        if (fieldConfig.isNull()) {
            continue;
        }
        QSharedPointer<Kpf::Object> object = kpfObject
                                             .createObject(fieldConfig, this)
                                             .toStrongRef();
        if (!object) {
            continue;
        }
        fieldConfig.setAttribute(Kpf::KEY_ALREADYEXIST, object->name);
        object->object->setProperty(Kpf::KEY_ALREADYEXIST.toUtf8().constData(),
                                    object->name);

        Object obj;
        obj.widget = qobject_cast<QWidget*>(object->object);
        obj.layout = qobject_cast<QLayout*>(object->object);
        if (obj.layout) {
            obj.layout->setParent(nullptr);
        }
        if (!obj.widget || !obj.layout) {
            continue;
        }

        QDomElement labelConfig = child.firstChildElement(TAG_LABEL);
        if (!labelConfig.isNull())
        {
            obj.label.text = labelConfig.attribute(Kpf::KEY_TEXT);
            obj.widget = kpfObject.createObject<QWidget>(labelConfig, this);
            if (obj.widget)
            {
                labelConfig.setAttribute(Kpf::KEY_ALREADYEXIST,
                                         obj.widget->objectName());
                obj.widget->setProperty(Kpf::KEY_ALREADYEXIST
                                        .toUtf8().constData(),
                                        obj.widget->objectName());
            }
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

bool KGridLayout::init(const QDomElement& config)
{
    if (!initLayoutParent(this)) {
        return false;
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

        if (widget)
        {
            addWidget(widget, row, column, rowSpan, columnSpan);
        }
        else if (layout)
        {
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

bool KStackedLayout::init(const QDomElement& config)
{
    if (!initLayoutParent(this)) {
        return false;
    }

    initMargins(this, config);

    QList<Kpf::Object*> objects = initObjects(this, config);
    for (Kpf::Object* object : objects)
    {
        QWidget* widget = qobject_cast<QWidget*>(object->object);
        if (widget) {
            addWidget(widget);
        }
    }

    return true;
}

KStackedWidget::KStackedWidget(QWidget* parent)
    : QStackedWidget(parent)
{
}

bool KStackedWidget::init(const QDomElement& config)
{
    QList<Kpf::Object*> objects = initObjects(this, config);
    for (Kpf::Object* object : objects)
    {
        QWidget* widget = qobject_cast<QWidget*>(object->object);
        if (widget) {
            addWidget(widget);
        }
    }

    return true;
}

KTabWidget::KTabWidget(QWidget* parent)
    : QTabWidget(parent)
{
}

bool KTabWidget::init(const QDomElement& config)
{
    QList<Kpf::Object*> objects = initObjects(this, config);
    for (Kpf::Object* object : objects)
    {
        QWidget* widget = qobject_cast<QWidget*>(object->object);
        if (!widget) {
            continue;
        }
        QString title = widget->property(TAG_TAB.toUtf8().constData())
                        .toString();
        if (title.isEmpty()) {
            title = widget->objectName();
        }
        addTab(widget, title);
    }

    return true;
}

KListWidget::KListWidget(QWidget* parent)
    : QListWidget(parent)
{
}

bool KListWidget::init(const QDomElement& config)
{
    QList<Kpf::Object*> objects = initObjects(this, config);
    for (Kpf::Object* object : objects)
    {
        QWidget* widget = qobject_cast<QWidget*>(object->object);
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

bool KTableWidget::init(const QDomElement& config)
{
    QList<Kpf::Object*> objects = initObjects(this, config);
    for (Kpf::Object* object : objects)
    {
        QWidget* widget = qobject_cast<QWidget*>(object->object);
        if (!widget) {
            continue;
        }

        int row = widget->property(TAG_ROW.toUtf8().constData()).toInt();
        int col = widget->property(TAG_ROW.toUtf8().constData()).toInt();
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

bool KAction::init(const QDomElement &config)
{
    QIcon icon = QIcon(QFileInfo(QApplication::applicationDirPath()
                                 + QDir::separator()
                                 + config.attribute(TAG_ICON))
                       .absoluteFilePath());
    if(!icon.isNull()){
        setIcon(icon);
    }
    return true;
}

KMenuBar::KMenuBar(QWidget* parent)
    : QMenuBar(parent)
{
}

bool KMenuBar::init(const QDomElement& config)
{
    initMenu(this, config);
    return true;
}

KToolBar::KToolBar(QWidget* parent)
    : QToolBar(parent)
{
}

bool KToolBar::init(const QDomElement& config)
{
    initMenu(this, config);
    return true;
}

KToolButton::KToolButton(QWidget* parent)
    : QToolButton(parent)
{
}

bool KToolButton::init(const QDomElement& config)
{
    initMenu(this, config);
    return true;
}

KWidgetAction::KWidgetAction(QObject* parent)
    : QWidgetAction(parent)
{
}

bool KWidgetAction::init(QDomElement config)
{
    QWidget* widget = kpfObject.createObject<QWidget>(config, this);
    if (!widget) {
        return false;
    }
    config.setAttribute(Kpf::KEY_ALREADYEXIST, widget->objectName());
    widget->setProperty(Kpf::KEY_ALREADYEXIST.toUtf8().constData(),
                        widget->objectName());

    setDefaultWidget(widget);
    return true;

}

KMainWindow::KMainWindow(QWidget* parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
{
}

bool KMainWindow::init(QDomElement config)
{
    QDomElement menuBarConfig = config.firstChildElement(TAG_MENUBAR);
    QMenuBar* menuBar = kpfObject.createObject<QMenuBar>(menuBarConfig,
                                                         this);
    if (menuBar)
    {
        menuBarConfig.setAttribute(Kpf::KEY_ALREADYEXIST, menuBar->objectName());
        menuBar->setProperty(Kpf::KEY_ALREADYEXIST.toUtf8().constData(),
                             menuBar->objectName());
        setMenuBar(menuBar);
    }

    QDomElement toolBarConfig = config.firstChildElement(TAG_TOOLBARS);
    for (QDomElement subToolBar = toolBarConfig.firstChildElement(Kpf::TAG_CLASS);
         !subToolBar.isNull();
         subToolBar = subToolBar.nextSiblingElement(Kpf::TAG_CLASS))
    {
        QToolBar* toolBar = kpfObject.createObject<QToolBar>(subToolBar, this);
        if (toolBar)
        {
            subToolBar.setAttribute(Kpf::KEY_ALREADYEXIST,
                                    toolBar->objectName());
            toolBar->setProperty(Kpf::KEY_ALREADYEXIST.toUtf8().constData(),
                                 toolBar->objectName());
            addToolBar(toolBar);
        }
    }

    QDomElement centralWidgetConfig = config.firstChildElement(TAG_CENTRALWIDGET);
    QWidget* centralWidget = kpfObject.createObject<QWidget>(centralWidgetConfig, this);
    if (centralWidget)
    {
        centralWidgetConfig.setAttribute(Kpf::KEY_ALREADYEXIST,
                                         centralWidget->objectName());
        centralWidget->setProperty(Kpf::KEY_ALREADYEXIST.toUtf8().constData(),
                                   centralWidget->objectName());
        setCentralWidget(centralWidget);
    }

    QDomElement statusBarConfig = config.firstChildElement(TAG_STATUSBAR);
    QStatusBar* statusBar = kpfObject.createObject<QStatusBar>(statusBarConfig, this);
    if (statusBar)
    {
        statusBarConfig.setAttribute(Kpf::KEY_ALREADYEXIST,
                                     statusBar->objectName());
        statusBar->setProperty(Kpf::KEY_ALREADYEXIST.toUtf8().constData(),
                               statusBar->objectName());
        setStatusBar(statusBar);
    }

    return true;
}

void KMainWindow::closeEvent(QCloseEvent* event)
{
    QMainWindow::closeEvent(event);
    emit closed();
}
