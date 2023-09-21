#pragma once


#include <cassert>
#include <cstring>
#include <cstdarg>

#include "engine/Memory.hpp"

template<class TAlloc = StringMemory>
class String {
private:
    char *mStr{nullptr};
    int mCapacity{0};
    int mLength{0};

    inline void copy(const String &other) {
        if (other.mLength > 0) {
            Reserve(other.mLength);
            memcpy(mStr, other.mStr, other.mLength);
            mLength = other.mLength;
            mStr[mLength] = '\0';
        }
    }

public:
    explicit inline String() {
        mLength = 0;
        mCapacity = 0;
        mStr = nullptr;
    }

    [[maybe_unused]] explicit inline String(char value) {
        Reserve(1);
        mStr[0] = value;
        mLength = 1;
        mStr[mLength] = '\0';
    }

    [[maybe_unused]] explicit inline String(unsigned char value) {
        int len = snprintf(nullptr, 0, "%hhu", value);
        Reserve(len);
        sprintf_s(mStr, len + 1, "%hhu", value);
        mLength = len;
        mStr[mLength] = '\0';
    }

    [[maybe_unused]] explicit inline String(short value) {
        int len = snprintf(nullptr, 0, "%hd", value);
        Reserve(len);
        sprintf_s(mStr, len + 1, "%hd", value);
        mLength = len;
        mStr[mLength] = '\0';
    }

    [[maybe_unused]] explicit inline String(unsigned short value) {
        int len = snprintf(nullptr, 0, "%hu", value);
        Reserve(len);
        sprintf_s(mStr, len + 1, "%hu", value);
        mLength = len;
        mStr[mLength] = '\0';
    }

    [[maybe_unused]] explicit inline String(int value) {
        int len = snprintf(nullptr, 0, "%d", value);
        Reserve(len);
        sprintf_s(mStr, len + 1, "%d", value);
        mLength = len;
        mStr[mLength] = '\0';
    }

    [[maybe_unused]] explicit inline String(unsigned int value) {
        int len = snprintf(nullptr, 0, "%u", value);
        Reserve(len);
        sprintf_s(mStr, len + 1, "%u", value);
        mLength = len;
        mStr[mLength] = '\0';
    }

    [[maybe_unused]] explicit inline String(long value) {
        int len = snprintf(nullptr, 0, "%ld", value);
        Reserve(len);
        sprintf_s(mStr, len + 1, "%ld", value);
        mLength = len;
        mStr[mLength] = '\0';
    }

    [[maybe_unused]] explicit inline String(unsigned long value) {
        int len = snprintf(nullptr, 0, "%lu", value);
        Reserve(len);
        sprintf_s(mStr, len + 1, "%lu", value);
        mLength = len;
        mStr[mLength] = '\0';
    }

    [[maybe_unused]] explicit inline String(long long value) {
        int len = snprintf(nullptr, 0, "%zd", value);
        Reserve(len);
        sprintf_s(mStr, len + 1, "%zd", value);
        mLength = len;
        mStr[mLength] = '\0';
    }

    [[maybe_unused]] explicit inline String(unsigned long long value) {
        int len = snprintf(nullptr, 0, "%zu", value);
        Reserve(len);
        sprintf_s(mStr, len + 1, "%zu", value);
        mLength = len;
        mStr[mLength] = '\0';
    }

    [[maybe_unused]] explicit inline String(float value) {
        int len = snprintf(nullptr, 0, "%f", value);
        Reserve(len);
        sprintf_s(mStr, len + 1, "%f", value);
        mLength = len;
        mStr[mLength] = '\0';
    }

    [[maybe_unused]] explicit inline String(double value) {
        int len = snprintf(nullptr, 0, "%f", value);
        Reserve(len);
        sprintf_s(mStr, len + 1, "%f", value);
        mLength = len;
        mStr[mLength] = '\0';
    }

    [[maybe_unused]] explicit inline String(bool value) {
        if (value) {
            Reserve(4);
            memcpy(mStr, "true", 4);
            mLength = 4;
            mStr[mLength] = '\0';
        } else {
            Reserve(5);
            memcpy(mStr, "false", 5);
            mLength = 5;
            mStr[mLength] = '\0';
        }
    }

    [[maybe_unused]] explicit inline String(const char *str) {
        int len = (int) strlen(str);
        Reserve(len);
        memcpy(mStr, str, len);
        mLength = len;
        mStr[mLength] = '\0';
    }

    [[maybe_unused]] inline String(const char *fmt, ...) {
        if (fmt) {
            va_list args;
            va_start(args, fmt);
            int len = vsnprintf(nullptr, 0, fmt, args);
            Reserve(len);
            if (len > 0) {
                mLength = len;
                vsnprintf(mStr, len + 1, fmt, args);
                mStr[len] = '\0';
            }
            va_end(args);
        } else {
            Reserve(0);
        }
    }

    [[maybe_unused]] inline String(const String &str) {
        copy(str);
    }

    [[maybe_unused]] inline String(String &&other) noexcept {
        std::swap(mLength, other.mLength);
        std::swap(mCapacity, other.mCapacity);
        std::swap(mStr, other.mStr);
    }

    inline ~String() {
        mLength = 0;
        mCapacity = 0;
        if (mStr != nullptr) {
            Free<TAlloc>((void **) (&mStr));
            mStr = nullptr;
        }
    }

    [[maybe_unused]] inline void Fit() {
        if (mLength == 0) {
            mLength = 0;
            mCapacity = 0;

            if (mStr != nullptr)
                Free<TAlloc>((void **) (&mStr));
            return;
        }

        int newCapacity = mLength + 1;
        char *newList = Alloc<TAlloc, char>(newCapacity);
        assert(newList != nullptr && "String: Insufficient memory.\n");

        memcpy(newList, mStr, mLength);
        mStr[mLength] = '\0';

        if (mStr != nullptr) {
            Free<TAlloc>((void **) (&mStr));
        }

        mStr = newList;
        mCapacity = newCapacity;
    }

    inline char &operator[](const int index) const {
        assert(index >= 0 && index < mLength && "String: index out of range");
        return mStr[index];
    }

    inline void Reserve(int newCapacity) {
        if (newCapacity == 0 && mCapacity == 0) {
            mLength = 0;
            mCapacity = 0;
            if (mStr != nullptr)
                Free<TAlloc>((void **) (&mStr));
            return;
        }

        newCapacity++;
        if (newCapacity <= mCapacity)
            return;

        char *newList = Alloc<TAlloc, char>(newCapacity);
        assert(newList != nullptr && "String: Insufficient memory.\n");
        if (mStr) {
            memcpy(newList, mStr, mLength);
            newList[mLength] = '\0';

            Free<TAlloc>((void **) (&mStr));
        } else {
            mLength = 0;
            newList[mLength] = '\0';
        }
        mStr = newList;
        mCapacity = newCapacity;
    }


    String &operator=(const String &other) {
        if (this != &other)
            copy(other);
        return *this;
    }

    String &operator=(String &&other) noexcept {
        std::swap(mLength, other.mLength);
        std::swap(mCapacity, other.mCapacity);
        std::swap(mStr, other.mStr);
        return *this;
    }

    String &operator=(const char *other) {
        if (other) {
            int n = (int) strlen(other);
            Reserve(n);
            memcpy(mStr, other, n);
            mLength = n;
            mStr[mLength] = '\0';
        } else {
            Reserve(0);
            mLength = 0;
            mStr[0] = '\0';
        }
        return *this;
    }

    [[maybe_unused]] [[nodiscard]]
    inline friend String operator+(const String &lhs, const char *rhs) {
        int len = (int) strlen(rhs);
        String out;
        out.Reserve(lhs.mLength + len);
        out.mLength = out.mCapacity - 1;

        memcpy(&out.mStr[0], lhs.mStr, lhs.mLength);
        memcpy(&out.mStr[lhs.mLength], rhs, len);
        out.mStr[out.mLength] = '\0';
        return out;
    }

    [[maybe_unused]] [[nodiscard]]
    inline friend String operator+(const char *lhs, const String &rhs) {
        int len = (int) strlen(lhs);
        String out;
        out.Reserve(len + rhs.mLength);
        out.mLength = out.mCapacity - 1;

        memcpy(&out.mStr[0], lhs, len);
        memcpy(&out.mStr[len], rhs.mStr, rhs.mLength);
        out.mStr[out.mLength] = '\0';
        return out;
    }

    [[maybe_unused]] [[nodiscard]]
    inline friend String operator+(const String &lhs, const String &rhs) {
        String out;
        out.Reserve(lhs.mLength + rhs.mLength);
        out.mLength = out.mCapacity - 1;

        memcpy(&out.mStr[0], lhs.mStr, lhs.mLength);
        memcpy(&out.mStr[lhs.mLength], rhs.mStr, rhs.mLength);
        out.mStr[out.mLength] = '\0';
        return out;
    }

    inline String operator+=(const char *rhs) {
        int len = (int) strlen(rhs);
        Reserve(mLength + len);
        memcpy(&mStr[mLength], rhs, len);
        mLength += len;
        mStr[mLength] = '\0';
        return *this;
    }

    inline String operator+=(const String &rhs) {
        Reserve(mLength + rhs.mLength);
        memcpy(&mStr[mLength], rhs.mStr, rhs.mLength);
        mLength += rhs.mLength;
        mStr[mLength] = '\0';
        return *this;
    }

    inline bool operator==(const char *other) const {
        return Compare(other) == 0;
    }

    inline bool operator==(const String &other) const {
        return operator==(other.mStr);
    }

    inline bool operator!=(const char *other) const {
        return Compare(other) != 0;
    }

    inline bool operator!=(const String &other) const {
        return operator!=(other.mStr);
    }

    inline bool operator>(const char *other) const {
        return Compare(other) > 0;
    }

    inline bool operator>(const String &other) const {
        return operator>(other.mStr);
    }

    inline bool operator<(const char *other) const {
        return Compare(other) < 0;
    }

    inline bool operator<(const String &other) const {
        return operator<(other.mStr);
    }

    inline bool operator>=(const char *other) const {
        return Compare(other) >= 0;
    }

    inline bool operator>=(const String &other) const {
        return operator>=(other.mStr);
    }

    inline bool operator<=(const char *other) const {
        return Compare(other) <= 0;
    }

    inline bool operator<=(const String &other) const {
        return operator<=(other.mStr);
    }

    inline bool operator!() const {
        return !mStr || mLength == 0;
    }

    [[maybe_unused]] [[nodiscard]] inline const int &Length() const {
        return mLength;
    }

    [[maybe_unused]] [[nodiscard]] inline const int &Capacity() const {
        return mCapacity;
    }

    [[maybe_unused]] [[nodiscard]] inline int Compare(const char *other) const {
        if (mStr == nullptr)
            return other == nullptr ? 0 : -1;
        if (other == nullptr)
            return mStr == nullptr ? 0 : 1;
        return strcmp(mStr, other);
    }

    [[maybe_unused]] [[nodiscard]] inline int Compare(const String &other) const {
        if (mStr == nullptr)
            return other == nullptr ? 0 : -1;
        if (other.mStr == nullptr)
            return mStr == nullptr ? 0 : 1;
        return strcmp(mStr, other.mStr);
    }

    [[maybe_unused]] [[nodiscard]]
    inline const char *Str() const {
        if (!mStr)
            return nullptr;
        return const_cast<const char *>(mStr);
    }
};