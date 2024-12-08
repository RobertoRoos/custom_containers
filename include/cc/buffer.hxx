#ifndef buffer_H
#define buffer_H

#include <array>

/**
 * Custom containers.
 */
namespace cc {

/**
 * Extension of std::array that tracks the last used value, including a varying size.
 *
 * `size()` returns the current size, `max_size()` the buffer limit (`Nm`).
 *
 * Not all element access methods account for the current size!
 * These will honor the current size:
 *   - buffer.size()
 *   - buffer.free()
 *   - buffer.get(n)
 *   - buffer.assign(n, v)
 *   - buffer.push_back(v)
 *   - buffer.pop_back()
 *   - buffer.begin() / buffer.end()
 *
 * Whereas these interfaces will act the same as on std::array directly, not raising exceptions
 * because of, or modifying, `size()`:
 *   - v = buffer[n], buffer[n] = v
 *
 * You can iterate over used elements as usual:
 *
 * `for (auto& v : buffer) {}`
 *
 * @tparam Tp Underlying type
 * @tparam Nm Max size of this buffer
 */
template <typename Tp, std::size_t Nm>
class buffer : public std::array<Tp, Nm> {
public:
	typedef Tp value_type;
	typedef value_type* iterator;
	typedef const value_type* const_iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	buffer()
		: m_used(0)
	{}

	/**
	 * @defgroup Capacity
	 */
	/* @{ */

	/**
	 * Reset size() to zero or force it to a specific value.
	 */
	void reset(std::size_t n = 0)
	{
		m_used = n;
	}

	/**
	 * Get current active size.
	 */
	std::size_t size() const noexcept
	{
		return m_used;
	}

	/**
	 * Returns true if no values are yet used.
	 */
	bool empty() const noexcept
	{
		return size() == 0;
	}

	/**
	 * Return the number of elements still unused and available.
	 */
	std::size_t free() const
	{
		return this->max_size() - m_used;
	}

	/* @} */

	/**
	 * @defgroup Element access
	 */
	/* @{ */

	value_type& get(std::size_t n)
	{
		if(n > m_used)
			std::__throw_out_of_range("buffer::get");
		return this->at(n);
	}

	const value_type& get(std::size_t n) const
	{
		if(n > m_used)
			std::__throw_out_of_range("buffer::get");
		return this->at(n);
	}

	void assign(std::size_t n, const value_type& v)
	{
		this->at(n) = v;
		m_used = std::max(m_used, n + 1);
	}

	/**
	 * @defgroup Modifying element access
	 */
	/* @{ */

	void push_back(const value_type& v)
	{
		this->at(m_used) = v;
		m_used++;
	}

	value_type pop_back()
	{
		if(m_used == 0)
			std::__throw_out_of_range("buffer::pop");
		auto v = this->at(m_used - 1);
		m_used--;
		return v;
	}

	void fill_used(const value_type& v)
	{
		std::fill_n(this->begin(), m_used, v);
	}

	void fill_all(const value_type& v)
	{
		m_used = this->max_size();
		std::fill_n(this->begin(), m_used, v);
	}

	/* }@ */

	/**
	 * @defgroup Iterator
	 */
	/* @{ */

	// Rely on inherited begin(), etc.

	iterator end() noexcept
	{
		return iterator(this->data() + m_used);
		// Return address of first element that is not used (or outside buffer)
	}

	const_iterator end() const noexcept
	{
		return const_iterator(this->data() + m_used);
		// See end()
	}

	reverse_iterator rend() noexcept
	{
		return reverse_iterator(this->begin());
	}

	const_reverse_iterator rend() const noexcept
	{
		return const_reverse_iterator(this->begin());
	}

	const_iterator cend() const noexcept
	{
		return const_iterator(this->data() + m_used);
	}

	const_reverse_iterator crend() const noexcept
	{
		return const_reverse_iterator(this->begin());
	}

	/* @} */

	/**
	 * @defgroup Original array access
	 */

	std::array<Tp, Nm>& array()
	{
		return *this;
	}

	const std::array<Tp, Nm>& array() const
	{
		return *this;
	}

protected:
	std::size_t m_used; // Number of elements that are considered 'used'
};

} // namespace cc

#endif /* buffer_H */
