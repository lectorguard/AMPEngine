#pragma once
#include "TaskController.h"
#include "AMP_Engine.h"
#include "Components/DynamicMesh.h"

class CharacterControl : public amp::Task
{
public:
	amp::DynamicMesh * character;
	CharacterControl(amp::DynamicMesh* character) {
		this->character = character;
	}
	virtual ~CharacterControl() {};
	CharacterControl(const CharacterControl&) = delete;
	CharacterControl(CharacterControl&&) = delete;

	//EVENTS
	//INPUT

	TEXT CharacterControlReset = TEXT("CharacterControlReset");			//No Params: RESET has always 0 params
	TEXT startVelocityCheck = TEXT("startVelocityCheck");				//Two Params: bool isLaneChangeActive, bool isJumpActive
	TEXT endVelocityCheck = TEXT("endVelocityCheck");					//No Params

	TEXT startFixZLocation = TEXT("startFixZLocation");					//One Param: float Zlocation
	TEXT endFixZLocation = TEXT("endFixZLocation");						//No Params
	TEXT setIsFixZLocation = TEXT("setIsFixZlocation");					//One Param: bool setIsZLocationChecked

	TEXT startRelocateWhenFalling = TEXT("startRelocateWhenFalling");	//One Param: float Height of Trigger
	TEXT endRelocateWhenFalling = TEXT("endRelocateWhenFalling");		//NoParams
	

	TEXT startFixOrientation = TEXT("startFixOrientation");				//One Param: bool isForward
	TEXT endFixOrientation = TEXT("endFixOrientation");					//No Params

	//OUTPUTS
	TEXT onCharacterRelocate = TEXT("onCharacterRelocate");				//NoParams


	//Control Variables
	const float MaxLaneChangeVelocity = 55.0f;
	const float MaxJumpVelocity = 75.0f;


	virtual void Init() override;

	void StartVelocityCheck(bool laneChange, bool jump);

	void StartFixZLocation(float Zlocation);

	void SetIsZLocationFixed(bool isChecked);

	void StartFixOrientation(bool isForward);

	void StartRelocateWhenFalling(float YHeight);

	void UpdateZLocation();

	void SetRunningTransform();

	virtual void Update() override;

	void HeightRespawn();

	void UpdateVelocity();

	void EndVelocity();

	void EndFixZLocation();

	void EndRelocateWhenFalling();

	void EndFixOrientation();


	virtual void Reset() override;

private:

	bool isChangeVelocity = false;
	bool isJumpVelocity = false;
	bool isZFix = false; 
	bool isRelocating = false;
	bool isFixedOrientation = false;
	bool isForward = true;
	float Zlocation = 0.0f;
	
	float triggerHeight = -40.0f;
	

};

