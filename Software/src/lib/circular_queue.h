/**
 * @file src/util/circular_queue.h
 *
 * @brief Implements a header-only circular queue with STL-like public APIs.
 */

/*
 * This file is part of RTLib.
 *
 * Copyright (C) 2018-2019 Derppening <david.18.19.21@gmail.com>
 *
 * RTLib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RTLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with RTLib.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef RTLIB_UTIL_CIRCULAR_QUEUE_H_
#define RTLIB_UTIL_CIRCULAR_QUEUE_H_

#include <algorithm>
#include <array>
#include <utility>

namespace rtlib::util {

/**
 * A queue implementation using an @c std::array, implemented in a circular manner.
 *
 * @tparam T Type of the stored elements.
 * @tparam N Maximum elements that can be stored.
 */
template<typename T, std::size_t N>
class circular_queue {
 public:
  static_assert(N > 0, "circuar_queue must have at least one element");

  /**
   * @brief Type of the underlying container.
   */
  using container_type = std::array<T, N>;
  /**
   * @brief Type of the stored elements. Equivalent to `T`.
   */
  using value_type = typename container_type::value_type;
  /**
   * @brief Type used to represent sizes and indices.
   */
  using size_type = typename container_type::size_type;
  /**
   * @brief Reference type for the stored elements. Equivalent to `T&`.
   */
  using reference = typename container_type::reference;
  /**
   * @brief Constant reference type for the stored elements. Equivalent to `const T&`.
   */
  using const_reference = typename container_type::const_reference;

  /**
   * Default constructor. Value-initializes the container.
   */
  circular_queue() noexcept : _data() {}

  /**
   * @brief Conversion constructor from @c std::array.
   *
   * Converts an @c std::array to a circular_queue. If the buffer of this object is smaller than the array, the first
   * @c N elements will be copied. Otherwise, the elements from the array will be copied, and the remaining elements
   * will be default-constructed.
   *
   * The @c size() after initialization will be the equivalent of `std::min(SIZE, N)`.
   *
   * @tparam SIZE Size of the @c std::array.
   * @param[in] cont Data to copy from.
   */
  template<std::size_t SIZE>
  explicit circular_queue(const std::array<T, SIZE>& cont)
  noexcept(std::is_nothrow_copy_assignable<T>::value && std::is_nothrow_default_constructible<T>::value) :
      _begin(_data.begin()),
      _end(_data.begin() + std::min(SIZE, N)),
      _size(std::min(SIZE, N)) {
    std::copy(cont.begin(), cont.begin() + _size, _begin);
    std::fill(_end, _data.end(), T());
  }

  /**
   * @brief Move conversion constructor from @c std::array.
   *
   * Move-constructs the underlying @c std::array with `std::move(cont)`.
   *
   * The @c size() after initialization will be `N`.
   *
   * @param[in] cont Container to initialize from.
   */
  explicit circular_queue(container_type&& cont) noexcept(std::is_nothrow_move_assignable<container_type>::value)
      : _data(std::move(cont)), _begin(_data.begin()), _end(_data.end()), _size(N) {}

  /**
   * @brief Copy constructor.
   *
   * Copy-constructs this circular_queue with another one.
   *
   * @param[in] other The source circular queue to copy from.
   */
  circular_queue(const circular_queue& other) noexcept(std::is_nothrow_copy_constructible<container_type>::value) :
      _data(other._data),
      _begin(_data.begin() + (other._begin - other._data.begin())),
      _end(other._end == nullptr ? nullptr : _data.begin() + (other._end - other._data.begin())),
      _size(other._size) {}

  /**
   * @brief Move constructor.
   *
   * Move-constructs this circular queue with another one.
   *
   * @param[in] other The source circular queue to move from.
   */
  circular_queue(circular_queue&& other) noexcept(std::is_nothrow_move_constructible<container_type>::value) :
      _data(std::move(other._data)),
      _begin(_data.begin() + (other._begin - other._data.begin())),
      _end(other._end == nullptr ? nullptr : _data.begin() + (other._end - other._data.begin())),
      _size(other._size) {}

  /**
   * @brief Default destructor.
   */
  ~circular_queue() = default;

  /**
   * @brief Copy assignment operator.
   *
   * Replaces the contents with a copy of @p other.
   *
   * @param other The circular_queue to copy from.
   * @return `*this`.
   */
  circular_queue& operator=(const circular_queue& other) noexcept(std::is_nothrow_copy_assignable<container_type>::value) {
    if (&other == this) {
      return *this;
    }

    _data = other._data;
    _begin = _data.begin() + (other._begin - other._data.begin());
    _end = other._end == nullptr ? nullptr : _data.begin() + (other._end - other._data.begin());
    _size = other._size;

    return *this;
  }

  /**
   * @brief Move assignment operator.
   *
   * Replaces the contents with @p other using move semantics.
   *
   * @param other The circular_queue to move from.
   * @return `*this`.
   */
  circular_queue& operator=(circular_queue&& other) noexcept(std::is_nothrow_move_assignable<container_type>::value) {
    if (&other == this) {
      return *this;
    }

    _data = std::move(other._data);
    _begin = _data.begin() + (other._begin - other._data.begin());
    _end = other._end == nullptr ? nullptr : _data.begin() + (other._end - other._data.begin());
    _size = other._size;

    return *this;
  }

  /**
   * @brief Returns a reference to the first element.
   *
   * The behavior is undefined when there is no element in the circular_queue.
   *
   * @return Reference to the first element.
   */
  reference front() noexcept {
    return *_begin;
  }

  /**
   * @brief Returns a constant reference to the first element.
   *
   * If there is no element in the circular_queue, a default-constructed `T` will be returned.
   *
   * @return Constant reference to the first element.
   */
  const_reference front() const noexcept(std::is_nothrow_default_constructible<T>::value) {
    if (empty()) {
      return T();
    }

    return *_begin;
  }

  /**
   * @brief Returns a reference to the last element.
   *
   * The behavior is undefined if there is no element in the circular_queue.
   *
   * @return Reference to the last element.
   */
  reference back() noexcept {
    return _end[-1];
  }

  /**
   * @brief Returns a constant reference to the last element.
   *
   * If there is no element in the circular_queue, a default-constructed `T` will be returned.
   *
   * @return Constant reference to the last element.
   */
  const_reference back() const noexcept(std::is_nothrow_default_constructible<T>::value) {
    if (empty()) {
      return T();
    }

    return _end[-1];
  }

  /**
   * @brief Checks if the underlying container has no elements.
   *
   * @return `true` if there are no elements in the queue, `false` otherwise.
   */
  [[nodiscard]] bool empty() const noexcept { return _size == 0; }

  /**
   * @brief Returns the number of elements.
   *
   * @return The number of elements in the circular_queue.
   */
  size_type size() const noexcept { return _size; }

  /**
   * @brief Pushes the given element @p value to the end of the queue.
   *
   * Pushing an element into a full queue will silently fail.
   *
   * @param[in] value Value of the element to push.
   */
  void push(const value_type& value) noexcept(std::is_nothrow_copy_assignable<T>::value) {
    if (size() == N) {
      return;
    }

    if (empty() || _end == _data.end()) {
      _end = _data.begin();
    }

    *_end++ = value;
    ++_size;
  }

  /**
   * @brief Pushes the given element @p value to the end of the queue.
   *
   * Pushing an element into a full queue will silently fail.
   *
   * @param[in] value Value of the element to push.
   */
  void push(value_type&& value) noexcept(std::is_nothrow_move_assignable<T>::value) {
    if (size() == N) {
      return;
    }

    if (empty() || _end == _data.end()) {
      _end = _data.begin();
    }

    *_end++ = std::move(value);
    ++_size;
  }

  /**
   * @brief Pushes a new element to the end of the queue, which will be constructed in-place.
   *
   * Pushing an element into a full queue will silently fail.
   *
   * @tparam Args Types as supplied to the element's constructor.
   * @param args Arguments to forward to the constructor of the element.
   * @return A reference to the pushed element.
   */
  template<typename... Args>
  decltype(auto) emplace(Args&& ... args) noexcept(std::is_nothrow_constructible<T>::value) {
    if (size() == N) {
      return;
    }

    if (empty() || _end == _data.end()) {
      _end = _data.begin();
    }

    *_end++ = T(std::forward<Args>(args)...);
    ++_size;

    return back();
  }

  /**
   * @brief Removes an element from the top of the queue.
   */
  void pop() noexcept(std::is_nothrow_default_constructible<T>::value) {
    if (empty()) {
      return;
    }

    *_begin++ = T();
    --_size;

    if (_size == 0) {
      _begin = _data.begin();
      _end = nullptr;
    }
  }

  /**
   * @brief Exchanges the contents of this object with `other`.
   * @param other
   */
  void swap(circular_queue& other) noexcept(std::is_nothrow_swappable<container_type>::value) {
    std::swap(this->_data, other._data);
    std::swap(this->_begin, other._begin);
    std::swap(this->_end, other._end);
    std::swap(this->_size, other._size);
  }

  /**
   * @brief Specialization of `std::swap` algorithm.
   *
   * @param lhs Containers to swap.
   * @param rhs Containers to swap.
   */
  friend void swap(circular_queue& lhs,
                   circular_queue& rhs) noexcept(std::is_nothrow_swappable<container_type>::value) {
    lhs.swap(rhs);
  }

 private:
  /**
   * @brief Pointer type for the stored elements. Equivalent to `T*`.
   */
  using pointer = typename container_type::pointer;

  /**
   * @brief Underlying container for circular_queue.
   */
  std::array<T, N> _data;
  /**
   * @brief Pointer pointing to the first element of the circular_queue.
   */
  pointer _begin = _data.begin();
  /**
   * @brief Pointer pointing to the past-the-last element of the circular_queue.
   *
   * If there are no elements in the queue, this should point to `nullptr`.
   */
  pointer _end = nullptr;
  /**
   * @brief Number of elements in the circular queue.
   */
  size_type _size = 0;
};

}  // namespace rtlib::libutil

#endif  // RTLIB_UTIL_CIRCULAR_QUEUE_H_
