#pragma once
#include "TaskController.h"
#include "AMP_Engine.h"
#include "Components/TouchGesture.h"
#include "Components/DynamicMesh.h"
#include "Static/TimeManager.h"
#include "Event/EventSystem.h"

class ThrowingBottle : public amp::Task
{

public:
	amp::TouchGesture * gest = nullptr;
	amp::DynamicMesh* bottle = nullptr;

	ThrowingBottle(amp::TouchGesture* gest) {
		this->gest = gest;
	};
	virtual ~ThrowingBottle() {};

	//EVENTS
	//INPUT
	TEXT ThrowingBottleReset = TEXT("ThrowingBottleReset"); //No Params: RESET has always 0 params
	TEXT startThrowingBottle = TEXT("StartThrowingBottle");	//Two Params: glm::vec3 targetThrowVector, glm::mat4 originThrowBottle
	//OUTPUT
	TEXT onBottleThrow = TEXT("onBottleThrow");				//One Param: amp::DynamicMesh* mesh, glm::vec3 force
	TEXT onInvalidThrow = TEXT("onInvalidThrow");			//No Params

	const float ValidTouchDistance = 20.0f;
	const glm::vec2 bottleDisplayOrigin = glm::vec2(600.0f, 600.0f);
	const glm::vec2 displayButtonOrigin = glm::vec2(600, 600);
	const int displayButtonSizeX = 1200;
	const int displayButtonSizeY = 1200;
	const float maxForceFactor = 40.0f;
	const float minForceFactor = 0.01f;
	const glm::vec3 downForce = glm::vec3(-1.0f, -5.0f, -1.0f);


	virtual void Init() override;

	void StartThrowingBottle(amp::DynamicMesh* bottle, glm::vec3 targetThrowVector,glm::mat4 originThrowBottle);

	void OnInputAction(bool isFirst);

	void CheckThrowMovement(int locX, int locY);

	void OnInputUpdate();

	void EndThrowBottle();

	bool IsValidThrow();

	void ThrowBottle(glm::vec3 force);


	virtual void Reset() override;

private:
	amp::SimpleButton inputButton;
	glm::vec2 startThrowLocation;
	glm::vec2 lastTouch; 
	double startTime = 0.0f;
	bool isValidThrow = true;
	glm::vec3 targetThrowVector;
	glm::mat4 originThrowBottle;

	int throwSoundIndex = -1;
};


