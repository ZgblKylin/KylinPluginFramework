#ifndef TEST_H
#define TEST_H

#include <Kpf/Kpf.h>

#if defined(TEST_LIBRARY)
#  define TEST_EXPORT Q_DECL_EXPORT
#else
#  define TEST_EXPORT Q_DECL_IMPORT
#endif

class TEST_EXPORT Test : public QObject
{
    Q_OBJECT
    KPF_PUBEVENT(TestEvent)
    KPF_SUBEVENT(TestSlot, testSlot)

public:
    explicit Test(QObject *parent = nullptr);
    ~Test();

    Q_SLOT bool init(const QDomElement& config);

    Q_SLOT void start();

    Q_SIGNAL void testSignal(quint32 value);
    Q_SLOT void testSlot(quint32 value);
};
KPF_REGISTER(Test)

#endif // TEST_H
