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

#include <vector>
#include <string>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <memory>

#include <cassert>
#include <cerrno>

// #include <librr/option.hpp>
// #include <librr/result.hpp>

#include "../lib/option.hpp"
#include "../lib/result.hpp"

void test_option()
{
    rr::option<std::vector<std::string>> option01;
    option01 = std::vector<std::string>({ "A", "B", "C" });
    
    assert(option01.ok());
    assert(option01.value()[0] == "A");
    assert(option01.value()[1] == "B");
    assert(option01.value()[2] == "C");
    assert(option01.value().size() == 3);
    
    auto v = option01.take_or(std::vector<std::string>());
    assert(!option01.ok());
    assert(v[0] == "A");
    assert(v[1] == "B");
    assert(v[2] == "C");
    assert(v.size() == 3);
    
    v = option01.take_or(std::vector<std::string>());
    assert(!option01.ok());
    assert(v.empty());
    
    option01 = std::vector<std::string>({ "U", "X", "Y", "Z" });
    assert(option01.ok());
    assert(option01.value()[0] == "U");
    assert(option01.value()[1] == "X");
    assert(option01.value()[2] == "Y");
    assert(option01.value()[3] == "Z");
    assert(option01.value().size() == 4);
    assert(option01.ok());
    
    auto option02 = rr::option<double>();
    assert(!option02.ok());
    assert(option02.take_or(100.0) == 100.0);
    assert(!option02.ok());
    
    assert(rr::option<unsigned int>().take_or(42) == 42);
}

void test_result()
{
    auto result01 = rr::result<bool, int>(0);
    assert(!result01.ok());
    assert(result01.bad());
    assert(result01.ok() != result01.bad());
    assert(result01.error() == 0);
    assert(result01.take_or(true));
    (void) result01;
    
    rr::result<std::string, bool> result02 = false;
    assert(result02.bad());
    assert(!result02.ok());
    assert(result02.ok() != result02.bad());
    assert(result02.error() == false);
    assert(result02.take_or("Hello, World!") == "Hello, World!");
    (void) result02;

    auto result03 = rr::result<unsigned int, int>(0U);
    assert(result03.ok());
    assert(result03.value() == 0);
    assert(result03.take_or(42) == 0);
    result03 = 0U;
    result03.value() = 42;
    assert(result03.value() == 42);
    (void) result03;
    
    auto result04 = rr::result<int, std::string>("Hello, World!");
    assert(result04.bad());
    assert(result04.error() == "Hello, World!");
    (void) result04;
    
    auto result05 = rr::result<std::string, int>("Hello, World!");
    assert(result05.ok());
    assert(result05.value() == "Hello, World!");
    assert(result05.take() == "Hello, World!");
    
    result05 = "Goodbye, World!";
    assert(result05.ok());
    assert(result05.value() == "Goodbye, World!");
    
    result05 = EINVAL;
    assert(!result05.ok());
    assert(result05.bad());
    assert(result05.error() == EINVAL);
    (void) result05;
    
    auto result06 = rr::result<std::vector<std::string>, int>(0);
    result06 = std::vector<std::string>({ "A", "B", "C", "D" });
    assert(result06.ok());
    assert(!result06.bad());
    assert(result06.value()[0] == "A");
    assert(result06.value()[1] == "B");
    assert(result06.value()[2] == "C");
    assert(result06.value()[3] == "D");
    assert(result06.value().size() == 4);
    
    auto v1 = result06.take();
    assert(v1[0] == "A");
    assert(v1[1] == "B");
    assert(v1[2] == "C");
    assert(v1[3] == "D");
    assert(v1.size() == 4);
    
    result06 = EINVAL;
    assert(result06.take_or(v1)[3] == "D");
    assert(result06.take_or(std::move(v1))[3] == "D");
    assert(result06.bad());
    
    result06 = std::vector<std::string>({ "X", "Y", "Z" });
    assert(result06.value()[0] == "X");
    assert(result06.value()[1] == "Y");
    assert(result06.value()[2] == "Z");
    assert(result06.value().size() == 3);
    assert(result06.ok());

    auto result07 = std::move(result06);
    assert(result07.ok());
    (void) result06;
    auto result08 = result07;
    assert(result07.ok());
    assert(result08.ok());
    (void) result07;
    
    rr::result<std::vector<std::string>, double> result09 = result08.take();
    assert(result09.ok());
    assert(result09.value()[0] == "X");
    assert(result09.value()[1] == "Y");
    assert(result09.value()[2] == "Z");
    assert(result09.value().size() == 3);
    (void) result09;
}

rr::option<std::string> get_environ_val(const char *var)
{
    auto val = std::getenv(var);
    if (val)
        return { std::string(val) };
    
    return { };
}

void example_option()
{
    assert(get_environ_val("PWD").ok());
    assert(get_environ_val("PWD").take() == get_environ_val("PWD").take_or(""));
    assert(get_environ_val("PWD").value() == get_environ_val("PWD").take());
    assert(!get_environ_val("ABC").ok());
    assert(get_environ_val("ABC").take_or("XYZ") == "XYZ");
}


rr::result<FILE *, int> open_for_reading(const char *path)
{
    FILE *file = std::fopen(path, "r");
    if (!file)
        return { errno };
    else 
        return { file };
}

template <typename T>
rr::result<T, std::string> divide(const T &num, const T &denom)
{
    if (denom != static_cast<T>(0))
        return { num / denom };
    else 
        return { "division by zero" };
}

void example_result()
{
    auto result01 = open_for_reading("/dev/null");
    assert(result01.ok());
    assert(result01.value() != nullptr);
    assert(fclose(result01.take()) == 0);
    (void) result01;
    
    auto result02 = open_for_reading("/dev/mem");
    assert(!result02.ok());
    assert(result02.bad());
    assert(result02.error() == EACCES);
    (void) result02;
    
    assert(divide(1, 1).ok());
    assert(divide(1, 1).value() == 1);
    assert(divide(1, 1).take() == 1);
    assert(!divide(1, 0).ok());
    assert(divide(1, 0).take_or(1) == 1);
    assert(divide(1, 0).error() == "division by zero");
}

int main(int argc, char *argv[])
{
    std::cout << "Running \"" << argv[0] << "\" - "
#ifdef NDEBUG
              << "\"NDEBUG\" enabled\n";
#else
              << "\"NDEBUG\" disabled\n";
#endif
    
    (void) argc;
    (void) argv;
    
    test_option();
    test_result();
    
    example_option();
    example_result();
    
    std::cout << "ok\n";
    
    return 0;
}
