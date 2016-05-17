# rr - result return

A very simple C++ header only library which helps to avoid exceptions by
providing return wrapper types capable of holding either the return value or an
error value.

## Motivation

This library provides a decent mechanism for error reporting in the absence of
exceptions (e.g. a project uses the _-fno-exceptions_ flag) via return values.
The intention is to avoid additional function arguments which are normally
used in such scenarious and encapsulate them into a single return value.

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
supposed to check with _.ok()_ if an value is present before _.take()_ ing it.

## Usage examples

The file _src/test/main.cpp_ provides a lot of examples which should give
the user a good overview how the two provided classes can be used.
The examples can be run by calling __$ make__. 
