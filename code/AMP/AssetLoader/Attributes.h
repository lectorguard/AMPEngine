#pragma once
#include "cstdlib"

namespace amp
{
	
	/// This struct is used for the amp AssetAttribMap
	///
	/// All assets stored in a structs/classes, which inherit from Attributes can be managed inside the assetAttribMap
	/// You can find example code inside Mesh::LoadTexture() on how the use the assetAttribMap
	/// @note You can create your own asset struct, which is managed by the amp engine
	/// @note Therefore create struct which inherits from this struct e.g: CustomAttributes : public amp::Attributes
	/// ### Example for CustomAttributes struct using the amp assetAttribMap
	/// ```cpp
	/// CustomComponent::load(TEXT pathToYourAwesomeAsset){
	///		CustomAttributes* customAttr = nullptr;
	///		//The asset is already inside the asset map and can be directly returned
	///		if (auto result = engine->currentScene.AssetAttribMap.tryFind(pathToYourAwesomeAsset.getHash())) {
	///			customAttr = (CustomAttributes*)*result;
	///		}
	///		else {
	///			// The asset does not yet exist and must be loaded and added to the AssetAttribMap
	///			// Load your asset on your own and store it inside the CustomAttributes object, make sure to create the pointer with **new**
	///			// You can use MeshLoaderFromObj::getRawFile(const char* path) to load any file inside yourAwesomLoaderFunction 
	///			texAttr = yourAwesomLoaderFunction(pathToYourAwesomeAsset.getCharArray());
	///			engine->currentScene.AssetAttribMap.add(pathToYourAwesomeAsset.getHash(), customAttr);
	///		}
	///		// When using the Attributes default constructor the asset will be deleted after the postpostInit event
	/// }
	/// ```
	struct Attributes
	{
		/// Constructor
		///
		/// If true asset becomes deleted at the end of the scene (when scene is unloaded)
		/// If false asset becomes deleted after scene is initialized after postpostInit event from the engine
		/// Order: AfterWindowStartup -> postInit event -> postpostInit event
		Attributes(bool isUsedAtRuntime = false):isUsedAtRuntime(isUsedAtRuntime) {};
		virtual ~Attributes() {};
	
		Attributes(Attributes&&) = delete;
		Attributes(const Attributes&) = delete;
	
		bool isUsedAtRuntime = false;
	};
}

/// This is an asset struct containing all information to play sound
///
/// @see Attributes for additional information
struct SoundAttributes : public amp::Attributes{
	// This asset is used at runtime
	SoundAttributes(){
		isUsedAtRuntime = true;
	};
	virtual ~SoundAttributes() {
		free(buf);
	};
	char* buf = nullptr;
	int size = -1;
	int index = -1; 
	int durationInMS = -1;
};