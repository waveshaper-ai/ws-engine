#pragma once

#include "BasicTypes.h"
#include <cmath>
#include <vector>

namespace TL
{
namespace LibCore
{
/// Average is an utility class that help compute mean, standard deviation
/// and exponential average on a list of elements. It can be used with
/// element like float, double and other objet as long as they implements the
/// necessary operators.
template <class TYPE>
class Average
{
public:
    Average();
    Average(int histLength, float expAveraging = 0.0);
    Average(const Average& rhs);
    Average(const Average&& rhs);
    ~Average();

    void init(int histLength, float expAveraging = 0.0);

    void clear();

    /// Add a new element in the averager so it will be
    /// count in the computation.
    void add(const TYPE& in);

    /// Simple averaging.
    TYPE computeMean() const;

    /// Exponential average.
    TYPE computeAveExp() const;

    /// Compute the standard deviation.
    TYPE computeStd() const;

    const TYPE& getLastValue() const;

    inline bool isHistoryLooped() const;
    inline bool initialized() const;

    /// Copying and re-creating the iterator on the new vector.
    Average& operator=(const Average& rhs);
    Average& operator=(Average&& rhs);

private:
    // Private membe
    typedef std::vector<TYPE> HistoryList;
    typename std::vector<TYPE>::iterator _currentIt;

    // Degree of exponential averaging
    float _aveExp;
    // Length of moving window
    int _maxLength;
    bool _histLooped;

    // Data history storage
    int _histIdx;

    std::vector<TYPE> _history;
    TYPE _aveSumHist;
    TYPE _aveSumSquaredHist;
    TYPE _aveExpHist;
};

template <class TYPE>
Average<TYPE>::Average()
    : _aveExp(0.0f),
      _maxLength(0),
      _histLooped(false),
      _histIdx(-1),
      _history(),
      _aveSumHist(),
      _aveSumSquaredHist(),
      _aveExpHist()
{
}

//-----------------------------------------------------------------------------
//
template <class TYPE>
Average<TYPE>::Average(int histLength, float expAveraging)
    : _aveExp(0.0f),
      _maxLength(0),
      _histLooped(false),
      _histIdx(-1),
      _aveSumHist(),
      _aveSumSquaredHist(),
      _aveExpHist()
{
    init(histLength, expAveraging);
}

//-----------------------------------------------------------------------------
//
template <class TYPE>
Average<TYPE>::Average(const Average& rhs)
    : _aveExp(rhs._aveExp),
      _maxLength(rhs._maxLength),
      _histLooped(rhs._histLooped),
      _histIdx(rhs._histIdx),
      _history(rhs._history),
      _aveSumHist(rhs._aveSumHist),
      _aveSumSquaredHist(rhs._aveSumSquaredHist),
      _aveExpHist(rhs._aveExpHist)
{
    // update iterator
    _currentIt = _history.begin();
    if(_histIdx != -1)
    {
        std::advance(_currentIt, _histIdx);
    }
}

//-----------------------------------------------------------------------------
//
template <class TYPE>
Average<TYPE>::Average(const Average&& rhs)
    : _aveExp(rhs._aveExp),
      _maxLength(rhs._maxLength),
      _histLooped(rhs._histLooped),
      _histIdx(rhs._histIdx),
      _history(rhs._history),
      _aveSumHist(rhs._aveSumHist),
      _aveSumSquaredHist(rhs._aveSumSquaredHist),
      _aveExpHist(rhs._aveExpHist)
{
    // update iterator
    _currentIt = _history.begin();
    if(_histIdx != -1)
    {
        std::advance(_currentIt, _histIdx);
    }
}

//-----------------------------------------------------------------------------
//
template <class TYPE>
Average<TYPE>::~Average()
{
    _history.clear();
}

//-----------------------------------------------------------------------------
//
template <class TYPE>
Average<TYPE>& Average<TYPE>::operator=(const Average& rhs)
{
    _aveExp = rhs._aveExp;
    _maxLength = rhs._maxLength;
    _histLooped = rhs._histLooped;
    _histIdx = rhs._histIdx;
    _history = rhs._history;
    _aveSumHist = rhs._aveSumHist;
    _aveSumSquaredHist = rhs._aveSumSquaredHist;
    _aveExpHist = rhs._aveExpHist;

    // update iterator
    _currentIt = _history.begin();
    if(_histIdx != -1)
    {
        std::advance(_currentIt, _histIdx);
    }
    return *this;
}

//-----------------------------------------------------------------------------
//
template <class TYPE>
Average<TYPE>& Average<TYPE>::operator=(Average&& rhs)
{
    return Average<TYPE>::operator=(const_cast<const Average&>(rhs));
}

//-----------------------------------------------------------------------------
//
template <class TYPE>
void Average<TYPE>::init(int histLength, float expAveraging)
{
    _maxLength = histLength;
    _aveExp = expAveraging;
    clear();
}

//-----------------------------------------------------------------------------
//
template <class TYPE>
void Average<TYPE>::clear()
{
    _history.clear();
    for(int i = 0; i < _maxLength; i++)
    {
        _history.push_back(TYPE());
    }

    _currentIt = _history.begin();
    _histIdx = -1;
    _histLooped = false;

    // Reset sums
    _aveSumHist = TYPE();
    _aveSumSquaredHist = TYPE();
    _aveExpHist = TYPE();
}

//-----------------------------------------------------------------------------
//
template <class TYPE>
void Average<TYPE>::add(const TYPE& in)
{
    if(_histIdx >= -1)
    {
        // cycle through list.
        _currentIt++;
        if(_currentIt == _history.end())
        {
            _currentIt = _history.begin();
            _histIdx = -1;
            _histLooped = true;
        }
    }
    _histIdx++;

    // replace old value with new value in history sum.
    _aveSumHist = _aveSumHist - (*_currentIt);
    _aveSumHist = _aveSumHist + in;

    // replace old value with new value squared.
    _aveSumSquaredHist = _aveSumSquaredHist - (*_currentIt * *_currentIt);
    _aveSumSquaredHist = _aveSumSquaredHist + (in * in);

    // compute exponential averaging.
    _aveExpHist = in * static_cast<TYPE>(_aveExp) + static_cast<TYPE>(_aveExpHist * (1 - _aveExp));

    // store value
    (*_currentIt = in);
}

//-----------------------------------------------------------------------------
//
template <class TYPE>
const TYPE& Average<TYPE>::getLastValue() const
{
    return *_currentIt;
}

//-----------------------------------------------------------------------------
//
template <class TYPE>
TYPE Average<TYPE>::computeAveExp() const
{
    return _aveExpHist;
}

//-----------------------------------------------------------------------------
//
template <class TYPE>
TYPE Average<TYPE>::computeMean() const
{
    if(_maxLength > 0)
    {
        int histCnt = _histIdx + 1;
        if(histCnt == 0 && _histLooped == false)
            return _aveSumHist * 0;

        if(_histLooped == false)
            return _aveSumHist / static_cast<TYPE>(histCnt);

        return _aveSumHist / static_cast<TYPE>(_maxLength);
    }

    return 0;
}

//-----------------------------------------------------------------------------
//
template <class TYPE>
TYPE Average<TYPE>::computeStd() const
{
    TYPE ret = TYPE();
    if(_maxLength > 0)
    {
        int histCnt = _histIdx + 1;
        if(_histLooped == false && (histCnt == 0 || histCnt == 1))
        {
            return FLOAT_MAX;
        }
        else if(_histLooped == false)
        {
            ret = ((_aveSumSquaredHist * histCnt) - std::pow(_aveSumHist, 2.F)) / (histCnt * (histCnt - 1));
        }
        else
        {
            ret = ((_aveSumSquaredHist * _maxLength) - std::pow(_aveSumHist, 2.F)) / (_maxLength * (_maxLength - 1));
        }

        // Because we use a running standard deviation it is possible to get negative values!
        // quick fix is to use absolute value...
        return std::pow(std::abs(ret), 0.5F);
    }
    return ret;
}

template <class TYPE>
inline bool Average<TYPE>::isHistoryLooped() const
{
    return _histLooped;
}

template <class TYPE>
inline bool Average<TYPE>::initialized() const
{
    return _maxLength > 0;
}

} // namespace LibCore
} // namespace TL
