#include "pch.h"
#include "ThrowingActor.h"
#include "algorithm"
#include "Tasks/ThrowingBottle.h"
#include "Tasks/BottleHitCheck.h"
#include "Tasks/BottleSpectator.h"
#include "Tasks/MoveBottleToHead.h"
#include "Tasks/SpotlightTargetBottle.h"
#include "Tasks/BottleValidityCheck.h"


void ThrowingActor::AfterWindowStartup()
{
	attackerIdle->SetUp(TEXT("Animation/AnimatedAttacker.glb"), TEXT("Textures/Player/skin_1.png"));
	auto translation = glm::translate(glm::mat4(1.0f), AnimIdleLocation);
	attackerIdle->setTransform(translation); 
	attackerIdle->scale(glm::vec3(6, 6, 6));
	attackerIdle->SetDefaultRotation(glm::rotate(glm::mat4(1.0f), glm::radians(-97.0f), glm::vec3(0, 1, 0)));

	M_TargetSpot->Mesh_SetUpGL(TEXT("3DModels/beer_cloud.obj"), TEXT("Textures/DrunkenCloud.png"));
	M_TargetBottle->Mesh_SetUpGL(TEXT("3DModels/CoronaBottleLarge.obj"), TEXT("Textures/CoronaBottle.jpg"));
	M_TargetBottle->translate(0, 16, 0);
	M_ThrowBottle->Mesh_SetUpGL(TEXT("3DModels/CoronaBottle.obj"), TEXT("Textures/CoronaBottle.jpg"));

	M_ThrowBottle->translate(StartThrowLocation);
	M_ThrowBottle->rotateQuat(StartThrowRotation);

	R_ThrowBottle->SetUpCapsule(M_ThrowBottle, amp::PhysicType::DynamicBody, 1.4f, 2, 1.0f);
	R_ThrowBottle->SetCollisionFilter(amp::Filter::F_1, amp::Filter::F_0);
	R_TargetBottle->SetUpCapsule(M_TargetBottle, amp::PhysicType::DynamicBody, 2.8f, 6, 2.0f);
	R_ThrowBottle->setActive(false);

	for (int i = 0; i < NumberOfAttackBottles; ++i) {
		M_AttackBottles[i]->Mesh_SetUpGL(TEXT("3DModels/CoronaBottle.obj"), TEXT("Textures/CoronaBottle.jpg"));
		//Just that the bottles are not visible
		M_AttackBottles[i]->translate(glm::vec3(0, -5, 0));
		R_AttackBottles[i]->SetUpCapsule(M_AttackBottles[i], amp::PhysicType::DynamicBody, 1.4f, 2, 1.0f);
		R_AttackBottles[i]->SetCollisionFilter(amp::Filter::F_1, amp::Filter::F_0);
		R_AttackBottles[i]->setActive(false);
	}
	U_throw->SetUp();

	SetStartingVariables();

	controller->AddTask<BottleValidityCheck>();
	controller->AddTask<ThrowingBottle>(U_throw);
	controller->AddTask<BottleHitCheck>(R_TargetBottle);
	controller->AddTask<BottleSpectator>();
	controller->AddTask<MoveBottleToHead>(R_TargetBottle);
	controller->AddTask<SpotlightTargetBottle>(R_TargetBottle, M_TargetSpot);
}

void ThrowingActor::SetStartingVariables()
{
	originTargetBottle = M_TargetBottle->getTransform();
	originThrowBottle = glm::translate(glm::mat4(1.0f), StartThrowLocation)* glm::toMat4(StartThrowRotation);
	auto originThrowLocation = M_ThrowBottle->getLocation();
	auto originTargetLocation = M_TargetBottle->getLocation();
	TargetThrowVec = glm::normalize(originTargetLocation - originThrowLocation);
}

void ThrowingActor::Update()
{
	controller->CallUpdateForTasks();
}

void ThrowingActor::ResetBottles()
{
	ResetThrowBottle(R_ThrowBottle);
	ResetTargetBottle();
}

void ThrowingActor::ResetTargetBottle()
{
	R_TargetBottle->setTransform(originTargetBottle);
	R_TargetBottle->applyForceToCenter(glm::vec3(0, 10, 0));
}

void ThrowingActor::ResetThrowBottle(amp::DynamicMesh* bottle)
{
	assert(originThrowBottle[3][0] = 120.0f);
	bottle->setTransform(originThrowBottle);
	bottle->setActive(false);
}

void ThrowingActor::Reset()
{
	ResetBottles();
	HideAttackBottles();
}

void ThrowingActor::HideAttackBottles()
{
	for (auto& bottle : R_AttackBottles) {
		bottle->setActive(false);
		bottle->setLocation(glm::vec3(0, -5, 0));
	}
}

void ThrowingActor::ThrowBottle(amp::DynamicMesh* bottle, glm::vec3 force)
{
	assert(bottle);
	bottle->setActive(true);
	bottle->applyForceToCenter(force);
}

ThrowingActor::~ThrowingActor()
{
	controller->clear();
}