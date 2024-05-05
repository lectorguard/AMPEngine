#pragma once
#include "TaskController.h"
#include "AMP_Engine.h"
#include "Components/Mesh.h"
#include "Components/DynamicMesh.h"

class SpotlightTargetBottle : public amp::Task
{
public:
	amp::DynamicMesh * bottle = nullptr;
	amp::Mesh* spotlight = nullptr;
	SpotlightTargetBottle(amp::DynamicMesh* targetBottle, amp::Mesh* mesh) {
		this->bottle = targetBottle;
		this->spotlight = mesh;
	};
	virtual ~SpotlightTargetBottle() {};

	SpotlightTargetBottle(const SpotlightTargetBottle&) = delete;
	SpotlightTargetBottle(SpotlightTargetBottle&&) = delete;

	const glm::vec3 hidingLocation = glm::vec3(0, -1000000000, 0);
	const float height = 45.0f;


	//EVENTS
	//INPUTS
	TEXT SpotlightTargetBottleReset = TEXT("SpotlightTargetBottleReset");		//No Params: RESET has always 0 params
	TEXT startSpotlightTargetBottle = TEXT("startSpotlightTargetBottle");   //One Param: amp::DynamicMesh* character
	TEXT endSpotlightTargetBottle = TEXT("endSpotlightTargetBottle");		  //No Param


	virtual void Init() override;

	void StartSpot();

	virtual void Update() override;

	virtual void End() override;

	virtual void Reset() override;
};
