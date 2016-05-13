# rr - result return

A very simple C++ header only library which helps to avoid exceptions by
providing return wrapper types capable of holding either the return value or an
error value.

## Installation

Simply run:

```
    # make install
```

As this is a header only library you don't have to compile anything for this
to work. Also if you want to use the library you just need to include the header
files in your sources - no library linking required.


Use __# make uninstall__ if you want to remove the files from your system.

## General Notes

The library disables safety checks if the preprocessor directive "NDEBUG" is
specified. This can introduce undefined behaviour if you call _.take()_ or
_.take_error()_ serveral times on the same object. Also you are __always__
supposed to check with _.ok()_ if an value is present before _.take()_ing it.

## Usage examples

This sections shows just some very random examples on how this library can be
used.

### rr::result

```cpp
    rr::result<int> div(int num, int denom)
    {
        if (denom == 0)
            return rr::result<int>(rr::error(EINVAL, "denominator is zero"));
         
        return rr::result<int>(num / denom);
    }
    
    {
        rr::result<int> result = div(val1, val2);
        if (result.ok()) {
            std::cout << "Result is: " << result.take() << "\n";
        } else {
            rr::error err = result.take_error();
            
            std::cerr << "Error: " << err.message() << " , error code: "
                      << err.code() << "\n";
        }
    }
    
    {
        auto val = div(100, 0).take_or(42);
        assert(val = 42);
    }
```

How would the default alternative look with _-fno-exceptions_ enabled?
```cpp
    int div(int num, int denom, int &err, std::string &err_msg)
    {
        err_msg.clear();
        
        if (denom == 0) {
            err = EINVAL;
            err_msg += "denominator is zero";
            return 0.0;
        }
        
        err = 0;
        return num / denom;
    }
    
    {
        int err;
        std::string msg;
        
        int val = div(val1, val2, err, msg);
        if (err == 0) {
            std::cout << "Result is: " << val << "\n";
        } else {
            std::cerr << "Error: " << msg << " , error code: " 
                      << err.code() << "\n";
        }
    }
    
    {
        int err;
        std::string msg;
        
        int val = div(100, 0, err, msg);
        if (err != 0)
            val = 42;
        
        assert(val == 42);
    }
```

### rr::option for values

_rr::option_ is like _rr::result_ but without the additional error information.

```cpp
    {
        rr::option<T> option = f();
        if (option.ok()) {
            T value = option.take();
            /* use value here */
        }
    }
```

### rr::option for errors

Sometimes you just want to report if an error occured. A _rr::option_ can be
used for this case too.

```cpp
    {
        rr::option<rr::error> option = f();
        if (option.ok()) {
            rr::error error = option.take();
            
            int code = error.code();
            const string &msg = error.message();
            /* handle error here */
        }
    }
```
