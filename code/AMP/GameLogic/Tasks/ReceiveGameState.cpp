#include "Tasks/WinLoose.h"
#include "ReceiveGameState.h"

void ReceiveGameState::Init()
{
	engine->events.subscribeEvent(receiveGameStateReset, &ReceiveGameState::Reset, this);
	engine->events.subscribeEvent(startReceiveGameState, &ReceiveGameState::StartReceivingGameState, this);
	engine->events.subscribeEvent(stopReceiveGameState, &ReceiveGameState::End, this);
	engine->events.subscribeEvent(engine->postInit, &ReceiveGameState::PostInit, this);
	engine->events.subscribeEvent(setBottleCounter, &ReceiveGameState::SetBottleCounter, this);
	engine->events.subscribeEvent(TEXT("endStreamedDefender"), &ReceiveGameState::EndSounds, this);
	engine->events.subscribeEvent(TEXT("endStreamAttacker"), &ReceiveGameState::EndSounds, this);
	LoadSounds();
}

void ReceiveGameState::LoadSounds()
{
	hitSoundIndex[0] = engine->sound.load(TEXT("Sound/S_bottlesound1.wav"));
	hitSoundIndex[1] = engine->sound.load(TEXT("Sound/S_bottlesound2.wav"));
	hitSoundIndex[2] = engine->sound.load(TEXT("Sound/S_bottlesound3.wav"));
	hitSoundIndex[3] = engine->sound.load(TEXT("Sound/S_bottlesound4.wav"));

	rollSoundIndex[0] = engine->sound.load(TEXT("Sound/S_BottleRoll1.wav"));
	rollSoundIndex[1] = engine->sound.load(TEXT("Sound/S_BottleRoll2.wav"));

	fallbackSoundIndex = engine->sound.load(TEXT("Sound/S_fallback.wav"));
	jumpSoundIndex = engine->sound.load(TEXT("Sound/S_jump.wav"));
}

void ReceiveGameState::PostInit()
{
	this->pickUpActor = (PickUpActor*)engine->getSceneActorByName(TEXT("PickUpActor"));
	this->throwingActor = (ThrowingActor*)engine->getSceneActorByName(TEXT("ThrowingActor"));
	assert(pickUpActor);
	assert(throwingActor);
	if (networking)AddUpdate();
}

void ReceiveGameState::StartReceivingGameState(amp::NetworkUDP* network)
{
	assert(network);
	this->networking = network;
	if (pickUpActor)AddUpdate();
}

void ReceiveGameState::Update()
{
	auto bson = networking->getReceivedInfo();
	CurrentState s;
	if(bson.size() > 0){
		s = CurrentState(bson);
		UpdateGameScene(s);
		lastGameState = s;
		if (isFirst)CallFirstNetworkEvent();
	}
}

void ReceiveGameState::CallFirstNetworkEvent()
{
	engine->events.publishEvent(TEXT("NetworkingIsRunning"));
	isFirst = false;
}

void ReceiveGameState::UpdateGameScene(CurrentState& s)
{
	UpdateProgress(s);
	UpdateGameState(s);
	ClearExecutedInputFlags(s);
	UpdateDefendingPlayer(s);
	UpdateSpotlight(s);
	SetBottles(s);
	SetAnimation(s);
	UpdateSound(s);
}

void ReceiveGameState::SetAnimation(CurrentState& s)
{
	if (lastGameState.currentAnimation != s.currentAnimation)pickUpActor->anim->SetAnimation(s.currentAnimation);
}

void ReceiveGameState::UpdateSpotlight(CurrentState& s)
{
	throwingActor->M_TargetSpot->setTransform(s.spotlight);
}

void ReceiveGameState::UpdateDefendingPlayer(CurrentState& s)
{
	//add last Def Trans
	lastDefendingTransforms.insert(lastDefendingTransforms.begin(),s.DefenderTransform);
	if (lastDefendingTransforms.size() > numLastDefTrans)lastDefendingTransforms.pop_back();

	float avg_matrix[16] = { 0.0f };

	for (auto& lastMat : lastDefendingTransforms) {
		auto* mat = glm::value_ptr(lastMat);
		for (int i = 0; i < 16; ++i) {
			avg_matrix[i] += mat[i];
		}
	}
	for (int i = 0; i < 16; ++i) {
		avg_matrix[i] /= (float)lastDefendingTransforms.size();
	}

	pickUpActor->collider->setActive(false);
	pickUpActor->collider->setTransform(glm::make_mat4(avg_matrix));
}

void ReceiveGameState::SetBottles(CurrentState& s)
{
	if (s.currentClientStage == Attacker) {
		for (int i = 0; i < bottleCounter; ++i) {
			if (i == 0)SetNewRBTrans(throwingActor->R_ThrowBottle, s.ThrowingBottle);
			else if (i == 1)SetNewRBTrans(throwingActor->R_AttackBottles[i - 1], s.b0);
			else if (i == 2)SetNewRBTrans(throwingActor->R_AttackBottles[i - 1], s.b1);
			else if (i == 3)SetNewRBTrans(throwingActor->R_AttackBottles[i - 1], s.b2);
			else if (i == 4)SetNewRBTrans(throwingActor->R_AttackBottles[i - 1], s.b3);
			else if (i == 5)SetNewRBTrans(throwingActor->R_AttackBottles[i - 1], s.b4);
		}

	}
	else {
		SetNewRBTrans(throwingActor->R_ThrowBottle, s.ThrowingBottle);
		SetNewRBTrans(throwingActor->R_AttackBottles[0], s.b0);
		SetNewRBTrans(throwingActor->R_AttackBottles[1], s.b1);
		SetNewRBTrans(throwingActor->R_AttackBottles[2], s.b2);
		SetNewRBTrans(throwingActor->R_AttackBottles[3], s.b3);
		SetNewRBTrans(throwingActor->R_AttackBottles[4], s.b4);
	}
	SetNewRBTrans(throwingActor->R_TargetBottle, s.TargetBottle);
}

void ReceiveGameState::ClearExecutedInputFlags(CurrentState& s)
{
	engine->events.publishEvent(TEXT("clearInputFlags"), s.inputFlags);
}

void ReceiveGameState::SetNewRBTrans(amp::DynamicMesh* mesh, glm::mat4 trans)
{
	mesh->setActive(false);
	mesh->setTransform(trans);
}

void ReceiveGameState::UpdateGameState(CurrentState& s)
{
	if (s.currentClientStage == Attacker) {
		if (lastGameState.currentClientStage != Attacker)engine->events.publishEvent(TEXT("endStreamedDefender"));
		if (lastGameState.attackStage != s.attackStage)CallAttackEvent(s.attackStage);
	}
	else {
		if (lastGameState.currentClientStage != Defender) {
			engine->events.publishEvent(TEXT("endStreamAttacker"));
		}
		if (lastGameState.defendStage != s.defendStage) {
			CallDefendEvent(s.defendStage);
		}	
	}
}

void ReceiveGameState::UpdateProgress(CurrentState& s)
{
	if (s.progressClient != lastGameState.progressClient) {
		engine->events.publishEvent(TEXT("setPlayerProgress"), Player::One, s.progressClient);
	}
	if (s.progressServer != lastGameState.progressServer) {
		engine->events.publishEvent(TEXT("setPlayerProgress"), Player::Two, s.progressServer);
	}
}

void ReceiveGameState::CallDefendEvent(DefendingStages ds)
{
	switch (ds)
	{
	case WaitingForHit:
		engine->events.publishEvent(TEXT("WaitingForHit"));
		break;
	case RunToBottle:
		engine->events.publishEvent(TEXT("RunToBottle"));
		break;
	case GoBackToStart:
		engine->events.publishEvent(TEXT("GoBackToStart"));
		break;
	default:
		break;
	}
}

void ReceiveGameState::CallAttackEvent(AttackingStages as)
{
	switch (as)
	{
	case AttackingCenterBottle:
		engine->events.publishEvent(TEXT("AttackingCenterBottle"));
		break;
	case AttackDefendingPlayer:
		engine->events.publishEvent(TEXT("AttackDefendingPlayer"));
		break;
	default:
		break;
	}
}

void ReceiveGameState::End()
{
	RemoveUpdate();
	networking = nullptr;
}

void ReceiveGameState::Reset()
{
	ResetUpdate();
	networking = nullptr;
}

void ReceiveGameState::SetBottleCounter(int counter)
{
	bottleCounter = counter;
}

void ReceiveGameState::UpdateSound(CurrentState& s)
{
	if (soundFlags != s.soundflags) {
		if (!isFlagged(soundFlags, SoundFlags::ROLL) && isFlagged(s.soundflags, SoundFlags::ROLL)) {
			int index = std::rand() % 2;
			engine->sound.play(rollSoundIndex[index], 1.0f);
		}
		if (!isFlagged(soundFlags, SoundFlags::HIT) && isFlagged(s.soundflags, SoundFlags::HIT)) {
			int index = std::rand() % 4;
			engine->sound.play(hitSoundIndex[index], 1.0f);
		}
		if (!isFlagged(soundFlags, SoundFlags::FALLBACK) && isFlagged(s.soundflags, SoundFlags::FALLBACK)) {
			engine->sound.play(fallbackSoundIndex, 1.0f);
		}
		if (!isFlagged(soundFlags, SoundFlags::JUMP) && isFlagged(s.soundflags, SoundFlags::JUMP)) {
			engine->sound.play(jumpSoundIndex, 1.0f);
		}
		soundFlags = s.soundflags;
		engine->events.publishEvent(TEXT("acceptSoundFlags"), soundFlags);
	}
}

void ReceiveGameState::EndSounds()
{
	engine->sound.RemoveAllandClear({rollSoundIndex[0],rollSoundIndex[1]});
}