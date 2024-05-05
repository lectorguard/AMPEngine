#pragma once
#include "Actor.h"
#include "AMP_Engine.h"
#include "Sound/SoundPool.h"
#include "Actors/ThrowingActor.h"
#include "Components/DynamicMesh.h"
#include "Event/EventSystem.h"
#include "vector"

class GameSound : public amp::Actor
{
public:
	GameSound() {};
	GameSound(const GameSound&) = delete;
	GameSound(GameSound&&) = delete;
	virtual ~GameSound() {};

	const float maxRollVelocity = 12.0f;

	virtual void AfterWindowStartup() override;

	virtual void OnActivate() override;

	virtual void OnDeactivate() override;

	void OnStartRunning();

	void EndDefend();

	void OnPostInit();

	void OnCollide(std::pair<rp3d::CollisionBody*, rp3d::CollisionBody*> bodies, std::pair<glm::vec3, glm::vec3> contactPoints, glm::vec3 normal);

	void PlayRandomHit();

	void PlayRandomRoll();

	bool ContainsCollider(std::vector<amp::DynamicMesh*> container, rp3d::CollisionBody* collider, int& index);

	void AllowHit();

	void AllowRoll();

private:
	int run_sid = -1;

	int hitSoundIndex[4] = { 0 };
	int rollSoundIndex[2] = { 0 };

	int currStreamHit = -1;
	int currStreamRoll = -1;

	bool isHitAllowed = true;
	bool isRollAllowed = true;

	amp::DynamicMesh* R_target = nullptr;
	std::vector<amp::DynamicMesh*> R_attack;
};

