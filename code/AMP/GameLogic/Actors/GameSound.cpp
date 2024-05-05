#include "GameSound.h"
#include "algorithm"
#include "GameUtility.h"

void GameSound::AfterWindowStartup()
{
	run_sid = engine->sound.load(TEXT("Sound/S_run.wav"));
	//sounds
	hitSoundIndex[0] = engine->sound.load(TEXT("Sound/S_bottlesound1.wav"));
	hitSoundIndex[1] = engine->sound.load(TEXT("Sound/S_bottlesound2.wav"));
	hitSoundIndex[2] = engine->sound.load(TEXT("Sound/S_bottlesound3.wav"));
	hitSoundIndex[3] = engine->sound.load(TEXT("Sound/S_bottlesound4.wav"));

	rollSoundIndex[0] = engine->sound.load(TEXT("Sound/S_BottleRoll1.wav"));
	rollSoundIndex[1] = engine->sound.load(TEXT("Sound/S_BottleRoll2.wav"));

}

void GameSound::OnActivate()
{
	SUB(engine->postInit, GameSound::OnPostInit);
	SUB(engine->onCollission, GameSound::OnCollide);
	SUB(TEXT("onBottleHit"), GameSound::OnStartRunning);
	SUB(TEXT("RunToBottle"), GameSound::OnStartRunning);
	SUB(TEXT("endRound"), GameSound::EndDefend);
}

void GameSound::OnDeactivate()
{
	UnsubscribeEvents();
}

void GameSound::OnStartRunning()
{
	engine->sound.play(run_sid, 1.0f, true);
}

void GameSound::EndDefend()
{
	engine->sound.unloopAndClear(run_sid);
	engine->sound.clearBuffer(currStreamRoll);
}


void GameSound::OnPostInit()
{
	ThrowingActor* thrower =  (ThrowingActor*)engine->getSceneActorByName(TEXT("ThrowingActor"));
	R_target = thrower->R_TargetBottle;
	R_attack = thrower->R_AttackBottles;
	R_attack.push_back(thrower->R_ThrowBottle);
	
}

void GameSound::OnCollide(std::pair<rp3d::CollisionBody*, rp3d::CollisionBody*> bodies, std::pair<glm::vec3, glm::vec3> contactPoints, glm::vec3 normal)
{
	if (!R_target)return;
	if (contactPoints.first.y == 0.0f || contactPoints.second.y == 0.0f) {
		int index = -1;
		if (ContainsCollider(R_attack, bodies.first,index) || ContainsCollider(R_attack,bodies.second,index)) {
			if (index < 0)return;
			if (glm::length(R_attack[index]->getAngularVelocity()) > maxRollVelocity)PlayRandomHit();
			else PlayRandomRoll();
		}
		else if (ContainsCollider(R_attack, R_target->getCollissionBody(), index)) {
			PlayRandomHit();
		}
	}
}

void GameSound::PlayRandomHit()
{
	int sound = hitSoundIndex[std::rand() % 4];
	currStreamHit = engine->sound.play(sound, 1.0f);
	engine->events.publishEvent(TEXT("addSoundFlags"), (uint8_t)SoundFlags::HIT);
}

void GameSound::PlayRandomRoll()
{
	if (isRollAllowed) {
		int sound = rollSoundIndex[std::rand() % 2];
		currStreamRoll = engine->sound.play(sound, 1.0f);
		engine->events.publishEvent(TEXT("addSoundFlags"), (uint8_t)SoundFlags::ROLL);
		isRollAllowed = false;
		amp::TimeEvent ev;
		ev.init(2, false, &GameSound::AllowRoll, this);
		engine->addTimerFunction(ev);
	}
}

bool GameSound::ContainsCollider(std::vector<amp::DynamicMesh*> container, rp3d::CollisionBody* collider, int& index)
{
	int counter = 0;
	for (amp::DynamicMesh* dyn : container) {
		if (dyn->getCollissionBody() == collider) {
			index = counter;
			return true;
		}
		++counter;
	}
	return false;
}

void GameSound::AllowHit()
{
	isHitAllowed = true;
	currStreamHit = -1;
}

void GameSound::AllowRoll()
{
	isRollAllowed = true;
	currStreamRoll = -1;
}

