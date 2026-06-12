#ifndef _TL_SUBJECT_H_
#define _TL_SUBJECT_H_

#include "BasicTypes.h"
#include <algorithm>
#include <functional>
#include <list>
#include <memory>

namespace WS
{
namespace Util
{
template <class VALUETYPE>
class Subject
{
public:
    using HANDLERPTR = void(VALUETYPE const&);
    using Observer = std::function<HANDLERPTR>;
    using ObsTuple = std::tuple<Observer*, const void*, const void*>;
    using ObserverList = std::list<ObsTuple>;

    void registerObserver(Observer& obs, const void* mptr, const void* obj)
    {
        mObservers.push_back(ObsTuple{&obs, mptr, obj});
    }

    void unregisterObserver(const void* mptr, const void* obj)
    {
        typename ObserverList::iterator iter{std::find_if(mObservers.begin(), mObservers.end(), [&mptr, &obj](ObsTuple obsTuple) -> bool
            { return (std::get<1>(obsTuple) == mptr) && (std::get<2>(obsTuple) == obj); })};
        if(iter != mObservers.end())
        {
            delete std::get<0>(*iter);
            mObservers.remove(*iter);
        }
    }

    void fireNotifications(VALUETYPE const& value)
    {
        for(auto iter : mObservers)
        {
            (*std::get<0>(iter))(value);
        }
    }

    inline void clearAllObservers()
    {
        mObservers.clear();
    }

    inline int getObserverCnt() { return static_cast<int>(mObservers.size()); }

private:
    ObserverList mObservers;
};

} // namespace Util

template <class SUBJECT, class METHODPTR, class OBJPTR>
void registerObserver(SUBJECT& sub, METHODPTR const& mptr, OBJPTR* obj)
{
    using std::placeholders::_1;
    typename SUBJECT::Observer* obsPtr{new typename SUBJECT::Observer{std::bind(mptr, obj, _1)}};
    // clang-format off
    union MethodID {
        METHODPTR a;
        void* voidPtr;
    } convert;
    convert.a = mptr;
    // clang-format on
    sub.registerObserver(*obsPtr, convert.voidPtr, obj);
}

template <class SUBJECT, class METHODPTR, class OBJPTR>
void unregisterObserver(SUBJECT& sub, METHODPTR const& mptr, OBJPTR* obj)
{
    // clang-format off
    union MethodID {
        METHODPTR a;
        void* voidPtr;
    } convert;
    convert.a = mptr;
    // clang-format on
    sub.unregisterObserver(convert.voidPtr, obj);
}

} // namespace WS

#endif
