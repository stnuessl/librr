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

namespace rr {

template <typename V, typename E>
class result {
public:
    result(const V &val);
    result(V &&val);
    result(const E &err);
    result(E &&err);
    
    result<V, E> &operator=(const V &val);
    result<V, E> &operator=(V &&val);
    result<V, E> &operator=(const E &err);
    result<V, E> &operator=(E &&err);
    
    bool ok() const;
    bool bad() const;
    
    V &value() &;
    V &&value() &&;
    const V &value() const &;
    const V &&value() const &&;
    V &&take();
    template <typename T> V take_or(T &&alt);
    
    const E &error() const &;
    const E &&error() const &&;
private:
    enum class state : unsigned char { 
#ifndef NDEBUG
        CONSUMED,
#endif
        ERROR,
        VALUE, 
    };

    V _val;
    E _err;
    enum state _state;
};

template <typename V, typename E>
result<V, E>::result(const V &val)
    : _val(val),
      _state(state::VALUE)
{
}

template <typename V, typename E>
result<V, E>::result(V &&val)
    : _val(std::move(val)),
      _state(state::VALUE)
{
}

template <typename V, typename E>
result<V, E>::result(const E &err)
    : _err(err),
      _state(state::ERROR)
{
}

template <typename V, typename E>
result<V, E>::result(E &&err)
    : _err(std::move(err)),
      _state(state::ERROR)
{
}

template <typename V, typename E>
result<V, E> &result<V, E>::operator=(const V &val)
{
    _val = val;
    _state = state::VALUE;
    
    return *this;
}

template <typename V, typename E>
result<V, E> &result<V, E>::operator=(V &&val)
{
    _val = std::move(val);
    _state = state::VALUE;
    
    return *this;
}

template <typename V, typename E>
result<V, E> &result<V, E>::operator=(const E &err)
{
    _err = err;
    _state = state::ERROR;
    
    return *this;
}

template <typename V, typename E>
result<V, E> &result<V, E>::operator=(E &&err)
{
    _err = std::move(err);
    _state = state::ERROR;
    
    return *this;
}

template <typename V, typename E>
bool result<V, E>::ok() const
{
    return _state == state::VALUE;
}

template <typename V, typename E>
bool result<V, E>::bad() const
{
    return _state == state::ERROR;
}

template <typename V, typename E>
V &result<V, E>::value() &
{
    assert(_state == state::VALUE && "result has no value");
    
    return _val;
}

template <typename V, typename E>
V &&result<V, E>::value() &&
{
    assert(_state == state::VALUE && "result has no value");
    
    return std::move(_val);
}

template <typename V, typename E>
const V &result<V, E>::value() const &
{
    assert(_state == state::VALUE && "result has no value");
    
    return _val;
}

template <typename V, typename E>
const V &&result<V, E>::value() const &&
{
    assert(_state == state::VALUE && "result has no value");
    
    return std::move(_val);
}

template <typename V, typename E>
V &&result<V, E>::take()
{
    assert(_state == state::VALUE && "result has no value");
    
#ifndef NDEBUG
    _state = state::CONSUMED;
#endif
    
    return std::move(_val);
}

template <typename V, typename E> template <typename T>
V result<V, E>::take_or(T &&alt)
{
    assert(_state != state::CONSUMED && "result already consumed");
    
    if (_state == state::ERROR)
        return static_cast<V>(std::forward<T>(alt));
    
#ifndef NDEBUG
    _state = state::CONSUMED;
#endif
    
    return std::move(_val);
}

template <typename V, typename E>
const E &result<V, E>::error() const &
{
    assert(_state == state::ERROR && "result has no error");
    
    return _err;
}

template <typename V, typename E>
const E &&result<V, E>::error() const &&
{
    assert(_state == state::ERROR && "result has no error");
    
    return std::move(_err);
}


} /* namespace rr */

#endif /* _RESULT_HPP_ */
