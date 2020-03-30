#pragma once

#include <memory>
#include <limits>

template<class T>class UninitializedAllocator final
{
public:
	UninitializedAllocator() {}
	UninitializedAllocator(const UninitializedAllocator&) {}
	template<class U>UninitializedAllocator(const UninitializedAllocator<U>&) {}
	UninitializedAllocator operator=(const UninitializedAllocator&) = delete;
	UninitializedAllocator(UninitializedAllocator&&) = delete;
	UninitializedAllocator operator=(UninitializedAllocator&&) = delete;

public:
	typedef T value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef typename std::true_type propagate_on_container_move_assignment;
	typedef typename std::true_type is_always_equal;

	template <class U> using rebind_alloc = UninitializedAllocator<U>;

	constexpr T* allocate(const size_type num)
	{
		return reinterpret_cast<T*>(::operator new(num * sizeof(T)));
	}

	void deallocate(T* const ptr, const size_t)
	{
		::operator delete(ptr);
	}

	static constexpr size_type max_size() noexcept
	{
		return std::numeric_limits<size_type>::max();
	}

};

namespace std
{
	template<class T> class allocator_traits<UninitializedAllocator<T>> final
	{
	public:
		typedef UninitializedAllocator<T> allocator_type;
		typedef typename allocator_type::value_type value_type;
		typedef typename allocator_type::size_type size_type;
		typedef typename allocator_type::difference_type difference_type;
		typedef T* pointer;
		typedef const T* const_pointer;

		template <class U> using rebind_alloc = UninitializedAllocator<U>;

	public:
		template< class T, class... Args > static constexpr void construct(allocator_type& allocator, T* p, Args&&... args)
		{
		}
		template< class T> static constexpr void destroy(allocator_type& allocator, T* p)
		{
		}
		static constexpr T* allocate(allocator_type& a, const size_type num)
		{
			return a.allocate(num);
		}

		static void deallocate(allocator_type& a, T* const ptr, const size_t size)
		{
			return a.deallocate(ptr, size);
		}
		static constexpr size_type max_size(const allocator_type& a) noexcept
		{
			return a.max_size();
		}
	};
}
template<class T>bool operator==(const UninitializedAllocator<T>& a, const UninitializedAllocator<T>& b)
{
	return true;
}
template<class T>bool operator!=(const UninitializedAllocator<T>& a, const UninitializedAllocator<T>& b)
{
	return false;
}

