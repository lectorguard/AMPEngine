#include "pch.h"
#include "Defender_Streamed.h"

void Defender_Streamed::OnActivate()
{
	//Engine Events
	SUB(engine->postInit, Defender_Streamed::OnPostInit);

	//Game Events
	SUB(startStreamDefend, Defender_Streamed::StartDefending);
	SUB(resetStreamDefend, Defender_Streamed::ResetDefendingStream);

	//CharacterTouch
	SUB(TEXT("onDirectionCalculated"), Defender_Streamed::OnDirCalculated);
	SUB(TEXT("endTouchGest"), Defender_Streamed::OnEndTouch);

	//StateEvents
	SUB(TEXT("WaitingForHit"), Defender_Streamed::StartCharacterTouch);
	SUB(TEXT("RunToBottle"), Defender_Streamed::OnRunToBottle);
	SUB(TEXT("GoBackToStart"), Defender_Streamed::OnGoBackToStart);
}

void Defender_Streamed::OnDeactivate()
{
	UnsubscribeEvents();
}

void Defender_Streamed::AfterWindowStartup()
{
}

void Defender_Streamed::OnPostInit()
{
	auto* pickUpActor = (PickUpActor*)engine->getSceneActorByName(TEXT("PickUpActor"));
	this->pickUpActor = pickUpActor;
	this->character = pickUpActor->collider;
	this->ground = (Ground_Client*)engine->getUIActorByName(TEXT("GroundActor_Client"));
}

void Defender_Streamed::StartDefending()
{
	InitCamera();
	inputInverted = true;
}

void Defender_Streamed::OnRunToBottle()
{
	ground->SetActive(true);
}

void Defender_Streamed::InitCamera()
{
	amp::Camera::resetView();
	amp::Camera::setViewLocation(glm::vec3(-47, 40, 0));
	amp::Camera::setViewRotation(glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));
	amp::Camera::rotateView(-25.0f, glm::vec3(0, 0, 1));
	engine->events.publishEvent(TEXT("startFollowingCamera"), amp::Camera::getWorldView(), std::make_optional<float>(40.0f));
}


void Defender_Streamed::StartCharacterTouch()
{
	engine->events.publishEvent(TEXT("StartCheckingCharacterTouch"));
}

//EVENT
void Defender_Streamed::OnDirCalculated(Move m)
{
	//Invert right left
	if (m == Move::Left && inputInverted)m = Move::Right;
	else if (m == Move::Right && inputInverted)m = Move::Left;
	//call function at SendInput
	engine->events.publishEvent(TEXT("acceptNewDefendMove"), m);
}

//EVENT
void Defender_Streamed::OnEndTouch()
{
	StartCharacterTouch();
}

void Defender_Streamed::ResetDefendingStream()
{
	ground->SetActive(false);
	pickUpActor->controller->ResetAllActiveTasks();
	pickUpActor->Reset();
	currentStage = DefendingStages::NotActive;
	inputInverted = true;
}

void Defender_Streamed::OnGoBackToStart()
{
	inputInverted = false;
}

Defender_Streamed::~Defender_Streamed()
{
}