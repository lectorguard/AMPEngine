#include "pch.h"
#include "Tasks/BottleValidityCheck.h"
#include "Tasks/WinLoose.h"
#include "Attacker_Streamed.h"

void Attacker_Streamed::AfterWindowStartup()
{
}

void Attacker_Streamed::OnActivate()
{
	//Engine Events
	SUB(engine->postInit, Attacker_Streamed::OnPostInit);

	//Game Events
	SUB(startStreamAttackPlayer, Attacker_Streamed::StartAttackPlayer);
	SUB(resetStreamAttackPlayer, Attacker_Streamed::ResetAttackPlayer);

	//ThrowingBottleTask
	SUB(TEXT("onBottleThrow"), Attacker_Streamed::onThrowBottle);
	SUB(TEXT("onInvalidThrow"), Attacker_Streamed::OnInvalidBottleThrow);

	//Attack_Stage Events
	SUB(TEXT("AttackingCenterBottle"), Attacker_Streamed::OnAttackCenterBottle);
	SUB(TEXT("AttackDefendingPlayer"), Attacker_Streamed::StartAttackDefendingPlayerState);
}

void Attacker_Streamed::OnDeactivate()
{
	UnsubscribeEvents();
}

void Attacker_Streamed::OnPostInit()
{
	throwActor = (ThrowingActor*)engine->getSceneActorByName(TEXT("ThrowingActor"));
	if (!throwActor)throw "Invalid Throw Actor";
	R_ThrowBottle = throwActor->R_ThrowBottle;
	R_TargetBottle = throwActor->R_TargetBottle;
	this->ground = (Ground_Client*)engine->getUIActorByName(TEXT("GroundActor_Client"));
}

void Attacker_Streamed::StartAttackPlayer()
{
	SetCamera();
	SetBottleToCamera();
	throwActor->HideAttackBottles();
}

//State : AttackCenterBottle
void Attacker_Streamed::OnAttackCenterBottle()
{
	if (currentStage == AttackDefendingPlayer)ThrowAttackBottle();
	else {
		engine->events.publishEvent(TEXT("StartThrowingBottle"), R_ThrowBottle, throwActor->TargetThrowVec, throwActor->originThrowBottle);
		currentBottle = R_ThrowBottle;
	}
}

//EVENT
void Attacker_Streamed::onThrowBottle(amp::DynamicMesh* bottle, glm::vec3 force)
{
	engine->events.publishEvent(TEXT("acceptNewAttackThrowForce"), force);
	engine->events.publishEvent(TEXT("setBottleCounter"), ++bottleThrowCounter);
	if(currentStage == AttackDefendingPlayer)OnAttackCenterBottle();
}

//State: AttackDefendingPlayer
void Attacker_Streamed::StartAttackDefendingPlayerState()
{
	currentStage = AttackDefendingPlayer;
	ground->SetActive(true);
	ThrowAttackBottle();
	engine->events.publishEvent(TEXT("startIconTouch"));
}

void Attacker_Streamed::ThrowAttackBottle()
{
	if (AttackBottleIndex < throwActor->NumberOfAttackBottles) {
		auto* AttackBottle = throwActor->R_AttackBottles[AttackBottleIndex];
		throwActor->ResetThrowBottle(AttackBottle);
		engine->events.publishEvent(TEXT("StartThrowingBottle"), AttackBottle, throwActor->TargetThrowVec, throwActor->originThrowBottle);
		currentBottle = AttackBottle;
		++AttackBottleIndex;
	}
}

void Attacker_Streamed::onResetThrow()
{
	throwActor->ResetBottles();
	engine->events.publishEvent(TEXT("StartThrowingBottle"), R_ThrowBottle, throwActor->TargetThrowVec, throwActor->originThrowBottle);
	currentBottle = R_ThrowBottle;
}

void Attacker_Streamed::SetBottleToCamera()
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

void Attacker_Streamed::SetCamera()
{
	amp::Camera::resetView();
	amp::Camera::setViewLocation(glm::vec3(148.0f, 30.0f, 0));
	//Camera::setViewLocation(glm::vec3(235.0f, 30.0f, 0));
	amp::Camera::setViewRotation(glm::vec3(0, 1, 0), glm::vec3(-1, 0, 0));
	amp::Camera::rotateView(15.0f, glm::vec3(0, 0, 1));
}

void Attacker_Streamed::OnInvalidBottleThrow()
{
	OnAttackCenterBottle();
}

void Attacker_Streamed::ResetAttackPlayer()
{
	ground->SetActive(false);
	throwActor->controller->ResetAllActiveTasks();
	CanThrow = false;
	AttackBottleIndex = 0;
	throwActor->Reset();
	currentStage = NoAttack;
	bottleThrowCounter = 0;
	engine->events.publishEvent(TEXT("setBottleCounter"), bottleThrowCounter);
	engine->events.publishEvent(TEXT("IconTouchReset"));
}