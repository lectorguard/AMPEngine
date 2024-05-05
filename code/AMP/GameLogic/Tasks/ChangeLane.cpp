#include "pch.h"
#include "Sound/SoundPool.h"
#include "ChangeLane.h"
#include "GameUtility.h"

void ChangeLane::Init()
{
	engine->events.subscribeEvent(StartChange,&ChangeLane::StartLaneChange,this);
	engine->events.subscribeEvent(ChangeLaneReset, &ChangeLane::Reset, this);

	laneChangeSoundIndex = engine->sound.load(TEXT("Sound/S_fallback.wav"));
	jumpSoundIndex = engine->sound.load(TEXT("Sound/S_jump.wav"));
}


void ChangeLane::StartLaneChange(Move m)
{
	currentMove = m;
	if (currentMove == Move::NoMove){
		InvalidLaneChange();
		return;
	}
	glm::vec3 Directions[3] = { amp::Camera::getRightView(),-amp::Camera::getRightView(),amp::Camera::getUpView() };
	float laneWidth = Directions[currentMove].z * G_LaneInfo::laneDistancePlayer;

	//right or left
	if (currentMove < 2)
	{
		assert(G_LaneInfo::NumberOfLanesPlayer % 2 == 1);
		if (!IsChangedLaneValid(laneWidth)){
			InvalidLaneChange();
			return;
		}
		targetZlocation += laneWidth;
		assert(std::abs(character->getLocation().z - targetZlocation) < std::abs(laneWidth) + 5.0f);
		currentChangingVector = Directions[currentMove] * G_LaneInfo::changingForce;
		isTargetLaneGreater = targetZlocation > character->getLocation().z;
	}
	else {
		auto UpForce = Directions[currentMove] * G_LaneInfo::changingForceUpwards;
		currentMove = Move::NoMove;
		JumpEvent(UpForce);
		return;
	}
	engine->sound.play(laneChangeSoundIndex, 1.0f);
	engine->events.publishEvent(TEXT("addSoundFlags"), (uint8_t)SoundFlags::FALLBACK);
	AddUpdate();
}

bool ChangeLane::IsChangedLaneValid(float laneWidth)
{
	maxLanesPerSide = (G_LaneInfo::NumberOfLanesPlayer - 1) / 2;
	if (targetZlocation + laneWidth < -maxLanesPerSide * G_LaneInfo::laneDistancePlayer - 2.0f
		|| targetZlocation + laneWidth > maxLanesPerSide* G_LaneInfo::laneDistancePlayer + 2.0f) {
		currentMove = Move::NoMove;
		return false;
	}
	return true;
}



void ChangeLane::Update()
{
	//Fails e.g: when targetZlocation is wrong
	assert(character->getLocation().z > -maxLanesPerSide * G_LaneInfo::laneDistancePlayer - 10.0f);
	assert(character->getLocation().z <  maxLanesPerSide* G_LaneInfo::laneDistancePlayer + 10.0f);

	if (isTargetLaneGreater) {
		if (character->getLocation().z > targetZlocation) {
			End();
			return;
		}
	}
	else {
		if (character->getLocation().z < targetZlocation) {
			End();
			return;
		}
	}
	//Not reached Target Lane so more force
	character->applyForceToCenter(currentChangingVector);
}

void ChangeLane::FixCharacterZLocation()
{
	auto loc = character->getLocation();
	loc.z = targetZlocation;
	character->setLocation(loc);
	auto force = character->getLinearVelocity();
	force.z = 0.0f;
	character->ResetLinearVelocity(force);
}

void ChangeLane::End()
{
	RemoveUpdate();
	FixCharacterZLocation();
	currentMove = Move::NoMove;
	engine->events.publishEvent(endLaneChange,getTargetZLocation());
}

void ChangeLane::InvalidLaneChange()
{
	engine->events.publishEvent(invalidLaneChange, getTargetZLocation());
}

void ChangeLane::JumpEvent(glm::vec3 upForce)
{
	engine->sound.play(jumpSoundIndex, 1.0f);
	engine->events.publishEvent(TEXT("addSoundFlags"), (uint8_t)SoundFlags::JUMP);
	engine->events.publishEvent(jumpEvent, upForce, getTargetZLocation());
}

void ChangeLane::Reset()
{
	ResetUpdate();
	currentMove = NoMove;
	targetZlocation = 0.0f;
}

