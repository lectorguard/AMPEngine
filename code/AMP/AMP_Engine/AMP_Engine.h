#pragma once

#include "functional"
#include "assert.h"
#include "Graphics/Context.h"
#include "android/asset_manager.h"
#include "../AssetLoader/GeneralAssetLoader.h"
#include "../AssetLoader/AnimLoaderFromGLTF.h"
#include "Graphics/FrameBuffer.h"
#include "string"
#include "Graphics/Shader.h"
#include "Actor.h"
#include "memory"
#include "Memory/AMP_Memory.h"
#include "Memory/CurrentSceneContainer.h"
#include "../AssetLoader/Attributes.h"
#include "Event/EventSystem.h"
#include "Sound/SoundPool.h"
#include "Physics/PhysicsWrapper.h"
#include "android/native_activity.h"
#include "android/configuration.h"


namespace amp {
	
	/// Android Native Glue Event Wrapper
	/// 
	/// Enum of current Android CMD, describes the current state of the application
	enum CommandsAndroid {
		NO_CMD					=	0,
		APP_CMD_INIT_WINDOW		=	1,
		APP_CMD_TERM_WINDOW		=	2,
		APP_CMD_GAINED_FOCUS	=	3,
		APP_CMD_LOST_FOCUS		=	4,
		APP_CMD_STOP			=   5
	};

	/// Android Game Activity Information
	/// 
	/// This struct contains all necessary information to init and update the engine
	struct AndroidStatus {
		CommandsAndroid commands = NO_CMD;
		ANativeWindow* window = nullptr;
		bool touchEvent = false;
		bool touchChanged = false;
		int32_t TouchX;
		int32_t TouchY;
		int animating;
		int ProcessShutdown = 0;
	};

	/// Android Mulitplayer Physics Engine (AMP)
	/// 
	/// @note inherit from this class to create a game
	/// @note pass inside the constructor the ANativeActivity* to the engine
	/// @note create your scenes inside your constructor as unique_ptr
	/// @note load the first scene inside the @see AfterWindowStartup()
	/// ### ExampleGame
	/// ```cpp
	///class ExampleGame : public amp::AMP_Engine
	///{
	/// public:
	/// 	std::unique_ptr<ExampleScene> exampleScene;
	/// 	ExampleGame(ANativeActivity* activity) : AMP_Engine(activity) {
	/// 			exampleScene = std::make_unique<ExampleScene>(this);
	/// 		};
	///};
	///```
	class AMP_Engine
	{
		using FuncHandle_t = std::function<void(Actor* actor)>;
		using VectorFunctionHandle_t = std::vector<std::function<void()>>;
		using TouchHandle_t = std::vector<std::function<void(int32_t, int32_t)>>;
	public:
		AMP_Engine(ANativeActivity* activity){
			SetAssetManager(activity->assetManager);
			SetScreenDPI(activity);
			// TO DO: set internal data path in File Manager, read/write save files
		}
	    ~AMP_Engine();
		AMP_Engine(const AMP_Engine&) = delete;
		AMP_Engine(AMP_Engine&&) = delete;
		
		/// Sets the screen dpi of the current device in the engine
		/// 
		/// @param activity This parameter comes from the native android and contains device specific and android specific information
		void SetScreenDPI(ANativeActivity* activity);


		/// AAssetManager* is used to load asset from the device storage in main memory
		void SetAssetManager(AAssetManager* assetManager);

		/// Starts game Loop
		/// 
		/// This function starts all Subsystems and inits your game 
		/// It also performs a shutdown before the function returns
		void run(std::function<bool(AndroidStatus*)> AndroidStatusInput);

		/// Inits all Systems in case its necessary
		/// 
		/// When the game starts the first time or the application runs for a longer time in the background  an init call is necessary
		/// @note When CommandsAndroid is APP_CMD_STOP or you start the engine for the first time, systems will be initialized
		/// @note In all other cases no init step is performed, the system status should be all other cases up to date
		/// @param status current status from the native application @see Android::NativeApplicationHandler
		void StartSystemsWhenNecessary(AndroidStatus& status);

		/// Shutdown of all systems
		/// 
		/// When the android native app CommandsAndroid changes to APP_CMD_STOP, the app loses its window and draw calls will result in an error 
		/// So the engine is shutting down all systems, but keeps alive its loop. In case the app gets it window back
		/// @param status current status from the native application @see Android::NativeApplicationHandler
		void ProcessShutdown(AndroidStatus& status);

		/// FrameTime also called DeltaTime
		///
		/// @return the frametime in seconds of the last frame
		/// @note returns 1 when frame time is > 1 (e.g: new Scene is loaded)
		double getFrameTime();

		/// CPU time or Calculation Time
		/// 
		/// @return return the last frame time without the egl swap buffer time 
		/// @note returns 1 when frame time is > 1 (e.g: new Scene is loaded)
		double getCalculationTime();

		/// Subscription of a time event or also called delay
		/// 
		/// @param timeEvent A time event containing all neccessary information
		void addTimerFunction(TimeEvent timeEvent);

		/// Erases certain TimeEvent
		/// 
		/// @param timeEvent This must be the same TimeEvent you passed to @see addTimerFunction()
		/// @note Is mostly used to unloop timer functions
		/// @attention all TimeEvents become cleared when unloading a scene @see Scene::unloadScene()
		void removeTimerFunction(TimeEvent& timeEvent);

		/// Finds a Scene Actor by index
		/// 
		/// @param index returning the Actor at this index
		/// Time: O(1)
		Actor* getSceneActor(int index);

		/// Finds and returns Scene Actor by name
		/// 
		/// @param name Throws error when name does not exist
		/// Time: O(n)
		Actor* getSceneActorByName(TEXT name);

		/// Calculates the number of Scene Actors
		///
		/// @return number of SceneActors
		int getSceneCount();

		/// Finds and returns a UI Actor by index
		/// 
		/// @param index Is returning the Actor at this index
		/// Time: O(1)
		Actor* getUIActor(int index);

		/// Finds UI Actor by name
		/// 
		/// @param name Throws error when name does not exist
		/// @return The UI actor with this name or throws an error (not existing)
		/// Time: O(n)
		Actor* getUIActorByName(TEXT name);

		/// Calculates the number of UI Actors
		///
		/// @return number of UIActors
		int getUICount();

		/// Contains all important display context information
		Context ampContext;
		/// Loads your Shader Programs
		Shader ampShader;
		/// For loading assets
		AAssetManager* assetManager = nullptr;
		/// Loader for OBJ, FILE and Textures
		GeneralAssetLoader assetLoader;
		/// Loader for Animations
		AnimLoaderFromGLTF animLoader;
		/// All events are subscribed and published here
		EventSystem events;
		/// Frame Buffer mainly used for post processing
		FrameBuffer frameBuffer;
		/// Wraps physics from reactphysics 3D
		PhysicsWrapper physics;
		/// All sounds are loaded and played from here
		SoundPool sound;

		/// @name Default Engine Game Events
		/// @brief All of these events can be subscribed by @see Eventsystem::subscribeEvent(TEXT)
		/// @{
		/// Function Signature: **myfunc()**
		TEXT postSceneUpdate = TEXT("PostSceneUpdate");
		/// Function Signature: **myfunc(int32_t x, int32_t y)**
		TEXT touchUpdate	 = TEXT("touchUpdate");
		/// Function Signature: **myfunc(int32_t x, int32_t y)**
		TEXT startTouch		 = TEXT("startTouch");
		/// Function Signature: **myfunc(int32_t x, int32_t y)**
		TEXT endTouch		 = TEXT("endTouch");
		/// Function Signature: **myfunc()**
		TEXT postInit		 = TEXT("postInit");
		/// Function Signature: **myfunc()**
		TEXT postPostInit	 = TEXT("postPostInit");
		/// Function Signature: **myfunc(std::pair<rp3d::CollisionBody*, rp3d::CollisionBody*> bodies, std::pair<glm::vec3, glm::vec3> contactPoints, glm::vec3 normal)**
		TEXT onCollission	 = TEXT("onCollission");
		/// Function Signature: **myfunc()**
		TEXT beforeShutdown  = TEXT("beforeShutdown");
		/// @}
	protected:
		///Right after MainLoop starts this function is called
		///
		///@attention Do not load or set up any graphics, shader or other open gl es specific content here
		///@attention Take instead @see AfterWindowStartup()
		virtual void AfterSystemStartup() {};

		/// Post display init
		/// 
		/// @note here you can call any open gl specific code
		virtual void AfterWindowStartup() {};

		/// GameUpdate
		/// 
		/// Called each frame
		virtual void update() {};
		
		/// On display is touched event
		///
		/// @note Root of the coordinate system is top left
		/// @note Unit is pixels
		/// @see ampContext for screensize X in pixels and screensize Y in pixels
		virtual void OnTouch(int32_t x, int32_t y) {};

		/// On system before shutdown
		/// 
		/// Use cases: Free allocated memory, stop subsystems, ...
		virtual void BeforeSystemShutdown() {};

	private:
		void SystemStartupCallback();
		void AfterWindowStartupCallback();
		void UpdateCallback();
		void PostSceneUpdateCallback();
		void publishUIActorComponentsUpdate();
		void publishSceneActorComponentsUpdate();
		void OnTouchCallback(int32_t x, int32_t y);
		//Also called when scene is unloaded
		void BeforeSystemShutdownCallback();
		void publishActorEvent(FuncHandle_t function);
		void publishUIEvent(FuncHandle_t function);
		void PrepareShutdown();
		void InitSystems();
		void PublishTouchEvents(AndroidStatus& status);
		void ProcessTouchEvents(AndroidStatus& status);
		void StartTouch(int32_t x, int32_t y);
		void EndTouch(int32_t x, int32_t y);
		void GameUpdate();
		void Shutdown();

		CurrentSceneContainer currentScene;

		double calculationTime = 0.0;
		double frameTime = 0.0;
		int64_t startTime = 0;
		bool isRuntime = false;
		bool hasDisplay = false;
		int32_t TouchX;
		int32_t TouchY;
		bool animating = true;
		bool isFullStartUpNecessary = true;

		friend class Scene;
		friend class Mesh;
		friend class TextRenderer;
		friend class DynamicMesh;
		friend class DynamicConcaveMesh;
		friend class CubeMap;
		friend class Animation;
		friend class Image;
		friend class ProgressBar;
		friend class SoundPool;
	};
}
