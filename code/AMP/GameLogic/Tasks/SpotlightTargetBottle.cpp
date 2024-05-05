#include "SpotlightTargetBottle.h"

void SpotlightTargetBottle::Init()
{
	engine->events.subscribeEvent(startSpotlightTargetBottle, &SpotlightTargetBottle::StartSpot, this);
	engine->events.subscribeEvent(endSpotlightTargetBottle, &SpotlightTargetBottle::End, this);
	engine->events.subscribeEvent(SpotlightTargetBottleReset, &SpotlightTargetBottle::Reset, this);
	auto HideTransform = glm::translate(glm::mat4(1.0f), hidingLocation);
	spotlight->setTransform(HideTransform);
}

void SpotlightTargetBottle::StartSpot()
{
	assert(bottle);
	assert(spotlight);
	assert(!IsUpdateActive());
	AddUpdate();
}

void SpotlightTargetBottle::Update()
{
	glm::vec3 loc = bottle->getLocation();
	auto spotLightTransform = glm::translate(glm::mat4(1.0f), glm::vec3(loc.x, height, loc.z));
	spotlight->setTransform(spotLightTransform);
}

void SpotlightTargetBottle::End()
{
	auto HideTransform = glm::translate(glm::mat4(1.0f), hidingLocation);
	spotlight->setTransform(HideTransform);
	RemoveUpdate();
}

void SpotlightTargetBottle::Reset()
{
	auto HideTransform = glm::translate(glm::mat4(1.0f), hidingLocation);
	spotlight->setTransform(HideTransform);
	ResetUpdate();
}

