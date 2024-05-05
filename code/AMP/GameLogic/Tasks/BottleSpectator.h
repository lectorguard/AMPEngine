#pragma once
#include "TaskController.h"
#include "AMP_Engine.h"
#include "Components/DynamicMesh.h"
#include "Static/TimeManager.h"

class BottleSpectator : public amp::Task
{
	public:
	amp::DynamicMesh* meshes[2] = {nullptr,nullptr};
	BottleSpectator() {
	};
	virtual ~BottleSpectator() {};

	BottleSpectator(const BottleSpectator&) = delete;
	BottleSpectator(BottleSpectator&&) = delete;

	const float maxDistance = 24.0f;
	const float maxDistanceBehindBottle = 30.0f;

	//EVENTS
	//INPUTS
	TEXT BottleSpectatorReset = TEXT("BottleSpectatorReset");		//No Params: RESET has always 0 params
	TEXT addSpectatingMesh = TEXT("addSpectatingMesh");				//One Param : amp::DynamicMesh* mesh, bool isBottle
	TEXT stopBottleSpectator = TEXT("stopBottleSpectator");			//No Params
	//OUTPUT
	TEXT onMeshesClose = TEXT("onMeshesClose");						//No Params
	TEXT onBottleLoc = TEXT("onBottleLoc");							//One Param: glm::vec3 location
	//Called multiple times when character or bottle is not reset
	TEXT onCharacterBehineBottle = TEXT("onCharacterBehineBottle"); //No Params

	virtual void Init() override;

	void AddMeshSpectator(amp::DynamicMesh* mesh, bool isBottle);

	virtual void Update() override;

	virtual void End() override;


	virtual void Reset() override;

private:
	int meshCounter = 0;
	int bottleIndex = 0;
	int characterIndex = 0;
	
};


