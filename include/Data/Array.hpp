#pragma once

#include <type_traits>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <memory>
#include <type_traits>
#include "engine/Memory.hpp"
#include "engine/Object.hpp"

template <typename T>
class Array : public Object<Array<T>>
{
private:
	T *mList;
	Allocator *mAllocator;
	unsigned int mCapacity;
	unsigned int mLength;

	inline void expand()
	{
		if (mLength < mCapacity)
			return;

		size_t size = mCapacity * sizeof(T);
		T *newList = (T *)mAllocator->Alloc(size * 2);
		assert(newList != NULL && "Array: Insufficient memory.\n");
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
		assert(newList != NULL && "Array: Insufficient memory.\n");
		memcpy(newList, mList, size / 2);
		mAllocator->Free((void **)(&mList));
		mList = newList;
		mCapacity = mCapacity / 2;
	}

	inline void destroy(unsigned int index)
	{
		if constexpr (std::is_pointer_v<T>)
		{
			delete mList[index];
			mList[index] = NULL;
		} else if constexpr (is_shared_ptr<std::shared_ptr<T>>::value) {
			
			mList[index].~shared_ptr();
		} else if constexpr(is_unique_ptr<std::unique_ptr<T>>::value) {
			mList[index].~unique_ptr();
		}
	}

public:
	Array(Allocator *alloc, unsigned int capacity) : mCapacity(capacity), mAllocator(alloc), mLength(0)
	{
		size_t size = mCapacity * sizeof(T);
		mList = (T *)mAllocator->Alloc(size);
		memset(mList, 0, size);
	}
	Array(Allocator *alloc) : Array(alloc, 2) {}
	Array(const Array &) = delete;

	inline ~Array()
	{
		for (unsigned int i = 0; i < mLength; i++)
			destroy(i);
		mAllocator->Free((void **)(&mList));
	}

	inline void push(const T &element)
	{
		insert(element, mLength);
	}

	inline const T &pop() const
	{
		assert(mLength > 0 && "Array: is empty.\n");
		T item = mList[mLength - 1];
		remove(mLength - 1);
		return item;
	}

	inline void prepend(const T &element)
	{
		insert(element, 0);
	}

	inline void insert(const T &element, unsigned int index)
	{
		assert(index >= 0 && index <= mLength && "Array: Index out of range.\n");
		expand();

		for (unsigned int i = mLength; i > index; i--)
			mList[i] = mList[i - 1];

		mList[index] = element;
		mLength++;
	}

	inline void remove(unsigned int index)
	{
		assert(index >= 0 && index < mLength && "Array: Index out of range.\n");
		shrink();
		destroy(index);
		for (unsigned int i = index; i < mLength - 1; i++)
			mList[i] = mList[i + 1];
		mList[mLength - 1] = NULL;
		mLength--;
	}

	inline void remove(T item)
	{
		unsigned int j = 0;
		for (unsigned int i = 0; i < mLength; i++)
		{
			if (mList[i + j] == item)
			{
				destroy(i + j);
				j++;
			}
			if (j > 0)
				mList[i] = mList[i + j];
		}
		mLength -= j;
	}

	inline T &operator[](unsigned int index)
	{
		assert(index >= 0 && index < mLength && "Array: Index out of range.\n");
		return mList[index];
	}

	inline int find(const T &obj)
	{

		for (unsigned int i = 0; i < mLength; i++)
		{
			if (mList[i] == obj)
				return i;
		}
		return -1;
	}

	inline T &at(unsigned int index)
	{
		return this[index];
	}

	inline bool isEmpty()
	{
		return mLength == 0;
	}

	inline const unsigned int &size()
	{
		return mLength;
	}

	inline const unsigned int &capacity()
	{
		return mCapacity;
	}
};