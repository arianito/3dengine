#pragma once

#include <type_traits>
#include <cstddef>
#include <cstring>
#include <cassert>
#include <memory>
#include <type_traits>
#include "engine/Memory.hpp"
#include "engine/Object.hpp"

template <typename T>
class Array : public Object<Array<T>>
{
private:
	T *mList;
	Allocator *mAllocator{nullptr};
	int mCapacity{2};
	int mLength{0};

	inline void expand()
	{
		if (mLength < mCapacity)
			return;

		size_t size = mCapacity * sizeof(T);
		T *newList = (T *)mAllocator->Alloc(size * 2);
		assert(newList != nullptr && "Array: Insufficient memory.\n");
		memset(newList, 0, size * 2);
		memcpy(newList, mList, size);
		mAllocator->Free((void **)(&mList));
		mList = newList;
		mCapacity = mCapacity * 2;
	}
	inline void shrink()
	{
		if (mLength * 4 > mCapacity)
			return;

		size_t size = mCapacity * sizeof(T);
		T *newList = (T *)mAllocator->Alloc(size / 2);
		assert(newList != nullptr && "Array: Insufficient memory.\n");
		memcpy(newList, mList, size / 2);
		mAllocator->Free((void **)(&mList));
		mList = newList;
		mCapacity = mCapacity / 2;
	}

	inline void destroy(int index)
	{
		if constexpr (std::is_pointer_v<T>)
		{
			delete mList[index];
			mList[index] = nullptr;
		}
		else if constexpr (is_shared_ptr<std::shared_ptr<T>>::value || is_unique_ptr<std::unique_ptr<T>>::value)
		{
			mList[index] = nullptr;
		}
	}

public:
	Array(Allocator *a, int capacity) : mCapacity(capacity), mAllocator(a), mLength(0)
	{
		size_t size = mCapacity * sizeof(T);
		mList = (T *)mAllocator->Alloc(size);
		memset(mList, 0, size);
	}
	explicit Array(Allocator *a) : Array(a, 2) {}
    explicit Array(const Array &) = delete;

	inline ~Array()
	{
        for (int i = 0; i < mLength; i++)
            destroy(i);
		mAllocator->Free((void **)(&mList));
	}

	inline void removeAt(int index)
	{
		assert(index >= 0 && index < mLength && "Array: Index out of range.\n");
		shrink();
		for (int i = index; i < mLength - 1; i++)
            mList[i] = mList[i + 1];
        destroy(mLength - 1);
		mLength--;
	}

	inline void remove(const T& item)
	{
		int j = 0;
		for (int i = 0; i < mLength; i++)
		{
			if (mList[i + j] == item) {
                destroy(i + j);
                j++;
            }
			if (j > 0)
				mList[i] = mList[i + j];
		}
		mLength -= j;
	}

	inline void push(const T &element)
	{
		insert(element, mLength);
	}

	inline void prepend(const T &element)
	{
		insert(element, 0);
	}

	inline void insert(const T &element, int index)
	{
		assert(index >= 0 && index <= mLength && "Array: Index out of range.\n");
		expand();

		for (int i = mLength; i > index; i--)
			mList[i] = mList[i - 1];

		mList[index] = element;
		mLength++;
	}

	inline T &operator[](int index)
	{
		assert(index >= 0 && index < mLength && "Array: Index out of range.\n");
		return mList[index];
	}

	inline int find(const T &obj)
	{
		for (int i = 0; i < mLength; i++)
		{
			if (mList[i] == obj)
				return i;
		}
		return -1;
	}

	inline bool isEmpty()
	{
		return mLength == 0;
	}

	inline const int &size()
	{
		return mLength;
	}

	inline const int &capacity()
	{
		return mCapacity;
	}
};