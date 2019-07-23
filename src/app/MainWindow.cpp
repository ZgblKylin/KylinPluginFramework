#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::init(const QDomElement& config)
{
    qInfo() << "MainWindow: initialized at thread" << QThread::currentThread()
            << ",it's belonging thread is" << thread()
            << ", main thread is" << qApp->thread();

    QDomElement centralWidgetConfig = config.firstChildElement(QStringLiteral("CentralWidget"));
    QWidget* centralWidget = kpfObject.createObject<QWidget>(centralWidgetConfig, this);
    if (centralWidget) {
        setCentralWidget(centralWidget);
    }

    return true;
}

QString MainWindow::testEvent(QString text)
{
    qInfo() << "MainWindow: receive testEvent with arg =" << text
            << "invoke thread is" << QThread::currentThread()
            << "it's belonging thread is" << thread()
            << ", main thread is" << qApp->thread();
    return text;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
