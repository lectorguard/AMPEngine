#include "SendGameState.h"

void SendGameState::Init()
{
	engine->events.subscribeEvent(TEXT("SendGameStateReset"), &SendGameState::Reset, this);
	engine->events.subscribeEvent(TEXT("startSendGameState"), &SendGameState::StartSendingGameState, this);
	engine->events.subscribeEvent(TEXT("stopSendGameState"), &SendGameState::End, this);
	engine->events.subscribeEvent(engine->postInit, &SendGameState::PostInit, this);

	engine->events.subscribeEvent(TEXT("onChangingProgress"), &SendGameState::OnChangingProgressServer, this);

	engine->events.subscribeEvent(updateAttackingStage, &SendGameState::UpdateAttackingStage, this);
	engine->events.subscribeEvent(updateDefendingStage, &SendGameState::UpdateDefendingStage, this);
	engine->events.subscribeEvent(updateGameState, &SendGameState::UpdateGameState, this);
	engine->events.subscribeEvent(TEXT("sendInputFlags"), &SendGameState::SendInputFlags, this);
	engine->events.subscribeEvent(TEXT("addSoundFlags"), &SendGameState::AddSoundFlags, this);
	engine->events.subscribeEvent(TEXT("setSoundFlags"), &SendGameState::SetSoundFlags, this);
}

void SendGameState::PostInit()
{
	this->pickUpActor = (PickUpActor*)engine->getSceneActorByName(TEXT("PickUpActor"));
	this->throwingActor = (ThrowingActor*)engine->getSceneActorByName(TEXT("ThrowingActor"));
	assert(pickUpActor);
	assert(throwingActor);
	if (networking)AddUpdate();
}

void SendGameState::StartSendingGameState(amp::NetworkUDP* network)
{
	assert(network);
	this->networking = network;
	if (pickUpActor)AddUpdate();
}

void SendGameState::Update()
{
	SetUpCurrentState(state);
	networking->Send(state.bsify());
}

void SendGameState::SetUpCurrentState(CurrentState& state)
{
	state.DefenderTransform = pickUpActor->collider->getTransform();
	state.ThrowingBottle = throwingActor->M_ThrowBottle->getTransform();
	state.TargetBottle = throwingActor->M_TargetBottle->getTransform();
	state.b0 = throwingActor->M_AttackBottles[0]->getTransform();
	state.b1 = throwingActor->M_AttackBottles[1]->getTransform();
	state.b2 = throwingActor->M_AttackBottles[2]->getTransform();
	state.b3 = throwingActor->M_AttackBottles[3]->getTransform();
	state.b4 = throwingActor->M_AttackBottles[4]->getTransform();
	state.progressClient = drunkLevelClient;
	state.progressServer = drunkLevelServer;
	state.spotlight = throwingActor->M_TargetSpot->getTransform();
	state.currentAnimation = pickUpActor->anim->GetAnimation();
	state.soundflags = soundFlags;
}

void SendGameState::End()
{
	RemoveUpdate();
	networking = nullptr;
}

void SendGameState::Reset()
{
	ResetUpdate();
	networking = nullptr;
}

void SendGameState::OnChangingProgressServer(Player player, float currentProgress)
{
	if (player == Player::One) {
		drunkLevelServer = currentProgress;
	}
	else drunkLevelClient = currentProgress;
}

void SendGameState::UpdateAttackingStage(AttackingStages stage)
{
	state.attackStage = stage;
}

void SendGameState::UpdateDefendingStage(DefendingStages stage)
{
	state.defendStage = stage;
}

void SendGameState::UpdateGameState(GameStagesOffline stage)
{
	state.currentClientStage = stage;
}

void SendGameState::SendInputFlags(uint8_t flags)
{
	state.inputFlags = flags;
}

void SendGameState::AddSoundFlags(uint8_t flags)
{
	soundFlags |= flags;
}

void SendGameState::SetSoundFlags(uint8_t flags)
{
	for (int i = 0; i < 6; ++i) {
		if (isFlagged(flags, 1 << i))unflag(soundFlags, 1 << i);
	}
}

