#include "pch.h"
#include "RotatingTask.h"

void RotatingTask::Init()
{
	engine->events.subscribeEvent(startRotate,&RotatingTask::StartRotating, this);
	engine->events.subscribeEvent(RotatingTaskReset, &RotatingTask::Reset, this);
}

void RotatingTask::StartRotating(glm::vec3 targetRotationEuler)
{
	startRotation = dynMesh->getRotation();
	position = dynMesh->getLocation();
	targetRotation = glm::quat(targetRotationEuler);
	AddUpdate();
}

void RotatingTask::Update()
{
	//float factor = (glm::dot(dynMesh->getRotation(), targetRotation) + 1.0f) / 2.0f;
	rotation += engine->getFrameTime() * speed;
	if (rotation > 1.0f)rotation = 1;
	auto newRot = glm::lerp(startRotation, targetRotation, rotation);
	dynMesh->setRotation(glm::normalize(newRot));
	dynMesh->setLocation(position);
	if (rotation == 1.0f) {
		End();
	}
}

void RotatingTask::End()
{
	RemoveUpdate();
	rotation = 0.0f;
	targetRotation = glm::vec3(0.0f);
	engine->events.publishEvent(onRotateFinish);
}

void RotatingTask::Reset()
{
	ResetUpdate();
	rotation = 0.0f;
	targetRotation = glm::vec3(0.0f);
}
