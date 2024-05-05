#pragma once

#include "event.h"
#include "Memory/AMP_HashMap.h"
#include "list"
#include "memory"
#include "Text.h"
#include "functional"
#include "typeinfo"
#include "Static/TimeManager.h"



namespace amp
{
	/// This is the AMP TimeEvent 
	/// 
	/// You can create TimeEvents almost everywhere in the engine
	/// With time events you can specify a function which will become executed in the future
	/// @note All subscribed time events become unsubscribed when a scene is unloaded
	/// @attention Make sure when attaching a function of a certain object, that when the function is executed in the future the object is still alive
	/// @attention This can produce crashes, which are hard to debug
	struct TimeEvent {
		
		///Each time event has its own unique id, which is the time when you called the constructor
		TimeEvent() {
			timeId = TIME::currentTimeInNanos();
		};
		
		/// Calculates the time, when the attached method is executed
		void calculatePublishTime() {
			publishTime = (int64_t)(TimeInSeconds * NANOS_IN_SECOND) + TIME::currentTimeInNanos();
		}

		/// This function initializes the TimeEvent 
		/// 
		/// @note This function is calculating the publish time
		/// @param TimeInSeconds When the attached function becomes called in the future **value must be > 0**
		/// @param IsLooped If true, every TimeInSeconds the specified callback function is called
		/// @param f The function attached to this time event
		/// @param ref The owner of this function
		/// @param ...args These are the arguments the attached function takes
		/// ### Example Time Event
		/// ```cpp
		/// ExampleActor::AfterWindowStartup(){
		///		amp::TimeEvent ev;
		///		ev.init(5.0f, false, &ExampleActor::AfterTimerIsOver, this);
		///		engine->addTimerFunction(ev);
		/// }
		///```
		template<typename Class, typename ... Args>
		void init(float TimeInSeconds, bool IsLooped, void(Class::* f)(Args ...), Class* ref, Args ... args) {
			this->CalledFunction = std::bind(f, ref, std::forward<Args>(args) ...);
			this->TimeInSeconds = TimeInSeconds;
			this->IsLooped = IsLooped;
			calculatePublishTime();
		}

		/// Compares two time events based on their timeId (The time when TimeEvent was created)
		bool operator==(TimeEvent Rhs) {
			return this->timeId == Rhs.timeId;
		}

		float TimeInSeconds = 0.0f;
		bool IsLooped = false;
		std::function<void()> CalledFunction;
		int64_t timeId;
	private:
		friend class amp::AMP_Engine;
		friend class EventSystem;
		int64_t publishTime = 0;

	};


	/// This is the AMP event system
	/// 
	/// An event works like the following. You can subscribe certain events and always when this event is fired you receive a callback in shape of a function call.
	/// You can subscribe, unsubscribe and fire events everywhere as long you have an engine reference 
	/// @note All subscribed events become unsubscribed when a scene is unloaded
	/// @attention Make sure when subscribing an event to unsubscribe the event when your object is delete
	/// @note OnActivate and OnDeactivate functions should be used to subscribe or unsubscribe events
	class EventSystem {
	
	public:
		EventSystem(const EventSystem&) = delete;
		EventSystem(EventSystem&&) = delete;
	
		EventSystem() {};
		using  callback_t = typename std::function<void()>;
		using  callback_container_t = typename std::list<callback_t>;
		using  handle_t = typename callback_container_t::iterator;
	
		/// @return The type ID of a typename
		template<typename T>
		size_t getTypeID() {
			return typeid(T).hash_code();
		}
		

		/// @name Subscription of game events
		/// @brief With these methods you can subscribe game events
		/// @brief Each subscribed game event must have a unique eventName. With this name you can later publish this event
		/// @brief You specify also a callback function which is called when the event is fired
		/// @brief The callback function can have up to three parameters
		/// @brief Each event has a unique signature, the first subscriber to an event defines this signature
		/// @brief Subscribing an event with wrong parameters will result in an error
		/// @brief Each of these function returns a handle. You need this handle to unsubscribe the event.
		/// @brief It is a good practice to define the event names of your subscribed events inside the header file of your class/struct and the signature as a comment
		/// ### Subscription example
		/// ```cpp
		/// ExampleActor::OnActivate(){
		///		engine->events.subscribeEvent(TEXT("myAwesomeUniqueEventName"), &ExampleActor::MyAwesomeCallbackFunction, this);
		/// }
		/// ```
		/// @{
		//0 Parameters
		template<typename Class>
		inline auto subscribeEvent(TEXT eventName, void(Class::* f)(), Class* ref) {
			if (auto result = eventMap.tryFind_unique(eventName.getHash()))
			{
				if ((*result)->types.size() != 0)throw "Added Function has wrong Number of Parameters";
				std::function<void()> fptr = std::bind(f, ref);
				return (*result)->subscribe(fptr);
			}
			else
			{
				std::unique_ptr<event> MyEvent = std::make_unique<event>();
				std::function<void()> fptr = std::bind(f, ref);
				auto itr = MyEvent.get()->subscribe(fptr);
				eventMap.add_unique(eventName.getHash(), std::move(MyEvent));
				return itr;
			}
		}
	
		//1 Parameters
		template<typename Class,typename OneParam>
		auto subscribeEvent(TEXT eventName,void(Class::*f)(OneParam),Class* ref) {
			if (auto result = eventMap.tryFind_unique(eventName.getHash()))
			{
	 			if ((*result)->types.size() != 1)throw "Added Function has wrong Number of Parameters";
	 			if ((*result)->types[0] != getTypeID<OneParam>())throw "Function Parameter has wrong type";
				auto* helper = (eventOneParam<OneParam>*)(*result);
				std::function<void()> fptr = std::bind(f, ref, std::cref(helper->param1));
				return (*result)->subscribe(fptr);
			}
			else
			{
				std::unique_ptr<event> MyEvent = std::make_unique<eventOneParam<OneParam>>();
				//Check Type of Param
				MyEvent.get()->types.push_back(getTypeID<OneParam>());
				auto& param1 = ((eventOneParam<OneParam>*)(MyEvent.get()))->param1;
				std::function<void()> fptr = std::bind(f, ref, std::cref(param1));
				auto itr = MyEvent.get()->subscribe(fptr);
				eventMap.add_unique(eventName.getHash(), std::move(MyEvent));
				return itr;
			}
		}
	
		//2 Parameters
		template<typename Class, typename OneParam,typename TwoParam>
		inline auto subscribeEvent(TEXT eventName, void(Class::* f)(OneParam,TwoParam), Class* ref) {
			if (auto result = eventMap.tryFind_unique(eventName.getHash()))
			{
				if ((*result)->types.size() != 2)throw "Added Function has wrong Number of Parameters";
				if ((*result)->types[0] != getTypeID<OneParam>() ||
					(*result)->types[1] != getTypeID<TwoParam>())throw "Function Parameter has wrong type";
				auto* helper = (eventTwoParam<OneParam,TwoParam>*)(*result);
				std::function<void()> fptr = std::bind(f, ref, std::cref(helper->param1),std::cref(helper->param2));
				return (*result)->subscribe(fptr);
			}
			else
			{
				std::unique_ptr<event> MyEvent = std::make_unique<eventTwoParam<OneParam,TwoParam>>();
				//Set Param Types
				MyEvent.get()->types.push_back(getTypeID<OneParam>());
				MyEvent.get()->types.push_back(getTypeID<TwoParam>());
				auto* eventPtr = (eventTwoParam<OneParam,TwoParam>*)(MyEvent.get());
				auto& param1 = eventPtr->param1;
				auto& param2 = eventPtr->param2;
				std::function<void()> fptr = std::bind(f, ref, std::cref(param1),std::cref(param2));
				auto itr = MyEvent.get()->subscribe(fptr);
				eventMap.add_unique(eventName.getHash(), std::move(MyEvent));
				return itr;
			}
		}
	
		//3 Parameters
		template<typename Class, typename OneParam, typename TwoParam,typename ThreeParam>
		inline auto subscribeEvent(TEXT eventName, void(Class::* f)(OneParam,TwoParam,ThreeParam), Class* ref) {
			if (auto result = eventMap.tryFind_unique(eventName.getHash()))
			{
				if ((*result)->types.size() != 3)throw "Added Function has wrong Number of Parameters";
				if ((*result)->types[0] != getTypeID<OneParam>() ||
					(*result)->types[1] != getTypeID<TwoParam>() ||
					(*result)->types[2] != getTypeID<ThreeParam>())throw "Function Parameter has wrong type";
				auto* helper = (eventThreeParam<OneParam,TwoParam,ThreeParam>*)(*result);
				std::function<void()> fptr = std::bind(f, ref, std::cref(helper->param1), std::cref(helper->param2),std::cref(helper->param3));
				return (*result)->subscribe(fptr);
			}
			else
			{
				std::unique_ptr<event> MyEvent = std::make_unique<eventThreeParam<OneParam,TwoParam,ThreeParam>>();
				//Set Param Types
				MyEvent.get()->types.push_back(getTypeID<OneParam>());
				MyEvent.get()->types.push_back(getTypeID<TwoParam>());
				MyEvent.get()->types.push_back(getTypeID<ThreeParam>());
				auto* eventPtr = (eventThreeParam<OneParam,TwoParam,ThreeParam>*)(MyEvent.get());
				auto& param1 = eventPtr->param1;
				auto& param2 = eventPtr->param2;
				auto& param3 = eventPtr->param3;
				std::function<void()> fptr = std::bind(f, ref, std::cref(param1), std::cref(param2),std::cref(param3));
				auto itr = MyEvent.get()->subscribe(fptr);
				eventMap.add_unique(eventName.getHash(), std::move(MyEvent));
				return itr;
			}
		}
		/// @}
	
		/// Unsubscribes an event
		/// 
		/// @param eventName The name of the event, which you want to unsubscribe
		/// @param retValueFromSubscribe This is the handle you received, when you subscribed this event
		void unsubscribeEvent(TEXT eventName,handle_t retValueFromSubscribe) {
			if (auto result = eventMap.tryFind_unique(eventName.getHash()))
			{
				(*result)->unsubscribe(retValueFromSubscribe);
			}
		}
	

		/// @name Publishing of game events
		/// @brief With these methods you can publish game events
		/// @brief You call every event with these methods, also events which do not exist without throwing errors
		/// @brief But all of these events return the actual number callbacks, which were executed
		/// @brief In order to make sure at least one callback was executed, check if the return value is > 0
		/// @brief You can publish events with up to 3 parameters
		/// @brief Publishing an event with the wrong number or type will fail in an error. (The first subscriber to an event defines the signature)
		/// @brief In order to publish an event you just need to know the name of the event and the signature
		/// ### Subscription example
		/// ```cpp
		/// ExampleActor::Update(){
		///		if(eventIsReadyToFire){
		///			//The assert makes sure, that at least one callback is executed
		///			assert(engine->events.publishEvent(TEXT("myAwesomeUniqueEventName"), 42.0f, "a_nice_char_array"));
		///		}
		/// }
		/// ```
		/// @{
		//0 parameter, return: number of callbacks
		int publishEvent(TEXT eventName) {
			if (auto result = eventMap.tryFind_unique(eventName.getHash()))
			{
				if ((*result)->types.size() != 0)throw "Publish Function has wrong Number of Parameters";
				return (*result)->fire();
			}
			return 0;
		}
	
		//1 parameter, return: number of callbacks
		template<typename OneParam>
		int publishEvent(TEXT eventName,OneParam param1) {
			if (auto result = eventMap.tryFind_unique(eventName.getHash()))
			{
	 			if ((*result)->types.size() != 1)throw "Publish Function has wrong Number of Parameters";
	 			if ((*result)->types[0] != getTypeID<OneParam>())throw "Function Parameter has wrong type";
				auto casted = (eventOneParam<OneParam>*)(*result);
				casted->param1 = param1;
				return (*result)->fire();	
			}
			return 0;
		}
	
		//2 parameter, return: number of callbacks
		template<typename OneParam,typename TwoParam>
		int publishEvent(TEXT eventName, OneParam param1,TwoParam param2) {
			if (auto result = eventMap.tryFind_unique(eventName.getHash()))
			{
				if ((*result)->types.size() != 2)throw "Publish Function has wrong Number of Parameters";
				if ((*result)->types[0] != getTypeID<OneParam>() ||
					(*result)->types[1] != getTypeID<TwoParam>())throw "Function Parameter has wrong type";
				auto casted = (eventTwoParam<OneParam,TwoParam>*)(*result);
				casted->param1 = param1;
				casted->param2 = param2;
				return (*result)->fire();
			}
			return 0;
		}
	
		//3 parameter, return: number of callbacks
		template<typename OneParam, typename TwoParam,typename ThreeParam>
		int publishEvent(TEXT eventName, OneParam param1, TwoParam param2,ThreeParam param3) {
			if (auto result = eventMap.tryFind_unique(eventName.getHash()))
			{
				if ((*result)->types.size() != 3)throw "Publish Function has wrong Number of Parameters";
				if ((*result)->types[0] != getTypeID<OneParam>() ||
					(*result)->types[1] != getTypeID<TwoParam>() ||
					(*result)->types[2] != getTypeID<ThreeParam>())throw "Function Parameter has wrong type";
				auto casted = (eventThreeParam<OneParam,TwoParam,ThreeParam>*)(*result);
				casted->param1 = param1;
				casted->param2 = param2;
				casted->param3 = param3;
				return (*result)->fire();
			}
			return 0;
		}
		/// @}
		
		/// Subscription of a time event or also called delay
		/// 
		/// @param timeEvent A time event containing all neccessary information for publishing an event in the future
		void addTimerFunction(TimeEvent& timeEvent) {
	 		auto Itr = timeEventList.rbegin();
	 		auto ItrEnd = timeEventList.rend();
			auto insertItr = timeEventList.end();
			for (; Itr != ItrEnd; ++Itr) {
				if (Itr->publishTime <= timeEvent.publishTime) {
					timeEventList.insert(insertItr, timeEvent);
					return;
				}
				--insertItr;
			}
			timeEventList.push_front(timeEvent);
		}

		/// Removes a timer event, in most cases you just want to remove looped time events
		/// 
		/// @param timeEvent Removes the time event you have initialized earlier (must be the same one)
		void removeTimerFunction(TimeEvent& timeEvent) {
			removeVector.push_back(timeEvent);
		}

private:
		void clear() {
			timeEventList.clear();
			removeVector.clear();
			eventMap.clear();
		}

		void publishTimeEvent() {
			if (timeEventList.size() == 0)return;
			if (removeVector.size() != 0)UnloopTimeEvents();
			std::vector<TimeEvent> loopElements;
			auto itr = timeEventList.begin();
			auto endItr = timeEventList.end();
			int64_t currTime = TIME::currentTimeInNanos();
			while (itr != endItr) {
				if (currTime >= itr->publishTime) {
					itr->CalledFunction();
					if (timeEventList.size() == 0)return;
					if (itr->IsLooped)loopTimer(*itr);
					timeEventList.erase(itr++);
				}
				else break;
			}
		}

		void UnloopTimeEvents()
		{
			for (auto& removeItem : removeVector) {
				timeEventList.erase(
					std::remove_if(timeEventList.begin(), timeEventList.end(),
						[&](TimeEvent& A) {return A.timeId == removeItem.timeId; }),
					timeEventList.end()
				);
			}
			removeVector.clear();
		}

		void loopTimer(TimeEvent& timeEvent) {
			timeEvent.calculatePublishTime();
			addTimerFunction(timeEvent);
		}
		
		std::list<TimeEvent> timeEventList;
		std::vector<TimeEvent> removeVector;
		HashMap<size_t, std::unique_ptr<event>> eventMap;
		friend class Scene;
		friend class AMP_Engine;
	};
}