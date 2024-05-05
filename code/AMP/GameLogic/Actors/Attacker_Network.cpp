#include "Attacker_Network.h"

void Attacker_Network::SubscribeGameEvents()
{
	SUB(startAttackNetwork, Attacker_Network::StartAttackNetwork);
	SUB(resetAttackNetwork, Attacker_Network::ResetAttackNetwork);
	SUB(TEXT("onThrowInput"), Attacker_Network::onThrowNetworkInput);
}

void Attacker_Network::StartAttackNetwork()
{
	currentStage = AttackingCenterBottle;
	//Send Attacking Stage to client
	engine->events.publishEvent(TEXT("updateAttackingStage"), currentStage);
	CheckUpdateFlags = 0;
	engine->events.publishEvent(TEXT("updateThrowedBottles"),CheckUpdateFlags);
}

void Attacker_Network::StartAttackDefendingPlayerState()
{
	//Full Reset of Old Tasks
	engine->events.publishEvent(TEXT("BottleHitCheckReset"));
	engine->events.publishEvent(TEXT("BottleValidityCheckReset"));
	//Start New Tasks
	currentStage = AttackDefendingPlayer;
	//Allows client to throw Attack Bottle and to collect drink icons
	engine->events.publishEvent(TEXT("updateAttackingStage"), currentStage);
	unsigned char flag = YChecking;
	engine->events.publishEvent(TEXT("startBottleValidityCheck"), R_TargetBottle, flag);
	//Helping Defending Actor to get Bottle Location
	engine->events.publishEvent(TEXT("addSpectatingMesh"), R_TargetBottle, true);
}

//Bottle Ptr is alway null, because the input just knows that a bottle is thrown, but not which one
void Attacker_Network::onThrowNetworkInput(glm::vec3 force)
{
	CanThrow = true;
	switch (currentStage)
	{
	case AttackingCenterBottle: {
		throwActor->ThrowBottle(throwActor->R_ThrowBottle, force);
		engine->events.publishEvent(TEXT("updateAttackingStage"), currentStage);
		unsigned char flags = YChecking | TimeChecking | VelocityChecking;
		engine->events.publishEvent(TEXT("startBottleValidityCheck"), R_ThrowBottle, flags);
		engine->events.publishEvent(TEXT("startBottleHitCheck"));
		break;
	}
	case AttackDefendingPlayer: {
		if (AttackBottleIndex < throwActor->NumberOfAttackBottles) {
			auto* AttackBottle = throwActor->R_AttackBottles[AttackBottleIndex];
			throwActor->ResetThrowBottle(AttackBottle);
			throwActor->ThrowBottle(AttackBottle, force);
			++AttackBottleIndex;
		}
		break;
	}
	case NotActive: {
		throw "currentStageShould be one of the listed";
		break;
	}
	}
}

void Attacker_Network::onIconTouch()
{
	assert(currentStage == AttackDefendingPlayer);
	engine->events.publishEvent(TEXT("updatePlayerProgress"), Player::Two);
}

void Attacker_Network::onInvalid(amp::DynamicMesh* mesh, InvalidType type)
{
	switch (currentStage)
	{
	case AttackingCenterBottle:
		engine->events.publishEvent(endAttackNetwork);
		break;
	case AttackDefendingPlayer:
		throwActor->ResetTargetBottle();
		break;
	default:
		throw "currentStageShould be one of the listed";
		break;
	}
}

void Attacker_Network::ResetAttackNetwork()
{
	throwActor->controller->ResetAllActiveTasks();
	hasHitBottle = false;
	CanThrow = false;
	AttackBottleIndex = 0;
	throwActor->Reset();
	currentStage = NoAttack;
	engine->events.publishEvent(TEXT("updateAttackingStage"), currentStage);
}
