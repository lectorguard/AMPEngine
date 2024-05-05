#include "MoveBottleToHead.h"

void MoveBottleToHead::Init()
{
	engine->events.subscribeEvent(startMoveBottleToHead, &MoveBottleToHead::StartMoveBottle, this);
	engine->events.subscribeEvent(endMoveBottleToHead, &MoveBottleToHead::End, this);
	engine->events.subscribeEvent(MoveBottleToHeadReset, &MoveBottleToHead::Reset, this);
}

void MoveBottleToHead::StartMoveBottle(amp::DynamicMesh* character)
{
	assert(!IsUpdateActive());
	assert(character);
	assert(bottle);
	hasReachedTarget = false;
	this->character = character;
	AddUpdate();
}

void MoveBottleToHead::Update()
{
	assert(bottle);
	if (bottle->isActive())bottle->setActive(false);
	glm::vec3 targetLocation = character->getLocation() + glm::vec3(0, HeightOverCharacter, 0);
	glm::vec3 currentLocation = bottle->getLocation();
	float maxMoveDistance = speedInMeterPerSecond * engine->getFrameTime();
	glm::vec3 direction = targetLocation - currentLocation;
	if (glm::length(direction) < maxMoveDistance || hasReachedTarget) {
		bottle->setLocation(targetLocation);
		hasReachedTarget = true;
	}
	else {
		direction = glm::normalize(direction);
		glm::vec3 newBottleLocation = currentLocation + maxMoveDistance * direction;
		bottle->setLocation(newBottleLocation);
	}
}

void MoveBottleToHead::End()
{
	RemoveUpdate();
	bottle->setActive(true);
	character = nullptr;
	hasReachedTarget = false;
}

void MoveBottleToHead::Reset()
{
	ResetUpdate();
	bottle->setActive(true);
	character = nullptr;
	hasReachedTarget = false;
}

