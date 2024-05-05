#pragma once
#include "TaskController.h"
#include "Components/DynamicMesh.h"

class RotatingTask : public amp::Task
{
public:

	amp::DynamicMesh* dynMesh = nullptr;
	RotatingTask(amp::DynamicMesh* mesh) {
		dynMesh = mesh;
	};
	virtual ~RotatingTask() {};

	const float speed = 1.2f;
	

	//INPUT
	TEXT RotatingTaskReset = TEXT("RotatingTaskReset");	//No Params: RESET has always 0 params
	TEXT startRotate = TEXT("startRotate");			//NoParams
	TEXT onRotateFinish = TEXT("onRotateFinished");	//NoParams
	virtual void Init() override;


	void StartRotating(glm::vec3 targetRotationEuler);


	virtual void Update() override;


	virtual void End() override;


	virtual void Reset() override;

private:
	glm::quat startRotation;
	glm::quat targetRotation;
	glm::vec3 position;
	float rotation = 0;

};

