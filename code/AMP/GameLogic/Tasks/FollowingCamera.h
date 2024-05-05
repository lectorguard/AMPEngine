#pragma once
#include "TaskController.h"
#include "AMP_Engine.h"
#include "Components/Component.h"
#include "../glm/gtx/matrix_decompose.hpp"
#include "Static/Camera.h"
#include "optional"

class FollowingCamera : public amp::Task
{

public:
	amp::Component* targetComponent = nullptr;
	FollowingCamera(amp::Component* comp) {
		targetComponent = comp;
	};
	virtual ~FollowingCamera() {};

	FollowingCamera(const FollowingCamera&) = delete;
	FollowingCamera(FollowingCamera&&) = delete;


	//EVENTS
	//INPUTS
	TEXT FollowingCameraReset = TEXT("FollowingCameraReset");   //No Params: RESET has always 0 params
	TEXT startFollowingCamera = TEXT("startFollowingCamera");   //Two Params: glm::mat4 deltaCameraTransform, std::optional<float> fixY = std::nullopt 
	TEXT stopFollowingCamera = TEXT("stopFollowingCamera");		//No Param

	virtual void Init() override;

	void StartFollowingCamera(glm::mat4 deltaTransform,std::optional<float> fixY = std::nullopt);

	virtual void Update() override;

	virtual void End() override;


	virtual void Reset() override;

private:
	glm::mat4 originCameraTransform;
	std::optional<float> yValue;
};

