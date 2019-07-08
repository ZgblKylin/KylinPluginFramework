#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(this, &MainWindow::test,
            this, &MainWindow::testSlot,
            Qt::DirectConnection);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::init(const QJsonObject& config)
{
    qDebug() << "MainWindow::init"
             << config
             << "Invoke thread" << QThread::currentThread()
             << "This thread" << thread();
    return true;
}

void MainWindow::testSlot(QString text)
{
    qDebug() << "MainWindow::test" << text
             << "Invoke thread" << QThread::currentThread()
             << "This thread" << thread();
}

QString MainWindow::testEvent(QString text)
{
    qDebug() << "MainWindow::testEvent" << text
             << "Invoke thread" << QThread::currentThread()
             << "This thread" << thread();
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
