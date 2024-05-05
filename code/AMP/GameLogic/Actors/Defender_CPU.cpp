#include "pch.h"
#include "Defender_CPU.h"

void Defender_CPU::OnActivate()
{
	//Engine Events
	SUB(engine->postInit, Defender_CPU::OnPostInit);

	//Game Events
	SUB(startDefendingCPU, Defender_CPU::StartDefendingCPU);
	SUB(resetDefendingCPU, Defender_CPU::ResetDefendingCPU);

	//BottleHitEvent
	SUB(TEXT("onBottleHit"), Defender_CPU::OnBottleHit);

	//Mesh Spectator
	SUB(TEXT("onMeshesClose"), Defender_CPU::OnMeshesClose);
	SUB(TEXT("onBottleLoc"), Defender_CPU::OnBottleLocationUpdate);

	//rotateTask
	SUB(TEXT("onRotateFinished"), Defender_CPU::OnRotateFinished);

	//collision
	SUB(TEXT("onCollisionObject"), Defender_CPU::OnCollisionBody);

	//Character Control
	SUB(TEXT("onCharacterRelocate"), Defender_CPU::OnRelocate);

}

void Defender_CPU::OnDeactivate()
{
	UnsubscribeEvents();
}

void Defender_CPU::AfterWindowStartup()
{
}

void Defender_CPU::StartDefendingCPU()
{
	ResetLocalVariables();
	engine->events.publishEvent(TEXT("startRelocateWhenFalling"), -40.0f);
}

void Defender_CPU::OnPostInit()
{
	auto* pickUpActor = (PickUpActor*)engine->getSceneActorByName(TEXT("PickUpActor"));
	this->pickUpActor = pickUpActor;
	this->character = pickUpActor->collider;
	this->characterAnim = pickUpActor->anim;
}

//StartDefending
void Defender_CPU::OnBottleHit()
{
	currentStage = RunToBottle;
	engine->events.publishEvent(TEXT("addSpectatingMesh"), character, false);
	engine->events.publishEvent(TEXT("startCollisionCheck"));
	characterAnim->SetAnimation(1);
}


void Defender_CPU::Update()
{
	switch (currentStage)
	{
	case RunToBottle:
		MoveCharacter(forwardForce);
		SetForwardVectorToBottle();
		break;
	case GoBackToStart:
		character->setRotation(glm::quat(glm::vec3(0,glm::radians(180.0f),0)));
		MoveCharacter(backwardForce);
		CheckReachedEndLine();
		break;
	default:
		break;
	}
}

void Defender_CPU::SetForwardVectorToBottle()
{
	auto directionNorm = glm::normalize(forwardForce);
	auto yRot = glm::dot(directionNorm, glm::vec3(1, 0, 0));
	float sign = 1.0f;
	if (directionNorm.z > 0)sign = -1.0f;
	glm::vec3 rot = glm::vec3(0, glm::acos(yRot) * sign, 0);
	character->setRotation(rot);
}

void Defender_CPU::CheckReachedEndLine()
{
	if (character->getLocation().x < pickUpActor->startLocation.x) {
		character->ResetLinearVelocity();
		currentStage = WaitingForHit;
		engine->events.publishEvent(TEXT("startRotate"), glm::vec3(0,0,0));
	}
}

void Defender_CPU::OnMeshesClose()
{
	//Reset events of old stage
	engine->events.publishEvent(TEXT("BottleSpectatorReset"));
	currentStage = PickUpBottle;
	engine->events.publishEvent(TEXT("startRotate"),glm::vec3(0, glm::radians(180.0f),0));
}

void Defender_CPU::OnRotateFinished()
{
	switch (currentStage)
	{
	case PickUpBottle:
		//Reset old tasks
		engine->events.publishEvent(TEXT("RotatingTaskReset"));
		currentStage = GoBackToStart;
		//For safety to clear forward values
		engine->events.publishEvent(TEXT("updateFallbackCheck"), false, 50.0f);
		break;
	case WaitingForHit:
		characterAnim->SetAnimation(0);
		engine->events.publishEvent(endDefendingCPU);
		break;
	default:
		break;
	}	
}



void Defender_CPU::OnBottleLocationUpdate(glm::vec3 loc)
{
	loc.y = 0;
	auto charLoc = character->getLocation();
	charLoc.y = 0;
	forwardForce = glm::normalize(loc - character->getLocation()) * 15.0f;
}

void Defender_CPU::OnCollisionBody()
{
	switch (currentStage)
	{
	case RunToBottle:
		character->applyForceToCenter(glm::vec3(-100000, 0, 0));
		engine->events.publishEvent(TEXT("updateFallbackCheck"), true, character->getLocation().x - X_Fallback);
		break;
	case GoBackToStart:
		character->applyForceToCenter(glm::vec3(100000, 0, 0));
		engine->events.publishEvent(TEXT("updateFallbackCheck"), false, character->getLocation().x + X_Fallback);
		break;
	default:
		break;
	}
}

void Defender_CPU::OnRelocate()
{
	engine->events.publishEvent(TEXT("updateFallbackCheck"), true, pickUpActor->startLocation.x - X_Fallback);
	character->setLocation(pickUpActor->startLocation);
	character->setRotation(glm::quat(1, 0, 0, 0));
}

void Defender_CPU::MoveCharacter(glm::vec3 dirForce)
{
	auto force = character->getLinearVelocity();
	if (glm::length(force) <= glm::length(dirForce))character->applyForceToCenter(dirForce * 200.0f);
	if (glm::dot(glm::normalize(force), glm::normalize(dirForce)) <= 0)character->applyForceToCenter(dirForce * 200.0f);
}

void Defender_CPU::ResetDefendingCPU()
{
	//Reset old tasks
	pickUpActor->controller->ResetAllActiveTasks();
	ResetLocalVariables();
	pickUpActor->Reset();
	characterAnim->SetAnimation(0);
}

void Defender_CPU::ResetLocalVariables()
{
	currentStage = WaitingForHit;
	forwardForce = glm::vec3(15, 0, 0);
	backwardForce = glm::vec3(-15, 0, 0);
}
