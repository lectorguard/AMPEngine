@page QuickSetup Short Project Setup
@tableofcontents

@section GameCreation1 How to create a new Game

There are two ways, described in the following to set up your game. 
The first one overrides the current game project, the second one creates a new game.

@subsection GameCreationSubsection1 Replace current project with your project

* Go inside your project in Visual Studio 19
* Delete all folders inside the GameLogic Project 
* Create a new folder with the name of your game 
* Create a header and a cpp class with the name of your game
* That's it

@subsection GameCreationSubsection2 Create a new android static library as your game

This tutorial can also be used to add libraries to the engine.

* Go inside your project in Visual Studio 19
* Add a new project to your solution
@image html CreateNewProject.PNG width=600 height=500
* Create an android static library 
@image html android_static_library.PNG 
* After you have created the library some properties of your project must be changed
* Go into the properties folder of your newly created project
@image html properties.PNG 
* Set the target API level 24 (Android 7.0)
* Make sure you have selected all configurations and all target platforms
@image html TargetAPIPlatform.PNG 
* Under C++ -> All Options you need to make also some changes
    * Add the following directories to the additional include directories : \verbatim $(ProjectDir);$(SolutionDir)\AMP_Engine; \endverbatim
    * Set the c++ language to c++ 17
    * Allow Exceptions
    * Enable runtime type information
@image html cppOptions.PNG 
* Now last but not least you have to set the references to the engine and the asset loader
* Make sure icon for "Show all files" is not clicked, this should allow you to see the project references
@image html showAllFiles.PNG width=600 height=500
* Go to your game project references and add the references AMP_Engine and AssetLoader
@image html addReferenceMenu.PNG width=600 height=500
@image html references.PNG 
* Add also the reference of your game project to the GameNativeActivity application project
* That's it

@section GameCreation2 Your Game class

This section shows you how an example game class can look like and how you start your game.

@subsection GameCreationSub1 Setup your class

The following header and cpp file are showing an example game class we will discuss here.

@subsubsection Header1 Header file

@image html gameHeader.PNG

You create a game by inheriting from the amp::AMP_Engine. So you must include the engine and call the constructor of the engine as you can see it in the header file. The constructor is also 
used to create your scenes. All scene objects (this does not load your scene) should be created inside the constructor of your game. This can easily be done by using unique_ptrs.
Additional information can be found inside the amp::AMP_Engine class.

You have also automatically subscribed some events like amp::AMP_Engine::AfterWindowStartup(). You will find this event all over the engine It should be used for everything you want to set up, when your game(or scene) starts.

The amp::AMP_Engine::update() method is the callback you receive every frame. This is very similar to other game engines.

The amp::AMP_Engine::BeforeSystemShutdown() callback is called, before the engine performs a shutdown. This function should be used to free allocated memory.

@subsubsection CPP CPP file

@image html gamecpp.PNG

The only function we use in this very simple example is amp::AMP_Engine::AfterWindowStartup(). 
The main thing we do here is to load our default scene, in our case the menu scene. Other actions we should do here are setting a custom frame buffer shader, setting the projection matrix and setting the gravity vector of the physics system. What you can also do when having multiple scenes are level transition events. There you unload the current scene and start a new one. The amp::EventSystem is also well documented.

@subsection GameCreationSub2 The main function

@image html main.PNG

The android_main() can be found inside the Android/GameNativeActivity project. The first step here is to create a stack instance of your game, therefore you need to call the constructor with the ANativeActivity pointer. You get this pointer from the native android application. The next step is to set up the Android::AndroidApplicationHandler, the handler takes care of android native events, handling touch input events and much more. Have a look at Android::AndroidApplicationHandler for further information. This class takes the android native application state as parameter. You get the current state from the main function. 

After you have set up everything, you can call the run function of your game. Inside this function the main loop is executed, so in order to get updates from the android native application, we have to pass the update function pointer from the Android::AndroidApplicationHandler. That's it. You will find many more explanations inside the classes of the engine like amp::Actor or amp::Component. A good starting point to understand the engine is the amp::Scene or of course the amp::AMP_Engine itself. 