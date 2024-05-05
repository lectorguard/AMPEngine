#pragma once
#include "list"
#include "functional"
#include "Text.h"



namespace amp
{
	struct event {
	 	using  callback_t = typename std::function<void()>;
		using type = typename std::list<callback_t>;
		using callbacksIter = typename type::iterator;
	
		event(){};
		virtual ~event(){};
		
	private:
		std::list<std::function<void()>> callbacks;
		std::vector<size_t> types;
		
		/***
		 * Supplied callbacks will be executed when the event is fired.
		 * @param callback The event handler.
		 * @return An iterator that must be used when unsubscribing from this event.
		 */
		callbacksIter subscribe(std::function<void()> cb) {
			callbacks.push_back(cb);
			return --callbacks.end();
		}
	
		/***
		 * The supplied handle will be used to remove the previously installed callback.
		 * @param callback_handle The handle that was returned by subscribe() when adding the callback.
		 */
		void unsubscribe(callbacksIter callback_handle) {
			callbacks.erase(callback_handle);
		}
	
		//
		// Iterators for Range-based for loops, e.g. for(auto & s : systems) { }
		//
		typename type::const_iterator cbegin() const noexcept {
			return callbacks.cbegin();
		}
	
		typename type::const_iterator begin() const noexcept {
			return callbacks.begin();
		}
	
		typename type::iterator begin() noexcept {
			return callbacks.begin();
		}
	
		typename type::const_iterator cend() const noexcept {
			return callbacks.cend();
		}
	
		typename type::const_iterator end() const noexcept {
			return callbacks.end();
		}
	
		typename type::iterator end() noexcept {
			return callbacks.end();
		}
	
	
		int fire() {
			// callbacks might be added/removed by callbacks
			type unmodified_callbacks(callbacks);
			for (auto cb : unmodified_callbacks)cb();
			return unmodified_callbacks.size();
		}
		
		friend class EventSystem;
	};
	
	template<typename Param1>
	struct eventOneParam : public event {
	
		eventOneParam() {};
		~eventOneParam() override {};
	
		Param1 param1;
	};
	
	template<typename Param1,typename Param2>
	struct eventTwoParam : public event {
	
		eventTwoParam() {};
		~eventTwoParam() override {};
	
		Param1 param1;
		Param2 param2;
	};
	
	template<typename Param1,typename Param2,typename Param3>
	struct eventThreeParam : public event {
	
		eventThreeParam() {};
		~eventThreeParam() override {};
	
		Param1 param1;
		Param2 param2;
		Param3 param3;
	};
}