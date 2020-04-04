#pragma once
#include <Kpf/Common.h>
#include <Kpf/InvokeHelper.h>
#include <Kpf/KpfCore.h>

// ======== API声明 ========
// 在类内部声明发布事件，eventId为事件名称
#define KPF_PUBEVENT(eventId)
// 在类内部声明订阅事件，eventId为事件名称，method为对应的函数
// 若编译出错，检查method输入/输出是否无法转换为QVariant
#define KPF_SUBEVENT(eventId, method)
// 同步发布事件的宏函数，eventId为事件名称，后续为输入参数列表，返回QVariant
#define kSendEvent(eventId, ...)
// 异步发布事件的宏函数
#define kPostEvent(eventId, ...)
// ======== API声明 ========

#undef KPF_PUBEVENT
#define KPF_PUBEVENT(eventId) \
struct KPF_PUBEVENT_##eventId : public Kpf::ObjectEvent \
{ \
    KPF_PUBEVENT_##eventId() \
    { \
        kpfEvent.registerPubEvent(#eventId, this); \
    } \
}; \
KPF_PUBEVENT_##eventId kpf_pubevent_##eventId;

//Q_SIGNAL QVariant kpfSendEvent_##eventId (const QVariantList& args);
//Q_SIGNAL QVariant kpfPostEvent_##eventId (const QVariantList& args);

#undef KPF_SUBEVENT
#define KPF_SUBEVENT(eventId, method) \
struct KPF_SUBEVENT_##eventId : public Kpf::ObjectEvent \
{ \
    template<typename T> \
    KPF_SUBEVENT_##eventId(T* self) \
    { \
        func = [self](const QVariantList& args)->QVariant \
        { \
            using KpfSelf = typename std::remove_reference<typename std::remove_cv<decltype(*self)>::type>::type; \
            Kpf::EventHelper<decltype(&KpfSelf::method)> helper = \
                Kpf::EventHelper<decltype(&KpfSelf::method)>(self, &KpfSelf::method); \
            return helper.invoke(args); \
        }; \
        kpfEvent.registerSubEvent(#eventId, this); \
    } \
}; \
KPF_SUBEVENT_##eventId kpf_subevent_##eventId = KPF_SUBEVENT_##eventId(this);

#undef kSendEvent
#define kSendEvent(eventId, ...) \
kpf_pubevent_##eventId.send(Kpf::packParams(__VA_ARGS__));

#undef kPostEvent
#define kPostEvent(eventId, ...) \
kpf_pubevent_##eventId.post(Kpf::packParams(__VA_ARGS__));

namespace Kpf {

/*
 * copy from qobjectdefs_impl.h
 * trick to set the return value of a slot that works even if the signal or the slot returns void
 * to be used like     function(), ApplyReturnValue<ReturnType>(ret.data()urn_value)
 * if function() returns a value, the operator,(T, ApplyReturnValue<ReturnType>) is called, but if it
 * returns void, the builtin one is used without an error.
 */
template <typename T>
struct ApplyReturnValue
{
    void *data;
    explicit ApplyReturnValue(void *data_) : data(data_) {}
};
template<typename T, typename U>
void operator,(const T &value, const ApplyReturnValue<U> &container)
{
    if (container.data) {
        *reinterpret_cast<U*>(container.data) = value;
    }
}
template<typename T, typename U>
void operator,(T &&value, const ApplyReturnValue<U> &container)
{
    if (container.data) {
        *reinterpret_cast<U*>(container.data) = value;
    }
}

template<typename Result>
inline QVariant resultGenerator()
{
    int typeId = qMetaTypeId<Result>();
    return QVariant(QVariant::Type(typeId));
}
template<>
inline QVariant resultGenerator<void>()
{
    return QVariant();
}

template<typename Handler>
struct EventHelper;

#define REMOVE_CONST_REF(T) typename std::remove_const<typename std::remove_reference<T>::type>::type

template<typename Result, typename KpfSelf>
struct EventHelper<Result(KpfSelf::*)(void)>
{
    using Func = Result(KpfSelf::*)(void);
    EventHelper(KpfSelf* self, Func func) : s(self), f(func) {}
    QVariant invoke(const QVariantList& args) {
        Q_UNUSED(args)
        QVariant ret = resultGenerator<Result>();
        emit (s->*f)(), ApplyReturnValue<Result>(ret.data());
        return ret;
    }
protected:
    KpfSelf* s;
    Func f;
};

template<typename Result, typename KpfSelf, typename Arg1>
struct EventHelper<Result(KpfSelf::*)(Arg1)>
{
    using Func = Result(KpfSelf::*)(Arg1);
    EventHelper(KpfSelf* self, Func func) : s(self), f(func) {}
    QVariant invoke(const QVariantList& args) {
        QVariant ret = resultGenerator<Result>();
        if (args.count() >= 1) {
            emit (s->*f)(args.at(0).value<REMOVE_CONST_REF(Arg1)>()),
                    ApplyReturnValue<Result>(ret.data());
        }
        return ret;
    }
protected:
    KpfSelf* s;
    Func f;
};

template<typename Result, typename KpfSelf, typename Arg1, typename Arg2>
struct EventHelper<Result(KpfSelf::*)(Arg1, Arg2)>
{
    using Func = Result(KpfSelf::*)(Arg1, Arg2);
    EventHelper(KpfSelf* self, Func func) : s(self), f(func) {}
    QVariant invoke(const QVariantList& args) {
        QVariant ret = resultGenerator<Result>();
        if (args.count() >= 2) {
            emit (s->*f)(args.at(0).value<REMOVE_CONST_REF(Arg1)>(),
                         args.at(1).value<REMOVE_CONST_REF(Arg2)>()),
                    ApplyReturnValue<Result>(ret.data());
        }
        return ret;
    }
protected:
    KpfSelf* s;
    Func f;
};

template<typename Result, typename KpfSelf, typename Arg1, typename Arg2, typename Arg3>
struct EventHelper<Result(KpfSelf::*)(Arg1, Arg2, Arg3)>
{
    using Func = Result(KpfSelf::*)(Arg1, Arg2, Arg3);
    EventHelper(KpfSelf* self, Func func) : s(self), f(func) {}
    QVariant invoke(const QVariantList& args) {
        QVariant ret = resultGenerator<Result>();
        if (args.count() >= 3) {
            emit (s->*f)(args.at(0).value<REMOVE_CONST_REF(Arg1)>(),
                         args.at(1).value<REMOVE_CONST_REF(Arg2)>(),
                         args.at(2).value<REMOVE_CONST_REF(Arg3)>()),
                    ApplyReturnValue<Result>(ret.data());
        }
        return ret;
    }
protected:
    KpfSelf* s;
    Func f;
};

template<typename Result, typename KpfSelf, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
struct EventHelper<Result(KpfSelf::*)(Arg1, Arg2, Arg3, Arg4)>
{
    using Func = Result(KpfSelf::*)(Arg1, Arg2, Arg3, Arg4);
    EventHelper(KpfSelf* self, Func func) : s(self), f(func) {}
    QVariant invoke(const QVariantList& args) {
        QVariant ret = resultGenerator<Result>();
        if (args.count() >= 4) {
            emit (s->*f)(args.at(0).value<REMOVE_CONST_REF(Arg1)>(),
                         args.at(1).value<REMOVE_CONST_REF(Arg2)>(),
                         args.at(2).value<REMOVE_CONST_REF(Arg3)>(),
                         args.at(3).value<REMOVE_CONST_REF(Arg4)>()),
                    ApplyReturnValue<Result>(ret.data());
        }
        return ret;
    }
protected:
    KpfSelf* s;
    Func f;
};

template<typename Result, typename KpfSelf, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
struct EventHelper<Result(KpfSelf::*)(Arg1, Arg2, Arg3, Arg4, Arg5)>
{
    using Func = Result(KpfSelf::*)(Arg1, Arg2, Arg3, Arg4, Arg5);
    EventHelper(KpfSelf* self, Func func) : s(self), f(func) {}
    QVariant invoke(const QVariantList& args) {
        QVariant ret = resultGenerator<Result>();
        if (args.count() >= 5) {
            emit (s->*f)(args.at(0).value<REMOVE_CONST_REF(Arg1)>(),
                         args.at(1).value<REMOVE_CONST_REF(Arg2)>(),
                         args.at(2).value<REMOVE_CONST_REF(Arg3)>(),
                         args.at(3).value<REMOVE_CONST_REF(Arg4)>(),
                         args.at(4).value<REMOVE_CONST_REF(Arg5)>()),
                    ApplyReturnValue<Result>(ret.data());
        }
        return ret;
    }
protected:
    KpfSelf* s;
    Func f;
};

template<typename Result, typename KpfSelf, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
struct EventHelper<Result(KpfSelf::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)>
{
    using Func = Result(KpfSelf::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6);
    EventHelper(KpfSelf* self, Func func) : s(self), f(func) {}
    QVariant invoke(const QVariantList& args) {
        QVariant ret = resultGenerator<Result>();
        if (args.count() >= 6) {
            emit (s->*f)(args.at(0).value<REMOVE_CONST_REF(Arg1)>(),
                         args.at(1).value<REMOVE_CONST_REF(Arg2)>(),
                         args.at(2).value<REMOVE_CONST_REF(Arg3)>(),
                         args.at(3).value<REMOVE_CONST_REF(Arg4)>(),
                         args.at(4).value<REMOVE_CONST_REF(Arg5)>(),
                         args.at(5).value<REMOVE_CONST_REF(Arg6)>()),
                    ApplyReturnValue<Result>(ret.data());
        }
        return ret;
    }
protected:
    KpfSelf* s;
    Func f;
};

template<typename Result, typename KpfSelf, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
struct EventHelper<Result(KpfSelf::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)>
{
    using Func = Result(KpfSelf::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7);
    EventHelper(KpfSelf* self, Func func) : s(self), f(func) {}
    QVariant invoke(const QVariantList& args) {
        QVariant ret = resultGenerator<Result>();
        if (args.count() >= 7) {
            emit (s->*f)(args.at(0).value<REMOVE_CONST_REF(Arg1)>(),
                         args.at(1).value<REMOVE_CONST_REF(Arg2)>(),
                         args.at(2).value<REMOVE_CONST_REF(Arg3)>(),
                         args.at(3).value<REMOVE_CONST_REF(Arg4)>(),
                         args.at(4).value<REMOVE_CONST_REF(Arg5)>(),
                         args.at(5).value<REMOVE_CONST_REF(Arg6)>(),
                         args.at(6).value<REMOVE_CONST_REF(Arg7)>()),
                    ApplyReturnValue<Result>(ret.data());
        }
        return ret;
    }
protected:
    KpfSelf* s;
    Func f;
};

template<typename Result, typename KpfSelf, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>
struct EventHelper<Result(KpfSelf::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)>
{
    using Func = Result(KpfSelf::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8);
    EventHelper(KpfSelf* self, Func func) : s(self), f(func) {}
    QVariant invoke(const QVariantList& args) {
        QVariant ret = resultGenerator<Result>();
        if (args.count() >= 8) {
            emit (s->*f)(args.at(0).value<REMOVE_CONST_REF(Arg1)>(),
                         args.at(1).value<REMOVE_CONST_REF(Arg2)>(),
                         args.at(2).value<REMOVE_CONST_REF(Arg3)>(),
                         args.at(3).value<REMOVE_CONST_REF(Arg4)>(),
                         args.at(4).value<REMOVE_CONST_REF(Arg5)>(),
                         args.at(5).value<REMOVE_CONST_REF(Arg6)>(),
                         args.at(6).value<REMOVE_CONST_REF(Arg7)>(),
                         args.at(7).value<REMOVE_CONST_REF(Arg8)>()),
                    ApplyReturnValue<Result>(ret.data());
        }
        return ret;
    }
protected:
    KpfSelf* s;
    Func f;
};

template<typename Result, typename KpfSelf, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9>
struct EventHelper<Result(KpfSelf::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9)>
{
    using Func = Result(KpfSelf::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9);
    EventHelper(KpfSelf* self, Func func) : s(self), f(func) {}
    QVariant invoke(const QVariantList& args) {
        QVariant ret = resultGenerator<Result>();
        if (args.count() >= 9) {
            emit (s->*f)(args.at(0).value<REMOVE_CONST_REF(Arg1)>(),
                         args.at(1).value<REMOVE_CONST_REF(Arg2)>(),
                         args.at(2).value<REMOVE_CONST_REF(Arg3)>(),
                         args.at(3).value<REMOVE_CONST_REF(Arg4)>(),
                         args.at(4).value<REMOVE_CONST_REF(Arg5)>(),
                         args.at(5).value<REMOVE_CONST_REF(Arg6)>(),
                         args.at(6).value<REMOVE_CONST_REF(Arg7)>(),
                         args.at(7).value<REMOVE_CONST_REF(Arg8)>(),
                         args.at(8).value<REMOVE_CONST_REF(Arg9)>()),
                    ApplyReturnValue<Result>(ret.data());
        }
        return ret;
    }
protected:
    KpfSelf* s;
    Func f;
};
} // namespace Kpf
