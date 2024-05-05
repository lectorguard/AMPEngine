#include "pch.h"
#include "Tasks/BottleValidityCheck.h"
#include "Tasks/WinLoose.h"
#include "Attacker_Player.h"

void Attacker_Player::AfterWindowStartup()
{
}

void Attacker_Player::OnActivate()
{
	//Engine Events
	SUB(engine->postInit, Attacker_Player::OnPostInit);

	SubscribeGameEvents();

	//ThrowingBottleTask
	SUB(TEXT("onBottleThrow"), Attacker_Player::onThrowInput);
	SUB(TEXT("onInvalidThrow"), Attacker_Player::onInvalidBottleThrow);

	//BottleHitTask
	SUB(TEXT("onBottleHit"), Attacker_Player::onHit);

	//BottleValidity Task
	SUB(TEXT("bottleInvalid"), Attacker_Player::onInvalid);
	
	//Icon Touch Event Task
	SUB(TEXT("onTouchIcon"), Attacker_Player::onIconTouch);
}

void Attacker_Player::SubscribeGameEvents()
{
	//Game Events
	SUB(startAttackPlayer, Attacker_Player::StartAttackPlayer);
	SUB(resetAttackPlayer, Attacker_Player::ResetAttackPlayer);
}

void Attacker_Player::OnDeactivate()
{
	UnsubscribeEvents();
}

void Attacker_Player::OnPostInit()
{
	throwActor = (ThrowingActor*)engine->getSceneActorByName(TEXT("ThrowingActor"));
	if (!throwActor)throw "Invalid Throw Actor";
	R_ThrowBottle = throwActor->R_ThrowBottle;
	R_TargetBottle = throwActor->R_TargetBottle;
}

void Attacker_Player::StartAttackPlayer()
{
	currentStage = AttackingCenterBottle;
	SetCamera();
	SetBottleToCamera();
	engine->events.publishEvent(TEXT("StartThrowingBottle"),R_ThrowBottle, throwActor->TargetThrowVec, throwActor->originThrowBottle);
}

//EVENT
void Attacker_Player::onThrowInput(amp::DynamicMesh* bottle, glm::vec3 force)
{
	throwActor->ThrowBottle(bottle, force);
	CanThrow = true;
	switch (currentStage)
	{
	case AttackingCenterBottle: {
		unsigned char flags = YChecking | TimeChecking | VelocityChecking;
		engine->events.publishEvent(TEXT("startBottleValidityCheck"), R_ThrowBottle, flags);
		engine->events.publishEvent(TEXT("startBottleHitCheck"));
		break;
	}
	case AttackDefendingPlayer: {
		ThrowAttackBottle();
		break;
	}
	case NotActive: {
		throw "currentStageShould be one of the listed";
		break;
	}
	}
}

//EVENT
void Attacker_Player::onInvalidBottleThrow()
{
	//For All Stages
	CanThrow = true;
	switch (currentStage)
	{
	case NoAttack:
	case AttackingCenterBottle:
		engine->events.publishEvent(TEXT("StartThrowingBottle"), R_ThrowBottle, throwActor->TargetThrowVec, throwActor->originThrowBottle);
		break;
	case AttackDefendingPlayer:
		assert(AttackBottleIndex > 0);
		engine->events.publishEvent(TEXT("StartThrowingBottle"), throwActor->R_AttackBottles[AttackBottleIndex-1], throwActor->TargetThrowVec, throwActor->originThrowBottle);
		break;
	default:
		break;
	}
}

//EVENT
void Attacker_Player::onHit()
{
	if (currentStage == AttackingCenterBottle) {
		StartAttackDefendingPlayerState();
	}
	else throw "Center Bottle can just be hit Attacking Center Bottle stage !!!";
}

void Attacker_Player::StartAttackDefendingPlayerState()
{
	//Full Reset of Old Tasks
	engine->events.publishEvent(TEXT("ThrowingBottleReset"));
	engine->events.publishEvent(TEXT("BottleHitCheckReset"));
	engine->events.publishEvent(TEXT("BottleValidityCheckReset"));
	//Start New Tasks
	currentStage = AttackDefendingPlayer;
	unsigned char flag = YChecking;
	engine->events.publishEvent(TEXT("startBottleValidityCheck"), R_TargetBottle, flag);
	AttackBottleIndex = 0;
	ThrowAttackBottle();
	//Helping Defending Actor to get Bottles Location
	engine->events.publishEvent(TEXT("addSpectatingMesh"), R_TargetBottle, true);
	//Player can drink now Prost !!!
	engine->events.publishEvent(TEXT("startIconTouch"));
}

void Attacker_Player::onIconTouch()
{
	assert(currentStage == AttackDefendingPlayer);
	engine->events.publishEvent(TEXT("updatePlayerProgress"), Player::One);
}

//EVENT
//ROUND CAN FINISH HERE
void Attacker_Player::onInvalid(amp::DynamicMesh* mesh, InvalidType type)
{
	switch (currentStage)
	{
	case AttackingCenterBottle:
		//onResetThrow();
		engine->events.publishEvent(endAttackPlayer);
		break;
	case AttackDefendingPlayer:
		throwActor->ResetTargetBottle();
		break;
	default:
		throw "currentStageShould be one of the listed";
		break;
	}
}

void Attacker_Player::ThrowAttackBottle()
{
	if (AttackBottleIndex < throwActor->NumberOfAttackBottles) {
		auto* AttackBottle = throwActor->R_AttackBottles[AttackBottleIndex];
		throwActor->ResetThrowBottle(AttackBottle);
		engine->events.publishEvent(TEXT("StartThrowingBottle"), AttackBottle, throwActor->TargetThrowVec, throwActor->originThrowBottle);
		++AttackBottleIndex;
	}
}

void Attacker_Player::onResetThrow()
{
	if (!CanThrow)return;
	engine->events.publishEvent(TEXT("stopBottleValidityCheck"), R_ThrowBottle);
	engine->events.publishEvent(TEXT("endBottleHitCheck"));
	throwActor->ResetBottles();
	engine->events.publishEvent(TEXT("StartThrowingBottle"), R_ThrowBottle, throwActor->TargetThrowVec, throwActor->originThrowBottle);
	CanThrow = false;
}

void Attacker_Player::SetBottleToCamera()
{
	R_ThrowBottle->setTransform(amp::Camera::getWorldView());
	auto loc = R_ThrowBottle->getLocation();
	auto delta = 30.0f * amp::Camera::getForwardView() - 6.0f * amp::Camera::getUpView();
	loc = loc + delta;
	R_ThrowBottle->setLocation(loc);
	auto resMat = glm::rotate(R_ThrowBottle->getTransform(), glm::radians(75.0f), glm::vec3(0, 1, 0));
	R_ThrowBottle->setTransform(resMat);
	throwActor->originThrowBottle = resMat;
}

void Attacker_Player::SetCamera()
{
	amp::Camera::resetView();
	amp::Camera::setViewLocation(glm::vec3(148.0f, 30.0f, 0));
	//Camera::setViewLocation(glm::vec3(235.0f, 30.0f, 0));
	amp::Camera::setViewRotation(glm::vec3(0, 1, 0), glm::vec3(-1, 0, 0));
	amp::Camera::rotateView(15.0f, glm::vec3(0, 0, 1));
}

void Attacker_Player::ResetAttackPlayer()
{

	throwActor->controller->ResetAllActiveTasks();
	hasHitBottle = false;
	CanThrow = false;
	AttackBottleIndex = 0;
	throwActor->Reset();
	engine->events.publishEvent(TEXT("IconTouchReset"));
}