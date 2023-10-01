#pragma once

#include <iostream>
#include <sstream>

#include "data/TString.hpp"

template<class T>
auto operator<<(std::ostream &os, const T &t) -> decltype(t.Stream(os), os) {
    t.Stream(os);
    return os;
}

template<typename... Args>
static inline void LTrace(Args const &... args) {
    int unpack[]{0, (std::cout << args << "\t", 0)...};
    std::cout << std::endl;
}


template<typename... Args>
static inline void LInfo(const char *_format, Args const &... args) {
    fprintf(stdout, _format, args...);
    std::cout << std::endl;
}

template<typename... Args>
static inline void LError(const char *_format, Args const &... args) {
    fprintf(stderr, _format, args...);
    std::cout << std::endl;
}

template<typename... Args>
static inline void LFatal(const char *_format, Args const &... args) {
    fprintf(stderr, _format, args...);
    exit(-1);
}