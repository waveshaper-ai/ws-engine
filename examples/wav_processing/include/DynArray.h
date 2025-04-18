#pragma once

#include "BasicTypes.h"
#include <array>
#include <assert.h>

#define CL_ASSERT(arg) assert(arg);
#define _NOEXCEPT

namespace TL
{
namespace LibCore
{
/// A std array container that acts like an std::vector within a fixed size container.
/// Adding ways to pushback and iterate through a variable amount of
/// elements in the static array (this alies performance and memory efficiency - no dynamic alloc).
template <class TYPE, std::size_t MAX_SIZE>
class DynArray : public std::array<TYPE, MAX_SIZE>
{
public:
    using Parent = std::array<TYPE, MAX_SIZE>;
    using size_type = typename Parent::size_type;
    using iterator = typename Parent::iterator;
    using const_iterator = typename Parent::const_iterator;
    using reverse_iterator = typename Parent::reverse_iterator;
    using const_reverse_iterator = typename Parent::const_reverse_iterator;
    using self = DynArray<TYPE, MAX_SIZE>;

    inline DynArray() : Parent(),
                        _size(0)
    {
    }
    inline DynArray(std::initializer_list<TYPE> const& initList)
        : Parent(), _size{0U}
    {
        append(initList);
    }
    inline DynArray(self const& rhs)
        : Parent(), _size{0U}
    {
        append(rhs);
    }
    ~DynArray() = default;

    /// Occlude the different methods in the base class
    inline void push_back(const TYPE& entry)
    {
        CL_ASSERT(_size < MAX_SIZE);

        if(_size < MAX_SIZE)
        {
            Parent::data()[_size++] = entry;
        }
    }

    inline void push_back(const TYPE&& entry)
    {
        CL_ASSERT(_size < MAX_SIZE);

        if(_size < MAX_SIZE)
        {
            Parent::data()[_size++] = entry;
        }
    }

    inline void append(std::initializer_list<TYPE> const& appendList)
    {
        // Loop through all the items in initList and push_back into my array.
        CL_ASSERT(appendList.size() + _size < MAX_SIZE);
        for(auto it : appendList)
        {
            push_back(it);
        }
    }

    inline void append(self const& rhsArray)
    {
        // Loop through all the items in initList and push_back into my array.
        CL_ASSERT(rhsArray.size() + _size < MAX_SIZE);
        for(auto it : rhsArray)
        {
            push_back(it);
        }
    }
    iterator end() _NOEXCEPT
    { // return iterator for end of mutable sequence
#ifdef OS_WINDOWS
        return iterator{Parent::data(), _size};
#endif
#ifdef OS_LINUX
        return iterator{Parent::data() + _size};
#endif
    }

    const_iterator end() const _NOEXCEPT
    { // return iterator for beginning of nonmutable sequence
#ifdef OS_WINDOWS
        return const_iterator{Parent::data(), _size};
#endif
#ifdef OS_LINUX
        return const_iterator{Parent::data() + _size};
#endif
    }

    reverse_iterator rbegin() _NOEXCEPT
    { // return iterator for beginning of reversed mutable sequence
        return (reverse_iterator(end()));
    }

    const_reverse_iterator rbegin() const _NOEXCEPT
    { // return iterator for beginning of reversed nonmutable sequence
        return (const_reverse_iterator(end()));
    }

    reverse_iterator rend() _NOEXCEPT
    { // return iterator for end of reversed mutable sequence
        return (reverse_iterator(Parent::begin()));
    }

    const_reverse_iterator rend() const _NOEXCEPT
    { // return iterator for end of reversed nonmutable sequence
        return (const_reverse_iterator(Parent::begin()));
    }

    const_iterator cend() const _NOEXCEPT
    { // return iterator for end of nonmutable sequence
        return const_cast<self*>(this)->end();
    }

    const_reverse_iterator crbegin() const _NOEXCEPT
    { // return iterator for beginning of reversed nonmutable sequence
        return const_cast<self*>(this)->rbegin();
    }

    const_reverse_iterator crend() const _NOEXCEPT
    { // return iterator for end of reversed nonmutable sequence
        return const_cast<self*>(this)->rend();
    }

    size_type size() const _NOEXCEPT
    { // return length of sequence
        return _size;
    }

    inline void clear()
    {
        _size = 0;
    }

private:
    std::size_t _size;
};

} // namespace LibCore
} // namespace TL