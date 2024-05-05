#pragma once
#include "TaskController.h"
#include "AMP_Engine.h"
#include "Components/DynamicMesh.h"
#include "Static/TimeManager.h"
#include "vector"
#include "limits"
#include "Utility/TaskVector.h"

#define  YChecking			 1
#define  TimeChecking		 2
#define  VelocityChecking	 4

enum InvalidType {
	Velocity, YLocation, Time
};

struct BottleCheckInfo {

	BottleCheckInfo(amp::DynamicMesh* mesh) : bottle(mesh){};
	amp::DynamicMesh* bottle = nullptr;
	float maxThrowVelocity = 0.0f;
	double timeAfterThrow = -1.0f;
	unsigned char flags = YChecking | TimeChecking | VelocityChecking;

	bool operator== (const BottleCheckInfo& rhs) {
		return this->bottle == rhs.bottle;
	}
};

class BottleValidityCheck : public amp::Task
{
	public:
	BottleValidityCheck() {};
	virtual ~BottleValidityCheck() {};

	BottleValidityCheck(const BottleValidityCheck&) = delete;
	BottleValidityCheck(BottleValidityCheck&&) = delete;


	//EVENTS
	//INPUTS
	TEXT BottleValidityCheckReset = TEXT("BottleValidityCheckReset"); //No Params: RESET has always 0 params
	TEXT startBottleValidityCheck = TEXT("startBottleValidityCheck");	//One Param: amp::DynamicMesh* mesh
	TEXT stopBottleValidityCheck = TEXT("stopBottleValidityCheck");		//One Param: amp::DynamicMesh* mesh
	//OUTPUT
	TEXT bottleInvalid = TEXT("bottleInvalid");							//Two Params: amp::DynamicMesh* mesh, InvalidType type	(enum defined here)		

	const double resetTimeInSeconds = 6.0;
	const float resetYLocation = -1.0f;
	const float leastVelocity = 3.0f;
		
	virtual void Init() override;

	//0 = YCHeck, 1 = TimeCheck, 2 = VelocityCheck
	void StartValidityCheck(amp::DynamicMesh* mesh, const unsigned char flags);


	virtual void Update() override;

	void Checking(BottleCheckInfo info, int index);

	bool isFlagged(BottleCheckInfo& info, unsigned char flag);

	void TimeCheck(BottleCheckInfo info);

	void YCheck(BottleCheckInfo info);

	void VelocityCheck(BottleCheckInfo info);

	void BottleCheckEnd(amp::DynamicMesh* mesh);


	virtual void Reset() override;

private:
	TaskVector<BottleCheckInfo> bottles;
};

