#pragma once

#include <stddef.h>

inline size_t nextClassId()
{
	static size_t lastID{0u};
	return lastID++;
}

template <typename T>
class PoolTrait
{
public:
	using value_type = T;

	inline PoolTrait() noexcept {
		printf("trait create!\n");
	};

	template <class T>
	inline PoolTrait(const PoolTrait<T> &) noexcept {};

	inline T *allocate(size_t n)
	{
		printf("pooltrait: alloc: %zu \n", n * sizeof(T));
		return static_cast<T *>(operator new(n * sizeof(T)));
	}

	inline void deallocate(T *p, size_t n) { operator delete(p); }

	inline void construct(T *p, T const &val) { new ((void *)p) T(val); }

	inline void destroy(T *p) { p->~T(); }
};