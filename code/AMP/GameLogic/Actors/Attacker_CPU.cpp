#include "pch.h"
#include "Attacker_CPU.h"
#include "cstdlib"
#include "Event/EventSystem.h"
#include "Tasks/WinLoose.h"

void Attacker_CPU::AfterWindowStartup()
{
}

void Attacker_CPU::OnActivate()
{
	//Engine Events
	SUB(engine->postInit, Attacker_CPU::OnPostInit);

	//Game Events
	SUB(startAttackerCPU, Attacker_CPU::StartAttackCPU);
	SUB(resetAttackerCPU, Attacker_CPU::ResetAttackCPU);

	//BottleHitTask
	SUB(TEXT("onBottleHit"), Attacker_CPU::onHit);

	//BottleValidity Task
	SUB(TEXT("bottleInvalid"), Attacker_CPU::onInvalid);
}

void Attacker_CPU::OnDeactivate()
{
	UnsubscribeEvents();
}

void Attacker_CPU::OnPostInit()
{
	throwActor = (ThrowingActor*)engine->getSceneActorByName(TEXT("ThrowingActor"));
	if (!throwActor)throw "Invalid Throw Actor";
	R_ThrowBottle = throwActor->R_ThrowBottle;
	R_TargetBottle = throwActor->R_TargetBottle;
}

void Attacker_CPU::StartAttackCPU()
{
	amp::TimeEvent ev;
	ev.init(5.0f, false, &Attacker_CPU::StartThrowBottles, this);
	engine->addTimerFunction(ev);
}

void Attacker_CPU::StartThrowBottles()
{
	if (!IsActive())return;
	currentStage = AttackingCenterBottle;
	throwActor->ResetBottles();
	ThrowBottle(R_ThrowBottle, bottleHitForce[std::rand() % 3]);
	unsigned char flags = YChecking | TimeChecking | VelocityChecking;
	engine->events.publishEvent(TEXT("startBottleValidityCheck"), R_ThrowBottle, flags);
	engine->events.publishEvent(TEXT("startBottleHitCheck"));
}

void Attacker_CPU::ThrowBottle(amp::DynamicMesh* bottle,glm::vec3 force)
{
	bottle->setActive(true);
	bottle->applyForceToCenter(force * 0.9f);
}

void Attacker_CPU::onHit()
{
	engine->events.publishEvent(TEXT("BottleHitCheckReset"));
	engine->events.publishEvent(TEXT("BottleValidityCheckReset"));
	//Start New Tasks
	currentStage = AttackDefendingPlayer;
	unsigned char flag = YChecking;
	engine->events.publishEvent(TEXT("startBottleValidityCheck"), R_TargetBottle, flag);
	

	//Helping Defending Actor to get Bottles Location
	engine->events.publishEvent(TEXT("addSpectatingMesh"), R_TargetBottle, true);

	AttackBottleIndex = 0;
	TimeAttackBottleThrow();
	//Drink beer all 2 seconds !!
	CreateDrinkingTimer();

}

void Attacker_CPU::CreateDrinkingTimer()
{
	amp::TimeEvent ev;
	ev.init(0.5f, false, &Attacker_CPU::drinkBeer, this);
	engine->addTimerFunction(ev);
}

void Attacker_CPU::TimeAttackBottleThrow()
{
	amp::TimeEvent ev;
	ev.init(3.0f, false, &Attacker_CPU::ThrowAttackBottle, this);
	engine->addTimerFunction(ev);
}

void Attacker_CPU::ThrowAttackBottle()
{
	if (!IsActive())return;
	if (AttackBottleIndex < throwActor->NumberOfAttackBottles) {
		auto* AttackBottle = throwActor->R_AttackBottles[AttackBottleIndex];
		throwActor->ResetThrowBottle(AttackBottle);
		ThrowBottle(AttackBottle,bottleHitForce[std::rand() % 3]);
		++AttackBottleIndex;
		TimeAttackBottleThrow();
	}
}

//CAN FINISH GAME ROUND
void Attacker_CPU::onInvalid(amp::DynamicMesh* mesh, InvalidType type)
{
	switch (currentStage)
	{
	case AttackingCenterBottle:
		//Game Round is Finished
		engine->events.publishEvent(endAttackerCPU);
		break;
	case AttackDefendingPlayer:
		throwActor->ResetTargetBottle();
		break;
	default:
		if(currentStage != NoAttack)throw "currentStageShould be one of the listed";
		break;
	}
}

void Attacker_CPU::drinkBeer()
{
	if (currentStage == AttackDefendingPlayer) {
		engine->events.publishEvent(TEXT("updatePlayerProgress"), Player::Two);
		CreateDrinkingTimer();
	}
}

void Attacker_CPU::ResetAttackCPU()
{
	throwActor->controller->ResetAllActiveTasks();
	AttackBottleIndex = 0;
	currentStage = NoAttack;
	throwActor->Reset();
}
