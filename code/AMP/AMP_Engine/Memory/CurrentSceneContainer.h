#pragma once

#include "memory"
#include "AMP_Memory.h"
#include "AMP_HashMap.h"
#include "../AssetLoader/Attributes.h"
#include "Actor.h"


namespace amp{
	/// This is the AMP scene container 
	/// 
	/// This container stores all actors created in a scene
	/// No actor can be deleted in an active scene, but you can set actors not active
	/// @see Actor
	struct CurrentSceneContainer
	{
		using ActorContainer_t = LinearArray<std::unique_ptr<Actor>, 128>;
		using UIContainer_t = LinearArray<std::unique_ptr<Actor>, 32>;

		/// Clears the scene container, deletes all actors in the scene
		/// 
		/// This function is called from the scene, when a scene becomes unloaded
		/// @param IsCleanUp If true all assets from the AssetAttribMap becomes deleted
		/// @param IsCleanUp If false, just assets become deleted which were used at runtime
		void clear(bool IsCleanUp) {
			CurrentSceneActorArray.clear();
			CurrentSceneUIArray.clear();
			IsActiveScene = false;
			if (IsCleanUp)removeRuntimeMapElements();
			else clearMap();
		}

		/// Clears the AssetAttribMap (deletes each element)
		void clearMap(){
			for (std::pair<size_t, Attributes*> element : AssetAttribMap) {
				delete element.second;
				element.second = nullptr;
			}
			AssetAttribMap.clear();
		}

		/// Removes all assets from the AssetAttribMap, which are used at runtime
		void removeRuntimeMapElements() {
			cleanUpMapOnRuntimeUsage(false);
		}

		/// Removes all assets from the AssetAttribMap, which are **not** used at runtime
		void removeUnusedRuntimeMapElements() {
			cleanUpMapOnRuntimeUsage(true);
		}

		/// This map stores and manages all assets, which become loaded the amp environment
		HashMap<size_t, Attributes*> AssetAttribMap;
	private:
		void cleanUpMapOnRuntimeUsage(bool ConditionInverted) {
			auto iter = AssetAttribMap.begin();
			auto endIter = AssetAttribMap.end();
			for (; iter != endIter;) {
				if (ConditionInverted) {
					if (!iter->second->isUsedAtRuntime) {
						delete iter->second;
						iter->second = nullptr;
						AssetAttribMap.erase(iter++);
					}
					else ++iter;
				}
				else
				{
					if (iter->second->isUsedAtRuntime) {
						delete iter->second;
						iter->second = nullptr;
						AssetAttribMap.erase(iter++);
					} 
					else ++iter;
				}
			}
		}

		ActorContainer_t CurrentSceneActorArray;
		UIContainer_t CurrentSceneUIArray;
		bool IsActiveScene = false;
		friend class AMP_Engine;
		friend class Scene;
	};
}
