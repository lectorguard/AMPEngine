#include "pch.h"
#include "Defender_Network.h"



Defender_Network::~Defender_Network()
{
}

void Defender_Network::SubscribeGameEvents()
{
	SUB(startDefendNetwork, Defender_Network::StartDefending);
	SUB(resetDefendNetwork, Defender_Network::ResetDefendNetwork);
}

void Defender_Network::ResetLocalVariables()
{
	currentStage = DefendingStages::NotActive;
	engine->events.publishEvent(TEXT("updateDefendingStage"), DefendingStages::NotActive);
	lastLaneLocation_Z = 0.0f;
	touchEnded = true;
	laneChangeEnded = true;
	canJump = true;
}

void Defender_Network::StartDefending()
{
	currentStage = WaitingForHit;
	//Automatically starts input Check on client 
	engine->events.publishEvent(TEXT("updateDefendingStage"), DefendingStages::WaitingForHit);
	StartRespawnAndOrientationCheck();
	StartCollissionCheck();
	laneChangeEnded = true;
	touchEnded = true;
	StartInputCheck();
	engine->events.publishEvent(TEXT("startFixZLocation"), 0.0f);
	engine->events.publishEvent(TEXT("startSpotlightTargetBottle"));
}

void Defender_Network::StartCharacterTouch()
{
	//Is already happening on Client side !!!
}

void Defender_Network::ResetDefendNetwork()
{
	ground->SetActive(false);
	ResetLocalVariables();
	pickUpActor->controller->ResetAllActiveTasks();
	pickUpActor->Reset();
	engine->events.publishEvent(TEXT("SpotlightTargetBottleReset"));
	characterAnim->SetAnimation(0);
}

void Defender_Network::OnRotateFinished()
{
	switch (currentStage)
	{
	case WaitingForHit:
		characterAnim->SetAnimation(0);
		engine->events.publishEvent(endDefendNetwork);
		break;
	case PickUpBottle:
		GoBackToStartLine();
		engine->events.publishEvent(TEXT("updateDefendingStage"), DefendingStages::GoBackToStart);
		break;
	default:
		break;
	}
}

void Defender_Network::OnDirCalculated(Move m)
{
	switch (currentStage)
	{
	case WaitingForHit:
	case RunToBottle:
	case GoBackToStart: {
		if (laneChangeEnded) {
			laneChangeEnded = false;
			engine->events.publishEvent(TEXT("endFixZLocation"));
			engine->events.publishEvent(TEXT("StartLaneChange"), m);
		}
		break;
	}
	default:
		laneChangeEnded = true;
		break;
	}
}

void Defender_Network::OnBottleHit()
{
	currentStage = RunToBottle;
	engine->events.publishEvent(TEXT("updateDefendingStage"), currentStage);
	StartRunning(true, 0.0f);
	StartCharacterIsCloseToBottleCheck();
	ground->SetActive(true);
	characterAnim->SetAnimation(1);
}
