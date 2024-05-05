#include "pch.h"
#include "CollissionCheck.h"
#include "GameUtility.h"

void CollissionCheck::Init()
{
 	engine->events.subscribeEvent(startCollisionCheck, &CollissionCheck::StartCollisionCheck, this);
	engine->events.subscribeEvent(endCollisionCheck, &CollissionCheck::EndCollisionCheck, this);
	engine->events.subscribeEvent(engine->onCollission, &CollissionCheck::OnCollide, this);
	engine->events.subscribeEvent(CollissionCheckReset, &CollissionCheck::Reset, this);


	fallbackSoundIndex = engine->sound.load(TEXT("Sound/S_fallback.wav"));
}

void CollissionCheck::StartCollisionCheck()
{
	isChecking = true;
}

void CollissionCheck::EndCollisionCheck()
{
	isChecking = false;
}

void CollissionCheck::OnCollide(std::pair<rp3d::CollisionBody*, rp3d::CollisionBody*> bodies, std::pair<glm::vec3, glm::vec3> contactPoints, glm::vec3 normal)
{
	if (!isChecking)return;
	glm::vec3 animContact;
	if(!IsCharacterPartOfCollission(bodies, animContact, contactPoints))return;

	//if collission is downside Character is just walking
	if (animContact.y >= bottomOfRigidBody) {
		engine->sound.play(fallbackSoundIndex, 1.0f);
		engine->events.publishEvent(TEXT("addSoundFlags"), (uint8_t)SoundFlags::FALLBACK);
		engine->events.publishEvent(onCollisionObject);
	}
	else {
		engine->events.publishEvent(onCollisionGround);
	}
}

bool CollissionCheck::IsCharacterPartOfCollission(std::pair<rp3d::CollisionBody*, rp3d::CollisionBody*>& bodies, glm::vec3& animContact, std::pair<glm::vec3, glm::vec3>& contactPoints)
{
	if (bodies.first == character->getCollissionBody()){
		animContact = contactPoints.first;
		return true;
	}
	else if (bodies.second == character->getCollissionBody()){
		animContact = contactPoints.second;
		return true;
	}
	else return false;
}

void CollissionCheck::Reset()
{
	isChecking = false;
}

