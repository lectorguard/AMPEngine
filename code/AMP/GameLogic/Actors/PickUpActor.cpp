#include "pch.h"
#include "PickUpActor.h"
#include "../glm/gtx/matrix_decompose.hpp"
#include "Components/DynamicConcaveMesh.h"
#include "cmath"
#include "Event/EventSystem.h"
#include "Tasks/RotatingTask.h"
#include "Tasks/Running.h"
#include "Tasks/CharacterControl.h"
#include "Tasks/CharacterFallback.h"
#include "Tasks/CollissionCheck.h"
#include "Tasks/FollowingCamera.h"
#include "Sound/SoundPool.h"
#include "Tasks/ChangeLane.h"

PickUpActor::~PickUpActor()
{
	controller->clear();
}

void PickUpActor::AfterWindowStartup()
{

	anim->SetUp(TEXT("Animation/AnimatedDefender.glb"), TEXT("Textures/Player/skin_0.png"));
	auto translation = glm::translate(glm::mat4(1.0f), startLocation);
	anim->setTransform(translation);
	anim->scale(glm::vec3(6,6,6));
	anim->SetDefaultRotation(glm::rotate(glm::mat4(1.0f), glm::radians(83.0f), glm::vec3(0, 1, 0)));

	//When weight is too small (ca: 2kg), the body is moving randomly upwards with no reason, (maybe wrong collission resolution)
 	collider->SetUpBox(anim, amp::PhysicType::DynamicBody, glm::vec3(2, 6, 2), 20.0f,glm::vec3(0,6,0));
	CurrenFallback_X = collider->getLocation().x;
	gest->SetUp();
	
	controller->AddTask<CharacterTouch>(gest);
	controller->AddTask<ChangeLane>(collider);
	controller->AddTask<Running>(collider);
	controller->AddTask<CharacterControl>(collider);
	controller->AddTask<CharacterFallback>(collider);
	controller->AddTask<CollissionCheck>(collider);
	controller->AddTask<FollowingCamera>(anim);
	controller->AddTask<RotatingTask>(collider);

	int index = engine->sound.load(TEXT("Sound/S_Titlesound.wav"));
	engine->sound.play(index, 1.0f,true);
}

void PickUpActor::Update()
{
	controller->CallUpdateForTasks();
}

void PickUpActor::Reset()
{
	auto translation = glm::translate(glm::mat4(1.0f), startLocation);
	collider->setTransform(translation);
	CurrenFallback_X = startLocation.x;
	collider->ResetAngularVelocity();
	collider->ResetLinearVelocity();
}

