#include "Test.h"

Test::Test(QObject *parent) : QObject(parent)
{
}

Test::~Test()
{
    QThread::sleep(5);
}

bool Test::init(const QJsonObject& config)
{
    qDebug() << "test::init"
             << config
             << "Invoke thread" << QThread::currentThread()
             << "This thread" << thread();

    return true;
}

void Test::start()
{
    qDebug() << "Emit testSignal" << 32;
    emit testSignal(32);

    qDebug() << "Send TestEvent" << 64;
    kSendEvent(TestEvent, 64);

    qDebug() << "Post TestEvent" << 128;
    kPostEvent(TestEvent, 128);
}

void Test::testSlot(quint32 value)
{
    qDebug() << "Test Slot Event" << value;
}
