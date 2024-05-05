#pragma once
#include "TaskController.h"
#include "AMP_Engine.h"
#include "Components/DynamicMesh.h"

class CollissionCheck : public amp::Task
{

public:
	amp::DynamicMesh * character = nullptr;
	CollissionCheck(amp::DynamicMesh* character) {
		this->character = character;
	};
	virtual ~CollissionCheck() {};

	CollissionCheck(const CollissionCheck&) = delete;
	CollissionCheck(CollissionCheck&&) = delete;


	//EVENTS
	//INPUTS
	TEXT CollissionCheckReset = TEXT("CollissionCheckReset"); //No Params: RESET has always 0 params
	TEXT startCollisionCheck = TEXT("startCollisionCheck"); //No Param
	TEXT endCollisionCheck = TEXT("endCollisionCheck");		//No Param
	//OUTPUTS
	TEXT onCollisionGround = TEXT("onCollisionGround");	//NoParam
	TEXT onCollisionObject = TEXT("onCollisionObject");	//NoParam

	//Bottom of Rigidbody from center + margin
	const float bottomOfRigidBody = -5.5f;

	virtual void Init() override;

	void StartCollisionCheck();

	void EndCollisionCheck();

	void OnCollide(std::pair<rp3d::CollisionBody*, rp3d::CollisionBody*> bodies, std::pair<glm::vec3, glm::vec3> contactPoints, glm::vec3 normal);

	bool IsCharacterPartOfCollission(std::pair<rp3d::CollisionBody*, rp3d::CollisionBody*>& bodies, glm::vec3& animContact, std::pair<glm::vec3, glm::vec3>& contactPoints);


	virtual void Reset() override;

private:
	bool isChecking = false;

	int fallbackSoundIndex = -1;
};

