#include "Test.h"
#include <QtWidgets/QtWidgets>

Test::Test(QObject *parent) : QObject(parent)
{
}

Test::~Test()
{
    QThread::sleep(5);
}

bool Test::init(const QDomElement&)
{
    qInfo() << "Test: initialized at thread" << QThread::currentThread()
            << ", it's belonging thread is" << thread()
            << ", main thread is" << qApp->thread();

    return true;
}

void Test::start()
{
    qInfo() << "Test: start() called at thread" << QThread::currentThread()
            << ", it's belonging thread is" << thread()
            << ", main thread is" << qApp->thread();

    qInfo() << "Test: emit testSignal at initialization with arg=32";
    emit testSignal(32);

    qInfo() << "Test: send TestEvent at initialization with arg=64";
    kSendEvent(TestEvent, 64);

    qInfo() << "Test: post TestEvent at initialization with arg=128";
    kPostEvent(TestEvent, 128);
}

void Test::testSlot(quint32 value)
{
    qInfo() << "Test: testSlot() received with arg =" << value
            << ", at thread" << QThread::currentThread()
            << ", it's belonging thread is" << thread()
            << ", main thread is" << qApp->thread();
}
