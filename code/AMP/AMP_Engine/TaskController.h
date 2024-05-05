#pragma once
#include "vector"
#include "functional"
#include "list"
#include "memory"

namespace amp
{
	class AMP_Engine;
	class TaskController;
	
	/// Task inside the AMP engine
	/// 
	/// **General Idea :** A task contains code, which becomes executed as long as a certain game state has not changed
	/// The task state is defined here as two events
	/// **1.)** The start event, when the state changes to the task state
	/// **2.)** The end event, when the task state changes again
	/// @note Tasks should be created inside an actor in the  @see Actor::AfterWindowStartup() method
	/// @note After a task is created, all game objects can start, end or reset a task
	/// @note Creating a certain actor for calling tasks and changing game states can be helpful
	/// @note Inherit from this class to create your game task
	/// ### ExampleTask inside an Actor
	///```cpp
	/// void ExampleActor::AfterWindowStartup()
	/// {
	/// 	//... some components set up
	///		//controller is an instance of @see TaskController
	/// 	controller->AddTask<ExampleTask>(...);
	/// }
	/// ```
	class Task {
		using CustomIter = std::list<Task>::iterator;
	
	public:
		Task() {};
		virtual ~Task(){};
		Task(const Task&) = delete;
		Task(Task&&) = delete;
	
		/// This function is called after the task was added to the task Controller
		///
		/// Use cases: Subscriptions of events, other initialization processes
		virtual void Init() {};
		
		/// This is the start function of your task, you can also define your own start function with an improved function name
		///
		/// @note You should call inside this function Update() or similar to receive the Update() callback
		virtual void Start() {};
	
		/// When your task needs updates per frame you can use this function
		///
		/// @note In order to receive updates you need to call AddUpdate() or similar inside your start function
		/// @note It is also necessary that you TaskController has the TaskController::CallUpdateForTasks() method called inside the actors Actor::Update() method
		virtual void Update() {};
		
		/// This is the end function of your task, you can also define your own end function with an improved function name
		/// 
		/// @note You should call inside this function RemoveUpdate() or similar to remove the Update() callback
		/// @note You should also shutdown everything you started in your Start() or custom Start() function
		virtual void End() {};
		
		/// This is a mandatory function, the task should behave after this function call like a newly created task
		/// 
		/// @note You should call here ResetUpdate() to safely remove the Update() callback when it exists
		/// @note Reset in here all variables you have modified in your Start() or custom start function
		virtual void Reset() = 0;
	
	protected:
		AMP_Engine* engine = nullptr;
		TaskController* controller = nullptr;
		

		/// Activates the Update() callback
		///
		/// @note Throws error when called multiple times without calling End() or Reset() before (btw: RemoveUpdate())
		void AddUpdate();

		/// Activates the Update() callback
		/// 
		/// @note Can be called multiple times
		/// @note Each call increases a counter, if counter > 0 the update callback is activated
		/// @note Counter can be decreased by calling RemoveSharedUpdate();
		/// Use cases: Using multiple small coherent tasks inside one task 
		void AddSharedUpdate();

		/// @return if the update callback is activated
		bool IsUpdateActive();
	
		/// Deactivates the Update() callback
		/// 
		/// @note Throws error when called multiple times without calling Start() or custom start() before (btw: AddUpdate())
		void RemoveUpdate();

		/// Can deactivate the Update() callback
		/// 
		/// @note Can be called multiple times
		/// @note Each call decreases a counter, if counter == 0 update callback is deactivated
		/// @note If counter < 0 an error is thrown
		/// Use cases: Using multiple small coherent tasks inside one task 
		void RemoveSharedUpdate();

		/// Resets (removes) Update callback 
		/// 
		/// @note Removes in all cases the Update() callback
		/// @note Can also be called when Update() is not activated
		/// @note Resets the SharedUpdate counter
		void ResetUpdate();
		
	private:
		void setEngine(AMP_Engine* n) {
			if (!n) throw "invalid engine pointer";
			engine = n;
		}
	
		void setController(TaskController* c) {
			controller = c;
		}
		int SharedCounter = 0;
		bool isUpdateActive = false;
		friend class TaskController;
	};
	
	/// TaskController inside the AMP engine
	/// 
	/// @note Can be easily created inside the constructor of each actor by calling Actor::CreateTaskController()
	/// @note Controls an many task as you want
	/// @note Inside your Actor you need to call CallUpdateForTasks() inside your update method and clear() inside your actor destructor
	/// @note When not calling these function the task controller and the attached tasks are not working properly
	/// @note This controller deletes itself when the actor attached to is destroyed (e.g: a new Scene is loaded)
	class TaskController
	{
		using CustomIter = std::list<Task>::iterator;
	
	
		AMP_Engine* engine = nullptr;
		std::list<std::unique_ptr<Task>> taskVector;
		std::vector<Task*> updateTasks;
		std::vector<Task*> removeVector;
		
	public:
		/// The engine is passed here to make sure each task has en engine reference
		TaskController(AMP_Engine* engine) : engine(engine) {};
		TaskController(){};
		~TaskController() {};
		TaskController(const TaskController&) = delete;
		TaskController(TaskController&&) = delete;
	
		/// Creates and Attaches a task to the TaskController
		/// 
		/// @param ...args the arguments which are passed to the constructor of your custom task
		/// @return a pointer to your task, the pointer is just as long alive as the TaskController is alive
		/// @note The function calls automatically the Task::Init() function
		template<typename T,typename ... Args>
		T* AddTask(Args ... args) {
			if (!engine)throw "engine ptr must be initialized first";
			taskVector.push_back(std::make_unique<T>(std::forward<Args>(args)...));
			auto* t = taskVector.back().get();
			t->setController(this);
			t->setEngine(engine);
			t->Init();
			return static_cast<T*>(t);
		}
	
		/// Clears all Tasks from the task Controller
		/// 
		/// @note First removes all Update callbacks, second delets all tasks
		void clear() {
			taskVector.clear();
			updateTasks.clear();
			removeVector.clear();
		}
	
		/// Calls Task::Update() for each update activated task attached to this controller
		/// 
		/// @attention Should be called inside the actors Actor::Update() method for properly working tasks
		/// @note When not calling this method inside an updateCallback (or just not calling it), no task receives an update
		void CallUpdateForTasks() {
			RemoveUpdateTasks();
			for (auto* iter : updateTasks) {
				iter->Update();
			}
		}

		/// This method calls the reset function of all attached tasks
		/// 
		/// @note Can be helpful when shutting down a certain process or a changing a big game state
		/// @note This methods replaces a Task::Reset() call for each attached task
		void ResetAllActiveTasks() {
			for (auto& task : taskVector) {
				task.get()->Reset();
			}
			removeVector.clear();
			updateTasks.clear();
		}

	private:
		void RemoveUpdateTasks() {
			for (auto& toRemove : removeVector)
			{
				updateTasks.erase(
					std::remove_if(updateTasks.begin(), updateTasks.end(), [&](auto A) {return toRemove == A; }),
					updateTasks.end());
			}
			removeVector.clear();
		}


		void AddUpdate(Task* iter) {
			//DEBUG
			if (IsRemovePrepared(iter))return;
			for (auto& elem : updateTasks)if (elem == iter)throw "you can not add element multiple times";
			updateTasks.push_back(iter);
		}

		bool IsRemovePrepared(Task* iter) {
			bool hasRemoved = false;
			removeVector.erase(std::remove_if(removeVector.begin(), removeVector.end(), [&](auto A) {return iter == A ? hasRemoved = true : false; }),
				removeVector.end());
			if (hasRemoved)return true;
			return false;
		}


		void RemoveUpdate(Task* iter) {
			removeVector.push_back(iter);
		}
	
		void SetEngine(AMP_Engine* ptr) {
			engine = ptr;
		}

		bool IsEngineValid() {
			if (engine)return true;
			else return false;
		}

		friend class Task;
		friend class Actor;
	};
	
}
