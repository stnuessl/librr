/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Steffen Nuessle
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _RESULT_HPP_
#define _RESULT_HPP_

#include <utility>
#include <cassert>

#include "error.hpp"

namespace rr {

template <typename T>
class result {
public:
    result(const T &val);
    result(T &&val);
    result(error &&err);
    
    result<T> &operator=(const T &val);
    result<T> &operator=(T &&val);
    result<T> &operator=(const error &err);
    result<T> &operator=(error &&err);
    
    bool ok() const;
    
    T &value() &;
    T &&value() &&;
    const T &value() const &;
    const T &&value() const &&;
    T &&take();
    template <typename U> T take_or(U &&alt);
    
    const error &err() const;
    error take_error();

private:
    enum class state : unsigned char { 
#ifndef NDEBUG
        CONSUMED,
#endif
        ERROR,
        VALUE, 
    };

    T _val;
    error _err;
    enum state _state;
};

template <typename T>
result<T>::result(const T &val)
    : _val(val),
      _state(result<T>::state::VALUE)
{
}

template <typename T>
result<T>::result(T &&val)
    : _val(std::move(val)),
      _state(result<T>::state::VALUE)
{
}

template <typename T>
result<T>::result(error &&err)
    : _err(std::move(err)),
      _state(result<T>::state::ERROR)
{
}

template <typename T>
result<T> &result<T>::operator=(const T &val)
{
    _val = val;
    _state = result<T>::state::VALUE;
    
    return *this;
}

template <typename T>
result<T> &result<T>::operator=(T &&val)
{
    _val = std::move(val);
    _state = result<T>::state::VALUE;
    
    return *this;
}

template <typename T>
result<T> &result<T>::operator=(const error &err)
{
    _err = err;
    _state = result<T>::state::ERROR;
    
    return *this;
}

template <typename T>
result<T> &result<T>::operator=(error &&err)
{
    _err = std::move(err);
    _state = result<T>::state::ERROR;
    
    return *this;
}

template <typename T>
bool result<T>::ok() const
{
    return _state == result<T>::state::VALUE;
}

template <typename T>
T &result<T>::value() &
{
    assert(_state == result<T>::state::VALUE && "result has no value");
    
    return _val;
}

template <typename T>
T &&result<T>::value() &&
{
    assert(_state == result<T>::state::VALUE && "result has no value");
    
    return std::move(_val);
}

template <typename T>
const T &result<T>::value() const &
{
    assert(_state == result<T>::state::VALUE && "result has no value");
    
    return _val;
}

template <typename T>
const T &&result<T>::value() const &&
{
    assert(_state == result<T>::state::VALUE && "result has no value");
    
    return std::move(_val);
}

template <typename T>
T &&result<T>::take()
{
    assert(_state == result<T>::state::VALUE && "result has no value");
    
#ifndef NDEBUG
    _state = result<T>::state::CONSUMED;
#endif
    
    return std::move(_val);
}

template <typename T> template <typename U>
T result<T>::take_or(U &&alt)
{
    assert(_state != result<T>::state::CONSUMED && "result already consumed");
    
    if (_state == result<T>::state::ERROR)
        return static_cast<T>(std::forward<U>(alt));
    
#ifndef NDEBUG
    _state = result<T>::state::CONSUMED;
#endif
    
    return std::move(_val);
}

template <typename T>
const error &result<T>::err() const
{
    assert(_state == result<T>::state::ERROR && "result has no error");
    
    return _err;
}

template <typename T>
error result<T>::take_error()
{
    assert(_state == result<T>::state::ERROR && "result has no error");

#ifndef NDEBUG    
    _state = result<T>::state::CONSUMED;
#endif
    
    return std::move(_err);
}




} /* namespace rr */

#endif /* _RESULT_HPP_ */
