#include "pch.h"
#include "Graphics/GLHelper.h"
#include "cstdlib"
#include "algorithm"
#include "PostProcessTask.h"

void PostProcessTask::Init()
{
	engine->events.subscribeEvent(PostProcessTaskReset, &PostProcessTask::Reset, this);
	engine->events.subscribeEvent(startPostProcess, &PostProcessTask::StartPostProcess, this);
	engine->events.subscribeEvent(endPostProcess, &PostProcessTask::EndPostProcess, this);

	engine->events.subscribeEvent(TEXT("onChangingProgress"), &PostProcessTask::OnChangingProgress, this);
	//Missing current drunk level event
}

void PostProcessTask::StartPostProcess()
{
	engine->frameBuffer.setDrawCallback([&](unsigned int s) {OnDrawFrameBufferScene(s); });
	engine->frameBuffer.setShadingType(amp::CUSTOM);
}

void PostProcessTask::EndPostProcess()
{
	engine->frameBuffer.resetDrawCallback();
	engine->frameBuffer.setShadingType(amp::DEFAULT);
}

void PostProcessTask::OnDrawFrameBufferScene(unsigned int shaderProgram)
{
	float rotationSpeed = engine->getFrameTime() * blurRotationSpeed;
	currentBlurDirection = RotateVec2(currentBlurDirection, glm::radians(rotationSpeed));
	auto appliedDirection = currentBlurDirection * maxBlurVectorLength * drunkLevel;

	glm::vec2 resDelta = glm::vec2(0, 0);
	if (moveRange != 0.0f)
	{
		resDelta = RotateVec2(deltaVal, glm::radians(engine->getFrameTime() * CameraMoveSpeed));
		resDelta = resDelta * moveRange;
		deltaVal = resDelta;
	
		resDelta[0] = std::min(resDelta[0], moveRange);
		resDelta[0] = std::max(resDelta[0], -moveRange);
		resDelta[1] = std::min(resDelta[1], moveRange);
		resDelta[1] = std::max(resDelta[1], -moveRange);
	}
	auto resMat = CreateMat2Transform(glm::radians(0.0f),zoom);

	GLHelper::SetUniform2DMatrix(shaderProgram, "transform", resMat);
	GLHelper::SetUniformVector2(shaderProgram, "delta", resDelta);
	GLHelper::SetUniformVector2(shaderProgram, "direction", appliedDirection);
	GLHelper::SetUniformVector2(shaderProgram, "resolution", glm::vec2(engine->ampContext.getHeight(), engine->ampContext.getWidth()));
}

glm::mat2 PostProcessTask::CreateMat2Transform(float rotInRad, float scale)
{
	glm::mat2 result; 
	result[0][0] = glm::cos(rotInRad) * scale;
	result[0][1] = -glm::sin(rotInRad);
	result[1][0] = glm::sin(rotInRad);
	result[1][1] = glm::cos(rotInRad) * scale;
	return result;
}

glm::vec2 PostProcessTask::RotateVec2(glm::vec2 vec, float radians, bool normalize /*= true*/)
{
	auto rotatedVector = glm::vec2(vec.x * glm::cos(radians) - vec.y * glm::sin(radians), vec.x * glm::sin(radians) + vec.y * glm::cos(radians));
	return normalize ? glm::normalize(rotatedVector): rotatedVector;
}

bool PostProcessTask::getRand(float min /*= 0.0f*/, float max /*= 1.0f*/)
{
	float delta = max - min;
	return ((double)std::rand() / (RAND_MAX)) * delta + min;
}

void PostProcessTask::Reset()
{
	EndPostProcess();
	drunkLevel = 0.0f;
	zoom = 1.0f;
	moveRange = 0.0f;
	deltaVal = glm::vec2(0.1f, 0.1f);
	currentBlurDirection = glm::vec2(1, 0);
}

void PostProcessTask::OnChangingProgress(Player player, float currProgress)
{
	if (player == One) {
		drunkLevel = 1.0f - currProgress;
		zoom = 1.0f + drunkLevel * 0.1f;
		moveRange = drunkLevel * 0.1f;
	}
}
