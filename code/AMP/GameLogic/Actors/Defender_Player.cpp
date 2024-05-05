#include "pch.h"
#include "Defender_Player.h"

void Defender_Player::OnActivate()
{
	//Engine Events
	SUB(engine->postInit, Defender_Player::OnPostInit);

	SubscribeGameEvents();

	//Character Control
	SUB(TEXT("onCharacterRelocate"), Defender_Player::OnCharacterRelocate);

	//BottleHitEvent
	SUB(TEXT("onBottleHit"), Defender_Player::OnBottleHit);

	//Mesh Spectator
	SUB(TEXT("onMeshesClose"), Defender_Player::OnMeshesClose);
	SUB(TEXT("onCharacterBehineBottle"), Defender_Player::OnCharacterRelocate);

	//rotateTask
	SUB(TEXT("onRotateFinished"), Defender_Player::OnRotateFinished);

	//collision
	SUB(TEXT("onCollisionObject"), Defender_Player::OnCollisionBody);
	SUB(TEXT("onCollisionGround"), Defender_Player::OnCollissionGround);

	//CharacterTouch
	SUB(TEXT("onDirectionCalculated"), Defender_Player::OnDirCalculated);
	SUB(TEXT("endTouchGest"), Defender_Player::OnEndTouch);

	//ChangeLane
	SUB(TEXT("EndLaneChange"), Defender_Player::LaneChangeEnded);
	SUB(TEXT("JumpEvent"), Defender_Player::Jump);
	SUB(TEXT("InvalidLaneChange"), Defender_Player::LaneChangeEnded);
}

void Defender_Player::SubscribeGameEvents()
{
	//Game Events
	SUB(startDefendPlayer, Defender_Player::StartDefending);
	SUB(resetDefendPlayer, Defender_Player::ResetDefendingPlayer);
}

void Defender_Player::OnDeactivate()
{
	UnsubscribeEvents();
}

void Defender_Player::AfterWindowStartup()
{
}

void Defender_Player::OnPostInit()
{
	auto* pickUpActor = (PickUpActor*)engine->getSceneActorByName(TEXT("PickUpActor"));
	this->pickUpActor = pickUpActor;
	this->character = pickUpActor->collider;
	this->characterAnim = pickUpActor->anim;
	this->ground = (Ground*)engine->getUIActorByName(TEXT("GroundActor"));
}

void Defender_Player::StartDefending()
{
	ResetLocalVariables();


	InitCamera();
	StartRespawnAndOrientationCheck();
	StartCollissionCheck();

	laneChangeEnded = true;
	touchEnded = true;
	//Random error here
	StartInputCheck();
	engine->events.publishEvent(TEXT("startFixZLocation"), 0.0f);
	engine->events.publishEvent(TEXT("startSpotlightTargetBottle"));
}

void Defender_Player::StartCollissionCheck()
{
	engine->events.publishEvent(TEXT("startCollisionCheck"));
}

void Defender_Player::InitCamera()
{
	amp::Camera::resetView();
	amp::Camera::setViewLocation(glm::vec3(-47, 40, 0));
	amp::Camera::setViewRotation(glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));
	amp::Camera::rotateView(-25.0f, glm::vec3(0, 0, 1));
	engine->events.publishEvent(TEXT("startFollowingCamera"), amp::Camera::getWorldView(), std::make_optional<float>(40.0f));
}



void Defender_Player::StartRespawnAndOrientationCheck()
{
	engine->events.publishEvent(TEXT("startRelocateWhenFalling"), -40.0f);
	engine->events.publishEvent(TEXT("startFixOrientation"),true);
}



//TRANSITION WaitingForHit -> RunToBottle
//EVENT
void Defender_Player::OnBottleHit()
{
	currentStage = RunToBottle;
	StartRunning(true, 0.0f);
	StartCharacterIsCloseToBottleCheck();
	ground->SetActive(true);
	characterAnim->SetAnimation(1);
}

void Defender_Player::StartCharacterIsCloseToBottleCheck()
{
	engine->events.publishEvent(TEXT("addSpectatingMesh"), character, false);
}

void Defender_Player::StartInputCheck()
{
	switch (currentStage)
	{
	case WaitingForHit:
	case RunToBottle:
	case GoBackToStart:
		//Random error here
		StartCharacterTouch();
		break;
	default:
		break;
	}
}

void Defender_Player::StartCharacterTouch()
{
	if (laneChangeEnded && touchEnded) {
		laneChangeEnded = false;
		touchEnded = false;
		engine->events.publishEvent(TEXT("StartCheckingCharacterTouch"));
	}
}

void Defender_Player::StartRunning(bool isForward,float Zlocation)
{
	engine->events.publishEvent(TEXT("startRunning"), isForward);
	engine->events.publishEvent(TEXT("startVelocityCheck"), true, true);
	StartInputCheck();
}

void Defender_Player::StopRunning()
{
	engine->events.publishEvent(TEXT("stopRunning"));
	engine->events.publishEvent(TEXT("endVelocityCheck"));
}

//EVENT
void Defender_Player::OnDirCalculated(Move m)
{
	switch (currentStage)
	{
	case WaitingForHit:
	case RunToBottle:
	case GoBackToStart: {
		engine->events.publishEvent(TEXT("endFixZLocation"));
		engine->events.publishEvent(TEXT("StartLaneChange"), m);
		break;
	}
	default:
		laneChangeEnded = true;
		break;
	}
	
}

//EVENT
void Defender_Player::OnEndTouch()
{
	touchEnded = true;
	StartInputCheck();
}

//EVENT
void Defender_Player::LaneChangeEnded(float targetZlocation)
{
	lastLaneLocation_Z = targetZlocation;
	engine->events.publishEvent(TEXT("startFixZLocation"), targetZlocation);
	laneChangeEnded = true;
	StartInputCheck();
}

void Defender_Player::Jump(glm::vec3 upForce, float targetZlocation)
{
	if(canJump){
		character->applyForceToCenter(glm::vec3(0,upForce.y,0) * JumpForce);
		//loc must be set to one otherwise collission check resets canJump instantly to true again 
		auto loc = character->getLocation();
		loc.y = loc.y + 1;
		character->setLocation(loc);
	}
	canJump = false;
	LaneChangeEnded(targetZlocation);
}


//TRANSITION RunToBottle->PickUpBottle
//EVENT
void Defender_Player::OnMeshesClose()
{
	//Reset events of old stage
	currentStage = PickUpBottle;
	StopRunning();
	ResetBottleSpectator();
	engine->events.publishEvent(TEXT("endFixOrientation"));
	engine->events.publishEvent(TEXT("endSpotlightTargetBottle"));
	engine->events.publishEvent(TEXT("startRotate"), glm::vec3(0, glm::radians(180.0f), 0));
	engine->events.publishEvent(TEXT("startMoveBottleToHead"), character);
}

void Defender_Player::ResetBottleSpectator()
{
	engine->events.publishEvent(TEXT("BottleSpectatorReset"));
}

//EVENT
//CALLS END GAME STATE
void Defender_Player::OnRotateFinished()
{
	switch (currentStage)
	{
	case WaitingForHit:
		engine->events.publishEvent(endDefendPlayer);
		characterAnim->SetAnimation(0);
		break;
	case PickUpBottle:
		GoBackToStartLine();
		break;
	default:
		break;
	}
}

//TRANSITION PickUpBottle-> GoBackToStart
void Defender_Player::GoBackToStartLine()
{
	currentStage = GoBackToStart;
	StartRunning(false, lastLaneLocation_Z);
	engine->events.publishEvent(TEXT("startFixOrientation"),false);
	engine->events.publishEvent(TEXT("updateFallbackCheck"), false, 50.0f);
	StartInputCheck();
}


//EVENT
void Defender_Player::OnCollisionBody()
{
	switch (currentStage)
	{
	case RunToBottle:
		character->applyForceToCenter(glm::vec3(-HitForce, 0, 0));
		engine->events.publishEvent(TEXT("updateFallbackCheck"), true, character->getLocation().x - X_DeltaFallback);
		break;
	case GoBackToStart:
		character->applyForceToCenter(glm::vec3(HitForce, 0, 0));
		engine->events.publishEvent(TEXT("updateFallbackCheck"), false, character->getLocation().x + X_DeltaFallback);
		break;
	default:
		break;
	}
}

void Defender_Player::OnCollissionGround()
{
	auto vel = character->getLinearVelocity();
	vel.y = 0;
	character->ResetLinearVelocity(vel);
	canJump = true;
}

void Defender_Player::OnCharacterRelocate()
{
	engine->events.publishEvent(TEXT("updateFallbackCheck"), true, pickUpActor->startLocation.x - X_DeltaFallback);
	character->setRotation(glm::quat(1, 0, 0, 0));
	character->setLocation(pickUpActor->startLocation);
}

void Defender_Player::Update()
{
	OnPlayerReachedStartLine();
}

//TRANSITION GoBackToStart -> Finished
void Defender_Player::ResetDefendingPlayer()
{
	ground->SetActive(false);
	ResetLocalVariables();
	pickUpActor->controller->ResetAllActiveTasks();
	pickUpActor->Reset();
	engine->events.publishEvent(TEXT("SpotlightTargetBottleReset"));
	characterAnim->SetAnimation(0);
}



void Defender_Player::ResetLocalVariables()
{
	currentStage = WaitingForHit;
	lastLaneLocation_Z = 0.0f;
	touchEnded = true;
	laneChangeEnded = true;
	canJump = true;
}

//TRANSITION WaitingForHit -> RunToBottle
void Defender_Player::OnPlayerReachedStartLine()
{
	if (currentStage == GoBackToStart && character->getLocation().x < pickUpActor->startLocation.x) {
		currentStage = WaitingForHit;
		character->ResetLinearVelocity();
		StopRunning();
		engine->events.publishEvent(TEXT("startRotate"), glm::vec3(0, 0, 0));
		engine->events.publishEvent(TEXT("MoveBottleToHeadReset"));
	}
}

Defender_Player::~Defender_Player()
{
}