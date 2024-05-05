#pragma once
#include "Components/Mesh.h"
#include "Actor.h"
#include "Text.h"
#include "Static/Camera.h"
#include "AMP_Engine.h"
#include "Event/event.h"
#include "Components/DynamicMesh.h"
#include "Components/DynamicConcaveMesh.h"
#include "Components/TouchGesture.h"
#include "Components/Animation.h"
#include "TaskController.h"


class ThrowingActor : public amp::Actor
{

public:
	amp::Mesh* M_ThrowBottle = nullptr;
	amp::Mesh* M_TargetBottle = nullptr;
	amp::Mesh* M_TargetSpot = nullptr;
	amp::DynamicMesh* R_ThrowBottle = nullptr;
	amp::DynamicMesh* R_TargetBottle = nullptr;
	amp::TouchGesture* U_throw = nullptr;
	amp::TaskController* controller = nullptr;
	amp::Animation* attackerIdle = nullptr;

	std::vector<amp::Mesh*> M_AttackBottles;
	std::vector<amp::DynamicMesh*> R_AttackBottles;

	const int NumberOfAttackBottles = 5;

	ThrowingActor() {
 		R_ThrowBottle = AddComponent<amp::DynamicMesh>(TEXT("R_ThrowBottle"));
 		R_TargetBottle = AddComponent<amp::DynamicMesh>(TEXT("R_TargetBottle"));
		M_ThrowBottle = AddComponent<amp::Mesh>(TEXT("M_ThrowBottle"));
		M_TargetBottle = AddComponent<amp::Mesh>(TEXT("M_TargetBottle"));
		U_throw = AddComponent<amp::TouchGesture>(TEXT("U_throw"));
		M_TargetSpot = AddComponent<amp::Mesh>(TEXT("TargetSpotLight"));
		attackerIdle = AddComponent<amp::Animation>(TEXT("Attacker_Idle"));

		std::string M_BottleName = "M_AttackMesh_";
		std::string R_BottleName = "R_AttackBottle";
		for (int i = 0; i < NumberOfAttackBottles; ++i)
		{
			R_AttackBottles.push_back(AddComponent<amp::DynamicMesh>(TEXT(R_BottleName + std::to_string(i))));
			M_AttackBottles.push_back(AddComponent<amp::Mesh>(TEXT(M_BottleName + std::to_string(i))));
		}
		controller = CreateTaskController();
	};
	virtual ~ThrowingActor();

	void Update() override;

	void AfterWindowStartup() override;

	void SetStartingVariables();

	void ResetBottles();

	void ResetTargetBottle();

	void ResetThrowBottle(amp::DynamicMesh* bottle);

	void Reset();

	void HideAttackBottles();

	void ThrowBottle(amp::DynamicMesh* bottle, glm::vec3 force);
	


	glm::mat4 originTargetBottle;
	glm::mat4 originThrowBottle;
	glm::vec3 TargetThrowVec;

	//glm::vec3 StartThrowLocation = glm::vec3(207, 16, 0);
	glm::vec3 AnimIdleLocation = glm::vec3(190.59f, 0.0f, 0.0f);
	glm::vec3 StartThrowLocation = glm::vec3(120, 16, 0);
	glm::quat StartThrowRotation = glm::quat(0.08,-0.13,0.98,0.01); 
};

