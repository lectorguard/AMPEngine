#include "TaskController.h"
#include "AMP_Engine.h"
#include "Components/DynamicMesh.h"

class BottleHitCheck : public amp::Task
{

public:
	amp::DynamicMesh * bottle = nullptr;
	BottleHitCheck(amp::DynamicMesh* bottle) {
		this->bottle = bottle;
	};
	virtual ~BottleHitCheck() {};

	BottleHitCheck(const BottleHitCheck&) = delete;
	BottleHitCheck(BottleHitCheck&&) = delete;

	const float YHitHeight = 4.1f;
	const float MaxBottleForce = 50.0f;

	//EVENTS
	//INPUTS
	TEXT BottleHitCheckReset = TEXT("BottleHitCheckReset");		//No Params: RESET has always 0 params
	TEXT startBottleHitCheck = TEXT("startBottleHitCheck");   //No Param
	TEXT endBottleHitCheck = TEXT("endBottleHitCheck");		  //No Param
	//OUTPUT
	TEXT onBottleHit = TEXT("onBottleHit");					  //No Params

	virtual void Init() override;

	void StartHitCheck();

	virtual void Update() override;

	virtual void End() override;

	virtual void Reset() override;

};