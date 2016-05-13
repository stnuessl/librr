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

#ifndef _OPTION_HPP_
#define _OPTION_HPP_

#include <utility>
#include <cstdlib>

namespace rr {

template <typename T>
class option {
public:
    option();
    explicit option(T &&val);
    
    bool ok() const;
    
    T take();
    
    template <typename U>
    T take_or(U &&val);
private:
    T _val;
    bool _ok;
};

template <typename T>
option<T>::option()
    : _val(),
      _ok(false)
{
}

template <typename T>
option<T>::option(T &&val)
    : _val(std::move(val)),
      _ok(true)
{
}

template <typename T>
bool option<T>::ok() const
{
    return _ok;
}

template <typename T>
T option<T>::take()
{
    assert(_ok && "option has no value");
    
#ifndef NDEBUG
    _ok = false;
#endif
    
    return std::move(_val);
}

template <typename T> template <typename U>
T option<T>::take_or(U &&val)
{
    if (!_ok)
        return static_cast<T>(std::forward<U>(val));
    
    _ok = false;
    
    return std::move(_val);
}

} /* namespace rr */

#endif /* _OPTION_HPP_ */
