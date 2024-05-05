#pragma once
#include "unordered_map"
#include "optional"


/// This is a wrapper around the std::unordered_map
///
/// Additional functionality, works with unique pointer
template<typename KEY,typename VALUE>
struct HashMap {
	using data_t = typename std::unordered_map<KEY,VALUE>;
	typedef typename std::remove_reference<decltype(*std::declval<VALUE>())>::type T;

	HashMap() { };
	virtual ~HashMap() {};

	HashMap(const HashMap&) = delete;
	HashMap(HashMap&&) = delete;

	/// Finds an element in the map
	/// 
	/// @note There is an online documentation about the usage of std::optional : https://en.cppreference.com/w/cpp/utility/optional
	/// @param key The key with which the element was added to the map
	/// @return An optional, when element was found, the element is returned oterwise a std::nullopt is returned
	std::optional<VALUE> tryFind(const KEY& key){
		auto found = data.find(key);
		if(found!=data.end()){
			return {found->second};
		}
		return std::nullopt;
	}

	/// Finds an unique_pointer element in the map
	/// 
	/// @note There is an online documentation about the usage of std::optional : https://en.cppreference.com/w/cpp/utility/optional
	/// @param key The key with which the element was added to the map
	/// @return An optional, when element was found, the element is returned oterwise a std::nullopt is returned
	/// @attention The actual pointer is returned
	std::optional<T*> tryFind_unique(const KEY& key) {
		auto found = data.find(key);
		if (found != data.end()) {
			return found->second.get();
		}
		return std::nullopt;
	}

	/// Adds an element to the std::unordered_map
	/// 
	/// @param key The key with which the element can be found
	/// @param value The actual element you want to insert
	void add(const KEY& key,VALUE&& value){
		data.insert(std::pair<KEY,VALUE>(key,value));
	}

	/// Adds an unique_ptr to the std::unordered_map
	/// 
	/// @param key The key with which the element can be found
	/// @param value The actual unique_ptr, you want to find
	void add_unique(const KEY& key, VALUE&& value) {
		data.insert(std::make_pair(key, std::move(value)));
	}
	
	/// Clears the unordered_map
	void clear(){
		data.clear();
	}

	/// Erases an element from the map by its iterator
	/// 
	/// @param iter The iterator of the key value pair
	void erase(typename data_t::iterator iter) {
		data.erase(iter);
	}


	/// @return The number of elements in the map
	size_t count(){
		return data.size();
	}

	//
	// Iterators for Range-based for loops, e.g. for(auto & s : systems) { }
	//
	constexpr typename data_t::const_iterator cbegin() const noexcept {
		return data.cbegin();
	}

	constexpr typename data_t::const_iterator begin() const noexcept {
		return data.begin();
	}

	constexpr typename data_t::iterator begin() noexcept {
		return data.begin();
	}

	typename data_t::const_iterator cend() const noexcept {
		return data.cend();
	}

	typename data_t::const_iterator end() const noexcept {
		return data.end();
	}

	typename data_t::iterator end() noexcept {
		return data.end();
	}

private:
	std::unordered_map<KEY, VALUE> data;

};