#pragma once

#include "AMP_Engine.h"
#include "Memory/AMP_Memory.h"
#include "memory"
#include "cassert"
#include "Actor.h"
#include "Text.h"

namespace amp
{
	/// Scene inside the amp engine
	/// 
	/// @note inherit from this class to create a scene
	/// @note pass the engine as parameter to the scene
	/// @note load all game actors inside the @see load() function
	/// ### ExampleScene
	///```cpp
	///class ExampleScene : public amp::Scene
	/// {
	/// public:
	/// 	ExampleSceneActor* sceneActor = nullptr;
	///		ExampleUIActor* uiActor = nullptr;
	///
	/// 	ExampleScene(amp::AMP_Engine* engine) : amp::Scene(engine) {};
	/// protected:
	///		void load() override{
	///			sceneActor = CreateActor<ExampleSceneActor>(TEXT("exampleSceneActor"));
	///			uiActor = CreateUIActor<ExampleUIActor>(TEXT("exampleUIActor"));
	///		};
	/// };
	///```
	class Scene
	{
	public:
		/// AMP Scene Constructor
		/// 
		/// @param engine Must be passed from the game
		Scene(AMP_Engine* engine) {
			assert(engine != nullptr && "engine not initialized");
			this->engine = engine;
		}
		
		/// Loading new scene
		/// 
		/// @note this function should be called from your game
		/// @attention make sure when loading a new Scene to unload the current Scene (if existing)
		/// @attention you should resubscribe your level transition events after loading btw. unloading a new Scene
		/// @param unloadAssetAttributes When ticked all assets inside the asset map become deleted before the first tick starts @see CurrentSceneContainer
		void loadScene(bool unloadAssetAttributes = true) {
			if (engine->currentScene.IsActiveScene)throw "You can just have one active Scene at a time, unload the scene before loading a new one";
			else 
			{
				engine->currentScene.IsActiveScene = true;
				load();
				engine->events.publishEvent(engine->postInit);
				engine->events.publishEvent(engine->postPostInit);
				if (unloadAssetAttributes)engine->currentScene.removeUnusedRuntimeMapElements();
			}
		}
		
		/// Unload current scene
		/// 
		/// @note this function should be called from your game
		/// @attention make sure to unload the current scene not any other scene
		/// @attention you should resubscribe your level transition events after loading btw. unloading a new Scene
		/// @attention This function removes all Runtime assetMap elements @see CurrentSceneContainer, stops and removes all loaded sounds,
		/// @attention removes all subscribed events, removes all TimeEvents, deletes all actors
		void unloadScene() {
			engine->BeforeSystemShutdownCallback();
			engine->currentScene.clear(true);
			engine->events.clear();
		}
	
	protected:
		/// All actors should be created inside this function
		virtual void load(){};

		/// Creates a scene Actor
		/// 
		/// @return This is casted ptr, the pointer is just valid as long the scene is alive. So watch out :)
		/// @param ...args Arguments for actor construction
		/// @param name unique name of the actor
		/// @note Components inside an actor created with this function are rendered **first**
		template<typename T, typename ... Args>
		T* CreateActor(TEXT name,Args ... args) {
			engine->currentScene.CurrentSceneActorArray.add(std::make_unique<T>(std::forward<Args>(args)...));
			auto* s = actorAt(engine->currentScene.CurrentSceneActorArray.count() - 1);
			SetUpActor(s, name);
			return static_cast<T*> (s);
		}

		/// Creates an UI Actor
		/// 
		/// @return This is casted ptr, the pointer is just valid as long the scene is alive. So watch out :)
		/// @param ...args Arguments for actor construction
		/// @param name unique name of the actor
		/// @note Components inside an actor created with this function are rendered **second**
		/// @note Make sure when rendering transparent assets to use this function
		template<typename T, typename ... Args>
		T* CreateUIActor(TEXT name,Args ... args) {
			engine->currentScene.CurrentSceneUIArray.add(std::make_unique<T>(std::forward<Args>(args) ...));
			auto* s = UIactorAt(engine->currentScene.CurrentSceneUIArray.count() - 1);
			SetUpActor(s, name);
			return static_cast<T*> (s);
		}
	private:
		void SetUpActor(Actor* s,TEXT name)
		{
			s->SetName(name);
			s->SetEngine(engine);
			for (auto& comp : s->components)
			{
				comp->initComponent(engine);
			}
			if (engine->isRuntime) {
				s->AfterSystemStartup();
				s->AfterWindowStartup();
				s->OnActivate();
			}
		}

		Actor* actorAt(int i) {
			return engine->currentScene.CurrentSceneActorArray[i].get();
		}

		Actor* UIactorAt(int i) {
			return engine->currentScene.CurrentSceneUIArray[i].get();
		}
		AMP_Engine* engine = nullptr;
	};
}

