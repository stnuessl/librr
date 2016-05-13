

#include <vector>
#include <string>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <memory>

#include <cassert>
#include <cerrno>

#include <librr/option.hpp>
#include <librr/result.hpp>


template <typename T>
rr::result<T> divide(const T &num, const T &denom)
{
    if (denom == static_cast<T>(0.0))
        return rr::result<T>(rr::error(EINVAL, "denominator is zero!"));
    
    return rr::result<T>(num / denom);
}

rr::option<std::vector<std::string>> option_vector_valid()
{
    std::vector<std::string> ret = { "Hello", ",", "World"};
    
    return rr::option<std::vector<std::string>>(std::move(ret));
}

rr::option<std::vector<std::string>> option_vector_invalid()
{
    return rr::option<std::vector<std::string>>();
}

rr::result<int> result_num_valid()
{
    return rr::result<int>(42);
}

rr::result<int> result_num_invalid()
{
    return rr::result<int>(rr::error(100, "error message"));
}

void test_error_simple()
{
    assert(rr::error(EAGAIN).message() == "(null)");
    assert(rr::error(EAGAIN, "this is bad").message() == "this is bad");
    
    /* test constructors & assignments */
    auto err1 = rr::error(EPERM);
    auto err2 = rr::error(EIO);
    
    err1 = err2;
    
    assert(err1.code() == EIO);
}

void test_option_simple()
{
    auto option = option_vector_valid();
    assert(option.ok());
    
    auto vec = option.take();
    
    assert(vec.size() == 3);
    assert(vec[0] == "Hello");
    assert(vec[1] == ",");
    assert(vec[2] == "World");
    
    assert(!option.ok());
    assert(option.take_or(std::vector<std::string>()).empty());
    
    option = option_vector_invalid();
    
    assert(!option.ok());
}

void test_result_simple()
{
    int val;
    auto result = result_num_valid();
    assert(result.ok());
    assert(result.take_or(7) == 42);
    assert(!result.ok());
    
    result = result_num_invalid();
    assert(!result.ok());
    assert(result.take_or(7) == 7);
    
    auto error = result.take_error();
    assert(error.code() == 100 && error.message() == "error message");
}

rr::result<int> random_number()
{
    rr::error err;
    FILE *f = std::fopen("/dev/urandom", "r");
    if (!f) {
        err = rr::error(errno, "failed to open \"/dev/urandom\"");
        goto fail;
    }
    
    int val;
    
    if (std::fread((void *) &val, sizeof(val), 1, f) < 0) {
        err = rr::error(errno, "failed to read from \"/dev/urandom\"");
        goto fail;
    }
    
    if (val & 0x01) {
        err = rr::error(EINVAL, "got odd number from \"/dev/urandom\"");
        goto fail;
    }
    
    fclose(f);
    return rr::result<int>(val);

fail:
    fclose(f);
    return rr::result<int>(std::move(err));
}

void example_usage01()
{
    for (volatile int i = 0; i < 10; i++) {
        auto result = random_number();
        if (!result.ok()) {
            auto err = result.take_error();
            std::cerr << "** ERROR: " << err.message() << " - " 
                      << std::strerror(err.code()) << "\n";
            continue;
        }
        
        assert(!(result.take() & 0x01));
    }
}

rr::result<std::shared_ptr<int>> make_shared_ptr_result()
{
    return rr::result<std::shared_ptr<int>>(std::make_shared<int>(42));
}

void example_usage02()
{
    auto result = make_shared_ptr_result();
    assert(result.ok());
    assert(*result.take() == 42);
    assert(!result.ok());
}

void example_usage03()
{
    assert(divide(100, 10).take() == 10);
    assert(divide(100, 10).take_or(40) == 10);
    assert(divide(100, 0).take_or(40) == 40);
    assert(divide(100.0, 0.0).take_or(100.0) == 100.0);
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
    
    test_error_simple();
    test_option_simple();
    test_result_simple();
    
    example_usage01();
    example_usage02();
    example_usage03();

    std::cout << "ok\n";
    
    return 0;
}
