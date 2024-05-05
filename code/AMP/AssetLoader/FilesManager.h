#pragma once
#include "android/asset_manager.h"
#include "stdlib.h"
#include "string"
#include "sstream"
#include "istream"


namespace amp
{
	/// This is the amp file manager 
	/// 
	/// The manager is used to load general files and some specific file types
	class FilesManager
	{

	public:
		/// Reads an asset file as c string
		///
		/// @param manager This is the manager you receive from the android native application. The amp engine stores the this pointer after being initialized
		/// @param path This is the relative path to your asset starting after the asset folder
		/// @param mode This is the type how the asset will be read
		/// @return The content of the asset as a c string, make sure you **free** the pointer after usage, otherwise memory leak
		static char* readAssetFile(AAssetManager* manager,const char* path, int mode = AASSET_MODE_BUFFER);
		
		/// Reads a shader file 
		///
		/// @param manager This is the manager you receive from the android native application. The amp engine stores the this pointer after being initialized
		/// @param path This is the relative path to your shader starting after the asset folder
		/// @param vertexFile The vertex shader code becomes set here
		/// @param fragmentFile The fragment shader code becomes set here
		static void readShaderFile(AAssetManager* manager,const char* path,std::string& vertexFile, std::string& fragmentFile);

		/// Reads an image file 
		///
		/// @param manager This is the manager you receive from the android native application. The amp engine stores the this pointer after being initialized
		/// @param path This is the relative path to your shader starting after the asset folder
		/// @return The image as file buffer
		static FILE* readImageFile(AAssetManager* manager, const char* path);
	private:
		static int android_read(void* cookie, char* buf, int size) {
			return AAsset_read((AAsset*)cookie, buf, size);
		}

		static int android_write(void* cookie, const char* buf, int size) {
			return EACCES; // can't provide write access to the apk
		}

		static fpos_t android_seek(void* cookie, fpos_t offset, int whence) {
			return AAsset_seek((AAsset*)cookie, offset, whence);
		}

		static int android_close(void* cookie) {
			AAsset_close((AAsset*)cookie);
			return 0;
		}
	};
}

