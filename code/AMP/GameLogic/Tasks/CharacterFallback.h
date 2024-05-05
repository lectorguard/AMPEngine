#pragma once
#include "TaskController.h"
#include "AMP_Engine.h"
#include "Components/DynamicMesh.h"

class CharacterFallback : public amp::Task
{

public:
	amp::DynamicMesh * character = nullptr;
	CharacterFallback(amp::DynamicMesh* character) {
		this->character = character;
	};
	virtual ~CharacterFallback() {};

	CharacterFallback(const CharacterFallback&) = delete;
	CharacterFallback(CharacterFallback&&) = delete;


	//EVENTS
	//INPUTS
	TEXT CharacterFallbackReset = TEXT("CharacterFallbackReset"); //No Params: RESET has always 0 params
	TEXT updateFallbackCheck = TEXT("updateFallbackCheck"); //Two Params:  bool isForward, float xValue
	TEXT resetFallbackCheck = TEXT("resetFallbackCheck"); //No Params

	virtual void Init() override;

	void UpdateFallbackCheck(bool isForward, float currXFallback);

	virtual void Update() override;

	void SetFallbackLocation();

	void ResetFallbackCheck();


	virtual void Reset() override;

private:
	bool isForward = true;
	float currXFallback = 0.0f;

};

