

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

void test_error()
{
    auto err_msg = "This is a very descriptive error message";
    
    assert(rr::error(EINVAL).code() == EINVAL);
    assert(rr::error(EINVAL).message() == "(null)");
    assert(rr::error(EINVAL, err_msg).message() == err_msg);
    
    auto err = rr::error(EPERM);
    err = rr::error(EINVAL, err_msg);
    
    assert(err.code() == EINVAL);
    assert(err.message() == err_msg);
}

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
    auto err_msg = "This is a very descriptive error message";
    
    assert(!rr::result<int>(rr::error()).ok());
    assert(rr::result<int>(0).ok());
    assert(rr::result<int>(42).value() == 42);
    assert(rr::result<int>(42).take() == 42);
    assert(rr::result<int>(42).take_or(100) == 42);
    assert(rr::result<int>(rr::error()).take_or(42) == 42);
    assert(!rr::result<int>(rr::error(0)).ok());
    assert(rr::result<int>(rr::error(EINVAL)).err().code() == EINVAL);
    assert(rr::result<int>(rr::error(0, err_msg)).err().message() == err_msg);
    
    auto error01 = rr::error(EINVAL, err_msg);
    auto result01 = rr::result<std::vector<std::string>>(std::move(error01));
    assert(!result01.ok());
    assert(result01.err().code() == EINVAL);
    assert(result01.err().message() == err_msg);
    assert(!result01.ok());
    
    result01 = std::vector<std::string>({ "A", "B", "C", "D" });
    assert(result01.ok());
    assert(result01.value()[0] == "A");
    assert(result01.value()[1] == "B");
    assert(result01.value()[2] == "C");
    assert(result01.value()[3] == "D");
    assert(result01.value().size() == 4);
    assert(result01.ok());
    
    auto v = result01.take();
    assert(!result01.ok());
    assert(v[0] == "A");
    assert(v[1] == "B");
    assert(v[2] == "C");
    assert(v[3] == "D");
    assert(v.size() == 4);
    
    result01 = rr::error(0);
    assert(result01.take_or(std::vector<std::string>()).empty());
    result01 = std::vector<std::string>({ "X", "Y", "Z" });
    assert(result01.ok());
    assert(result01.value()[0] == "X");
    assert(result01.value()[1] == "Y");
    assert(result01.value()[2] == "Z");
    assert(result01.value().size() == 3);
    assert(result01.ok());
    
    auto result02 = rr::result<std::vector<std::string>>(rr::error());
    result02 = result01.take();
    assert(!result01.ok());
    assert(result02.ok());
    
    rr::result<std::vector<std::string>> result03 = rr::error();
    assert(!result03.ok());
    rr::result<std::vector<std::string>> result04 = result02.take();
    assert(result04.ok());
    assert(result04.value()[0] == "X");
    assert(result04.value()[1] == "Y");
    assert(result04.value()[2] == "Z");
    assert(result04.value().size() == 3);
}

rr::option<std::string> get_environ_val(const char *var)
{
    auto val = std::getenv(var);
    if (val)
        return std::string(val);
    
    return rr::nullopt;
}

void example_option()
{
    assert(get_environ_val("PWD").ok());
    assert(get_environ_val("PWD").take() == get_environ_val("PWD").take_or(""));
    assert(get_environ_val("PWD").value() == get_environ_val("PWD").take());
    assert(!get_environ_val("ABC").ok());
    assert(get_environ_val("ABC").take_or("XYZ") == "XYZ");
}

rr::result<FILE *> open_for_reading(const char *path)
{
    FILE *file = std::fopen(path, "r");
    if (!file) {
        std::string msg;
        msg.reserve(64);
        msg += "failed to open \"";
        msg += path;
        msg += "\"";
        
        return rr::error(errno, std::move(msg));
    }
    
    return file;
}

template <typename T>
rr::result<T> divide(const T &num, const T &denom)
{
    if (denom != static_cast<T>(0))
        return num / denom;
    
    return rr::error(EINVAL, "invalid denominator");
}

void example_result()
{
    auto result01 = open_for_reading("/dev/null");
    assert(result01.ok());
    assert(result01.value() != nullptr);
    assert(fclose(result01.take()) == 0);
    
    auto result02 = open_for_reading("/dev/mem");
    assert(!result02.ok());
    assert(result02.err().code() == EACCES);
    
    assert(divide(1, 1).ok());
    assert(divide(1, 1).value() == 1);
    assert(divide(1, 1).take() == 1);
    assert(!divide(1, 0).ok());
    assert(divide(1, 0).take_or(1) == 1);
    assert(divide(1, 0).err().code() == EINVAL);
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
    
    test_error();
    test_option();
    test_result();
    
    example_option();
    example_result();
    
    std::cout << "ok\n";
    
    return 0;
}
