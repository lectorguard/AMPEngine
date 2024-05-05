#pragma once
#include "Actor.h"
#include "AMP_Engine.h"
#include "Components/Animation.h"
#include "Components/DynamicMesh.h"
#include "Components/TouchGesture.h"
#include "Components/Mesh.h"
#include "TaskController.h"
#include "utility"
#include "memory"
#include "Static/Camera.h"
#include "Tasks/CharacterTouch.h"


class ChangeLane;

struct G_LaneInfo {
	static constexpr int NumberOfLanesPlayer = 5;//9
	static constexpr int NumberOfLanesAssets = 5;//9
	static constexpr float startZlocation = 0.0f;
	static constexpr float laneDistancePlayer = 40.0f;//20
	static constexpr float laneDistanceAssets = 40.0f;//20
	static constexpr float changingForce = 19000.0f;
	static constexpr float changingForceUpwards = 6000.0f;
	static constexpr glm::vec3 location = glm::vec3(-150.0f,0.0f,0.0f);
};

class PickUpActor : public amp::Actor
{

public:
	PickUpActor(PickUpActor&&) = delete;
	PickUpActor(const PickUpActor&) = delete;
	~PickUpActor();
	PickUpActor() {
		collider = AddComponent<amp::DynamicMesh>(TEXT("Collider"));
		anim = AddComponent<amp::Animation>(TEXT("WalkingAnimation"));
		gest = AddComponent<amp::TouchGesture>(TEXT("PickUpTouch"));
		controller = CreateTaskController();
	};
	amp::Animation* anim = nullptr;
	amp::DynamicMesh* collider = nullptr;
	amp::TouchGesture* gest = nullptr;
	amp::TaskController* controller = nullptr;

	void AfterWindowStartup() override;

	void Update() override;

	void Reset(); 

private:
	glm::vec3 startLocation = glm::vec3(-190.59f, 15.0f, 0.0f);
	float CurrenFallback_X = 0.0f;
	friend class Defender_CPU;
	friend class Defender_Player;
};

