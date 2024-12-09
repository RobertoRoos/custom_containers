#ifndef FIFO_H
#define FIFO_H

#include <array>

/**
 * Mimics std::, but 'custom'.
 */
namespace cc {

/**
 * Circular first-in, first-out buffer.
 *
 * Note that the underlying buffer has a size of Nm + 1
 *
 * @tparam Tp Type of each element
 * @tparam Nm Number of items that fit in the fifo until full
 */
template <typename Tp, std::size_t Nm>
class fifo : public std::array<Tp, Nm> {
public:
	typedef Tp value_type;

	/**
	 * Define a custom iterator for navigating fifo items.
	 *
	 * The iterator can count beyond the queue size! But when resolving to items
	 * the module is taken appropriately.
	 */
	class iterator {
	public:
		iterator(Tp* a, std::size_t i = 0)
			: m_index(i)
			, m_array(a)
		{}

		operator std::size_t() const
		{
			return m_index;
		}

		void operator=(const std::size_t& other)
		{
			m_index = other;
		}

		iterator& operator+(const iterator& other)
		{
			return iterator(m_array, m_index + other.m_index);
		}

		iterator& operator-(const iterator& other)
		{
			return iterator(m_array, m_index - other.m_index);
		}

		iterator& operator++()
		{
			m_index++;
			return *this;
		}

		iterator& operator+=(std::size_t n)
		{
			m_index += n;
			return *this;
		}

		iterator& operator-=(std::size_t n)
		{
			m_index -= n;
			return *this;
		}

		bool operator==(iterator other) const
		{
			return m_index == other.m_index;
		}

		bool operator!=(iterator other) const
		{
			return !(*this == other);
		}

		Tp& operator*()
		{
			return m_array[m_index % Nm];
		}

		// Iterator traits:
		using difference_type = std::size_t;
		using value_type = Tp;
		using pointer = const value_type*;
		using reference = const value_type&;
		using iterator_category = std::random_access_iterator_tag;

		std::size_t m_index;

	protected:
		Tp* m_array;
	};

	fifo()
		: m_head(this->data(), 0)
		, m_tail(this->data(), 0)
	{}

	/**
	 * @defgroup Capacity
	 */
	/* @{ */

	/**
	 * Make buffer empty (memory is not actually overwritten).
	 */
	void clear()
	{
		m_tail = 0;
		m_head = 0;
	}

	/**
	 * Get current active size.
	 */
	std::size_t size() const noexcept
	{
		return static_cast<std::size_t>(m_head - m_tail);
	}

	/**
	 * Returns true if no values are yet used.
	 */
	bool empty() const noexcept
	{
		return m_head == m_tail;
	}

	/**
	 * Return the number of elements still unused and available.
	 */
	std::size_t free() const noexcept
	{
		return this->max_size() - size();
	}

	bool full() const noexcept
	{
		return m_head == m_tail + this->max_size();
	}

	/* @} */

	/**
	 * @defgroup Modifying element access
	 */
	/* @{ */

	void push(const value_type& v)
	{
		if(full()) // No space left, don't quitely overwrite
			std::__throw_out_of_range("fifo::push");

		this->operator[](head_modulo()) = v;
		// `m_head` can actually exceed max_size
		++m_head;
		// Don't modulo, only do that on tail updates
	}

	value_type pop()
	{
		if(empty())
			std::__throw_out_of_range("fifo::pop"); // No items left

		const auto v = this->operator[](m_tail);
		increment_tail();
		return v;
	}

	void push_list(const value_type values[], std::size_t elements)
	{
		push_list(values, values + elements);
	}

	void push_list(const value_type* other_begin, const value_type* other_end)
	{
		const std::size_t n = other_end - other_begin;
		if(free() < n)
			std::__throw_out_of_range("fifo::push_list"); // Not enough space left

		// Copy elements until the end of the buffer:
		const std::size_t n1 = std::min(n, this->max_size() - head_modulo());
		std::copy(other_begin, other_begin + n1, this->data() + head_modulo());
		// Copy elements from the start of the buffer:
		const std::size_t n2 = n - n1;
		std::copy(other_begin + n1, other_begin + n, this->data());
		m_head += n; // Don't modulo, do that in pop_*
	}

	/**
	 * Remove multiple elements from the queue.
	 *
	 * @param n Number of items - Default: take all available items
	 */
	void pop_list(value_type* other_begin, std::size_t n = 0)
	{
		if(n > 0 && n > size())
			std::__throw_out_of_range("fifo::pop_list"); // Not enough items left
		else if(n == 0)
			n = size();

		// Copy elements until the end of the buffer:
		const std::size_t n1 = std::min(n, this->max_size() - m_tail);
		std::copy(this->data() + m_tail, this->data() + m_tail + n1, other_begin);
		// Copy elements from the start of the buffer:
		const std::size_t n2 = n - n1;
		std::copy(this->data(), this->data() + n2, other_begin + n1);
		increment_tail(n);
	}

	/* }@ */

	/**
	 * @defgroup Iterators
	 */
	/* @{ */

	/**
	 * Get iterator to first value to read (tail).
	 */
	iterator begin() noexcept
	{
		return m_tail;
	}

	/**
	 * Get iterator to last value to read + 1 (head) and reset head!
	 *
	 * This method is not const, it causes consumption of the available items!
	 */
	iterator end() noexcept
	{
		m_tail += size();
		return m_head;
	}

	const iterator cbegin() const noexcept
	{
		return m_tail;
	}

	const iterator cend() const noexcept
	{
		return m_head;
	}

	/* @} */

protected:
	void increment_tail(std::size_t incr = 1)
	{
		m_tail += incr;
		if(m_tail >= this->max_size()) {
			m_tail -= this->max_size();
			m_head -= this->max_size();
			// Update both, instead of each modulo-ing each time
		}
	}

	std::size_t head_modulo() const
	{
		return m_head % this->max_size(); // Head can exceed max_size, return modulo
	}

	iterator m_tail; // Index of the next value to read
	iterator m_head; // Index of the next value to write
			 // Tail-to-head is the section with valid data

	// The tail is modulo'ed to the max size, the head is limited to the max_size * 2
	// such that the difference is always the current size
};

} // namespace cc

#endif /* FIFO_H */
