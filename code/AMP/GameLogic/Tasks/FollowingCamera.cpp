#include "pch.h"
#include "FollowingCamera.h"

void FollowingCamera::Init()
{
	engine->events.subscribeEvent(startFollowingCamera, &FollowingCamera::StartFollowingCamera, this);
	engine->events.subscribeEvent(stopFollowingCamera, &FollowingCamera::End, this);
	engine->events.subscribeEvent(FollowingCameraReset, &FollowingCamera::Reset, this);
}

void FollowingCamera::StartFollowingCamera(glm::mat4 deltaTransform, std::optional<float> fixY /*= std::nullopt*/)
{
	originCameraTransform = deltaTransform;
	yValue = fixY;
	AddUpdate();
}

void FollowingCamera::Update()
{
	auto result = glm::scale(targetComponent->getTransform(), 1.0f / targetComponent->getScale()) * originCameraTransform;
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(result, scale, rotation, translation, skew, perspective);
	amp::Camera::resetView();
	if (yValue == std::nullopt)amp::Camera::setViewLocation(translation);
	else amp::Camera::setViewLocation(glm::vec3(translation.x, *yValue, translation.z));
	amp::Camera::rotateView(rotation);
}

void FollowingCamera::End()
{
	RemoveUpdate();
	yValue = std::nullopt;
	originCameraTransform = glm::mat4(1.0f);
}

void FollowingCamera::Reset()
{
	ResetUpdate();
	yValue = std::nullopt;
	originCameraTransform = glm::mat4(1.0f);
}
