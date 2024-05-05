#pragma once

#include <stdexcept>
#include <array>

/// This is a wrapper around std::array
/// 
/// Array-like fixed-size container that only allows adding elements.
/// @attention This wrapper should only be used for unique_ptr
template<typename T, size_t Size>
struct LinearArray {
	LinearArray() { };
	virtual ~LinearArray() {};

	LinearArray(const LinearArray&) = delete;
	LinearArray(LinearArray&&) = delete;

	/// @return The actual number of explicitly added elements
	size_t count() const {
		return _count;
	}

	/// Clears the array
	/// 
	/// Resets all actual added elements (reset to nullptr -> Deallocation of each element in the array)
	void clear(){
		for (int i = 0; i < count(); i++) {
			data[i].reset();
		}
		_count = 0;
	}

	/// @return The maximum number of elements, which can be added
	size_t capacity() const {
		return data.size();
	}

	/// Adds an element to the array (replaces a nullptr with an actual ptr)
	///
	/// Range-checked move-insert at end
	/// @attention When the capacity() is reached and you are adding an element an error is thrown
	void add(T&& value) {
		if (!(_count < capacity()))
			throw std::out_of_range("Access out of range.");

		data[_count] = std::move(value);
		_count++;
	}

	/// Adds an element to the array (replaces a nullptr with an actual ptr)
	///
	/// Range-checked copy-insert at end
	/// @attention When the capacity() is reached and you are adding an element an error is thrown
	void add(const T& value) {
		if (!(_count < capacity()))
			throw std::out_of_range("Access out of range.");

		data[_count] = value;
		_count++;
	}

	/// Returns an element at a certain position in the array
	///
	/// Range-checked
	/// @param pos The position in the array
	/// @return T Returns element at position. If position is not in range of the array an error is thrown 
	constexpr T& at(size_t pos) {
		if (!pos < count())
			throw std::out_of_range("Access out of range.");
		return data[pos];
	}

	/// Returns an element at a certain position in the array
	///
	/// @attention Not range-checked (No error when pos is not in range), no modification
	/// @param pos The position in the array
	/// @return The element at the specified position
	constexpr const T& operator[](size_t pos) const {
		return data[pos];
	};
	
private:
	std::array<T, Size> data;
	size_t _count = 0;
};

