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

#ifndef _ERROR_HPP_
#define _ERROR_HPP_

#include <string>
#include <utility>

namespace rr {

class error {
public:
    explicit error(int code = 0, const char *msg = nullptr);
    error(int code, const std::string &msg);
    error(int code, std::string &&msg);
    
    int code() const;
    const std::string &message() const;
private:
    int _code;
    std::string _msg;
};

error::error(int code, const char *msg)
    : _code(code),
      _msg((msg) ? msg : "(null)")
{
}

error::error(int code, const std::string &msg)
    : _code(code),
      _msg(msg)
{
}

error::error(int code, std::string &&msg)
    : _code(code),
      _msg(std::move(msg))
{
}

int error::code() const
{
    return _code;
}

const std::string &error::message() const
{
    return _msg;
}

} /* namespace rr */

#endif /* _ERROR_HPP_ */
