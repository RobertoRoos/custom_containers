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

	fifo()
		: m_head(0)
		, m_tail(0)
	{}

	/**
	 * @defgroup Capacity
	 */
	/* @{ */

	/**
	 * Make buffer empty (memory is not actually overwritten).
	 */
	void truncate()
	{
		m_tail = 0;
		m_head = 0;
	}

	/**
	 * Get current active size.
	 */
	std::size_t size() const noexcept
	{
		return m_head - m_tail;
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
	std::size_t free() const
	{
		return this->max_size() - size();
	}

	bool full() const
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

		const std::size_t head_real = m_head % this->max_size();
		this->operator[](head_real) = v;
		// `m_head` can actually exceed max_size
		m_head++;
		// Don't modulo, only do that on tail updates
	}

	value_type pop()
	{
		if(empty())
			std::__throw_out_of_range("fifo::pop"); // No items left

		const auto v = this->operator[](m_tail);
		m_tail++;
		if(m_tail >= this->max_size()) {
			m_tail = 0;
			m_head -= this->max_size();
			// Update both, instead of each modulo-ing each time
		}
		return v;
	}

	template <typename other_iterator>
	void push_list(other_iterator other_begin, other_iterator other_end)
	{
		;
	}

	/* }@ */

protected:
	std::size_t m_tail; // Index of the next value to read
	std::size_t m_head; // Index of the next value to write
			    // Tail-to-head is the section with valid data

	// The tail is modulo'ed to the max size, the head is limited to the max_size * 2
	// such that the difference is always the current size
};

} // namespace cc

#endif /* FIFO_H */
