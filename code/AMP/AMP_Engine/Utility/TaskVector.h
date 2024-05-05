#pragma once
#include "vector"
#include "functional"
#include "optional"


/// This class is wrapper around the std::vector
/// 
/// You can safely delete elements from the vector while iterating over it
/// TaskVector uses an additional delete vector and deletes elements just when the vector becomes updated
template<typename T>
class TaskVector
{
	using vecIter = typename std::vector<T>::iterator;
public:
	/// Adds an element to the vector
	/// 
	/// @param elem Adds an element by reference to the vector
	void Add(T&& elem) {
		elementVector.push_back(elem);
	}

	/// Adds an element to the vector
	/// 
	/// @param elem Adds an element by value to the vector
	void Add(const T& elem) {
		elementVector.push_back(elem);
	}

	/// Prepares to remove an element of the vector in the next loop iteration
	/// 
	/// @note when there is just one element inside the vector, the vector becomes instantly cleared
	/// @param elem The reference element you want to remove
	void Remove(T&& elem) {
		if (elementVector.size() <= 1) {
			assert(elementVector.back() == elem);
			elementVector.clear();
		}
		else deleteVector.push_back(elem);
	}

	/// Prepares to remove an element of the vector in the next loop iteration
	/// 
	/// @note when there is just one element inside the vector, the vector becomes instantly cleared
	/// @param elem The value element you want to remove
	void Remove(const T& elem) {
		if (elementVector.size() <= 1) {
			assert(elementVector.back() == elem);
			elementVector.clear();
		}
		else deleteVector.push_back(elem);
	}

	/// Finds and returns an element inside the vector
	/// 
	/// @note When the element is not found a std::nullopt is returned. For further info https://en.cppreference.com/w/cpp/utility/optional
	/// @param elem The reference element you want to find
	std::optional<vecIter> Find(const T& elem) {
		auto itr = elementVector.begin();
		while (itr != elementVector.end()) {
			if (*itr == elem)return std::optional<vecIter>(itr);
			else ++itr;
		}
		return std::nullopt;
	}

	///The size of the vector
	///
	/// @attention Removed elements becomes deleted in the next loop iteration, so calling size() before and afert a remove call will result in the same value
	int size() {
		return elementVector.size();
	}

	//Looks for Deleting elements then iterates the vector
	//== operator must be overridden of T

	/// Calls the passed function for each vector once
	/// 
	/// @note Passes the elements of the vector as function parameters
	/// @param callback This function is called for each vector element. Signature myAwesomeExecutionFunction(int vectorIndex, vectorElement);
	/// @attention Deletes first all delete flagged elements in the vector, before iterating over the vector
	void Loop(std::function<void(int, T&)> callback) {
		for (auto& elem : deleteVector) {
			elementVector.erase(std::remove_if(elementVector.begin(), elementVector.end(), [&](T other) {return elem == other; }), elementVector.end());
		}
		deleteVector.clear();
		if (elementVector.size() == 1)callback(0, elementVector.back());
		else {
			int counter = 0;
			for (auto& elem : elementVector) {
				callback(counter, elem);
				++counter;
			}
		}
	}

	/// Clears all elements in the vector and also all delete flagged elements
	void clear() {
		elementVector.clear();
		deleteVector.clear();
	}

private:
	std::vector<T> elementVector;
	std::vector<T> deleteVector;
};

