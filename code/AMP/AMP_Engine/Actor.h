#pragma once
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "vector"
#include "Components/Component.h"
#include "list"
#include "Text.h"
#include "functional"
#include "TaskController.h"

//Watch out can Just be used inside a class Function which inherits from Actor
#define SUB(eventName , FunctionName) subIter.push_back(std::make_pair(eventName, engine->events.subscribeEvent(eventName, &FunctionName, this)))

namespace amp
{
	/// Actor Type
	enum Type {
		/// Transformation update are processed at runtime
		Dynamic =	0,
		/// No transformation updates at runtime
		Static  =	1
	};
	class AMP_Engine;

	/// Actor inside AMP Engine
	/// 
	/// @note Inherit from this class to create an Actor
	/// @note You can pass the type parameter inside the constructor of your inherited class
	/// @note Create actors inside your scenes
	/// @note Be aware that pointers to your class become invalidated after your scene is unloaded
	/// @note You can not delete an anctor from the scene but you can call SetActive()
	/// ### ExampleActor
	/// ```cpp
	///	class ExampleActor : public amp::Actor
	///	{
	///		amp::TaskController* controller = nullptr;
	///		ExampleComponent* exampleComponent = nullptr;
	///	public:
	///		//Or just ExampleActor(){ ... }; when dynamic
	///		ExampleActor(amp::Type type) :Actor(type) {
	///			exampleComponent = AddComponent<ExampleComponent>(TEXT("exampleComponent"));
	///			controller = CreateTaskController();
	///		};
	///		~HelmetActor() override {};
	///	};
	/// ```
	class Actor
	{
	public:
		using  callback_t = typename std::function<void()>;
		using callbacksIter = typename std::list<callback_t>::iterator;

		/// Constructs an actor
		/// 
		/// @param type Default type is dynamic, means the actor can be updated at runtime
		/// @note Components and the Task Controller can be created inside the constructor
		Actor(Type type = Dynamic):type(type) {};
		Actor(Actor&&) = delete;
		Actor(const Actor&) = delete;

		virtual ~Actor() {};

		/// Sets the actors transformation relativ to the parent actor
		///
		/// @note best practice is to call this function from your scene
		void SetParent(Actor* actor);
		
		/// @return true if actor has parent
		bool HasParent();

		/// @return parent or nullptr 
		Actor* GetParent();

		//make sure each component in SCENE has a Name otherwise inspector
		//is not working properly
		
		/// Adding a component to an actor
		/// 
		/// @note should be called inside the constructor of a class
		/// @param name Make sure each name in each component has a unique name
		/// @param ...args constructor arguments of inherited component class
		/// @return Object pointer of inherited component class, this ptr is invalidated after the current scene is unloaded
		template<typename T,typename ... Args>
		T* AddComponent(TEXT name = TEXT(),Args ... args) {
			auto* comp = new T(args ...);
			components.push_back(comp);
			comp->setName(name);
			return comp;
		};

		/// Creating a TaskController
		/// 
		/// @note inits the default actor TaskController
		/// @attention should be only used inside an actor
		/// @return a cast of the initialized actor TaskController
		/// @see TaskController
		TaskController* CreateTaskController() {
			if (!engine)defaultController = std::make_unique<TaskController>();
			else defaultController = std::make_unique<TaskController>(engine);
			return static_cast<TaskController*>(defaultController.get());
		}

		///Right after MainLoop starts this function is called
		///
		///@attention Do not load or set up any graphics, shader or other open gl es specific content here
		///@attention Take instead @see AfterWindowStartup()
		virtual void AfterSystemStartup() {};

		/// GameUpdate
		/// 
		/// Called each frame
		virtual void Update() {};

		/// On system before shutdown
		/// 
		/// Use cases: Free allocated memory, stop subsystems, ...
		virtual void BeforeSystemShutdown() {};
	
		/// Post display init
		/// 
		/// @note here you can call any open gl specific code
		virtual void AfterWindowStartup() {};

		/// Activates an actor (Default: All actors are activated)
		/// 
		/// @note AMP does not allow actor deletion at runtime
		/// @note Instead you can activate and deactivate actors
		/// @note When calling activate on a deactivated actor all components receive a OnActivate() callback
		/// @note When calling activate on a deactivated actor all components receive again the update callback
		/// @note You can place here for example event subscriptions
		/// ### Example
		/// ```cpp
		/// void ExampleActor::OnActivate()
		/// {
		///		//SUB Macro can just be used inside actors !!!
		///		//When using events somewhere else: yourCallbackIter = engine->events.subscribeEvents(engine->postSceneUpdate, &ExampleActor::PostSceneUpdate,this);
		///		SUB(engine->postSceneUpdate, ExampleActor::PostSceneUpdate);
		/// };
		/// ```
		virtual void OnActivate() {};

		/// Deactivates an actor
		/// 
		/// @note this function is automatically called after @see AfterWindowStartup()
		/// @note If an actor is deactivated the OnDeactivate() method is called also for each component
		/// @attention If an actor is deactivated, components do not receive an update callback anymore
		/// @note You can unsubscribe events here (Should be earlier created inside @see OnActivate())
		/// ### Example
		///```cpp
		/// void Attacker_Player::OnDeactivate()
		/// {
		///  //This function only exists inside the actor class and works just for events subscribed with the SUB(...) macro
		///	 //When using events somewhere else: engine->events.unsubscribeEvents(yourCallbackIter);
		///	 UnsubscribeEvents();
		/// }
		/// ```
		virtual void OnDeactivate() {};

		/// @name Transformation Methods
		/// @brief The transformation of all components is relative to the transformation of the actor 
		/// @{
		/// Actor translation
		void TranslateActor(float x, float y, float z) {
			Transformation = glm::translate(Transformation, glm::vec3(x, y, z));
		}
		/// Actor rotation
		void RotateActor(float angle, float x, float y, float z) {
			Transformation = glm::rotate(Transformation, glm::radians(angle), glm::vec3(x, y, z));
		}
		/// Actor scale
		void ScaleActor(float uniform) {
			Transformation = glm::scale(Transformation, glm::vec3(uniform, uniform, uniform));
		}
		/// @}

		/// Finds a certain component by name
		/// 
		/// @note Time: O(n)
		/// @param name of the component you want to find
		/// @return a ptr of the component or nullptr
		Component* GetComponentByName(TEXT name) {
			for (auto* comp : components) {
				if (comp->GetName() == name)return comp;
			}
			return nullptr;
		}

		/// Change activity state
		///
		/// @param active sets the new activity state and calls related functions if state has changed
		/// All Actors are initially active, so there is no need to call this at the beginning
		/// @note Decides if an actor with its components become updated or not 
		/// @note The actor stays in storage, for easy reactivation
		void SetActive(bool active) {
			if (active == isActive)return;
			isActive = active;
			if (active) {
				for (auto& comp : components)comp->OnActivate();
				OnActivate();
			}else {
				for (auto& comp : components)comp->OnDeactivate();
				OnDeactivate();
			}
		}

		///Unsubscribes all events which were subscribed with SUB() Macro
		void UnsubscribeEvents();

		/// @return activity state
		bool IsActive() { return isActive; };

		/// @return name of the actor
		TEXT GetName() { return name; };

		/// @return current actor transform as 4x4 matrix
		glm::mat4 getTransform() { return Transformation; };

		/// Reference to AMP
		AMP_Engine* engine = nullptr;

		/// Added Components of this actor
		std::vector<Component*> components;

		/// All callbackIter of each subscribed game event with Macro SUB(...)
		std::vector<std::pair<TEXT,callbacksIter>> subIter;
	private:
		///Just the initial (model)matrix is used, can not be changed during the game (better performance)
		Type type = Type::Dynamic;

		void SetEngine(AMP_Engine* engine);

		void SetName(TEXT name) { this->name = name; }

		void UpdateActor() {
			if (isActive)Update();
		}

		void ComponentUpdate();

		void ComponentsShutdown() {
			for (auto& comp : components) {
				delete comp;
				comp = nullptr;
			}
			components.clear();
		}

		void CallUpdateForComponents();

		//Becomes Deleted when actor is deleted
		std::unique_ptr<TaskController> defaultController = nullptr;
		Actor* parent = nullptr;
		glm::mat4 Transformation = glm::mat4(1.0f);
		bool HasInitComponentUpdate = false;
		bool isActive = true;
		TEXT name = TEXT();
		friend class AMP_Engine;
		friend class Scene;
	};
}

