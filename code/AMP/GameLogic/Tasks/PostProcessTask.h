#pragma once
#include "TaskController.h"
#include "AMP_Engine.h"
#include "Tasks/ProgressBars.h"
#include "Components/DynamicMesh.h"

class PostProcessTask : public amp::Task
{
public:
	PostProcessTask() {
	}
	virtual ~PostProcessTask() {};
	PostProcessTask(const PostProcessTask&) = delete;
	PostProcessTask(PostProcessTask&&) = delete;

	//EVENTS
	//INPUT

	TEXT PostProcessTaskReset = TEXT("PostProcessTaskReset");		//No Params: RESET has always 0 params
	TEXT startPostProcess = TEXT("startPostProcess");				//Two Params: bool isLaneChangeActive, bool isJumpActive
	TEXT endPostProcess = TEXT("endPostProcess");					//No Params
	
	//drink Inputs
	TEXT onPlayerDrink = TEXT("onPlayerDrink");						//One Param: float drinkingLevel, Player player

	const float blurRotationSpeed = 12.0f;
	const float maxBlurVectorLength = 30.0f;
	const float CameraMoveSpeed = 15.0f;


	virtual void Init() override;

	void StartPostProcess();

	void EndPostProcess();

	void OnDrawFrameBufferScene(unsigned int shaderProgram);

	glm::mat2 CreateMat2Transform(float rotInRad, float scale);

	glm::vec2 RotateVec2(glm::vec2 vec, float radians, bool normalize = true);

	bool getRand(float min = 0.0f, float max = 1.0f);

	virtual void Reset() override;

	void OnChangingProgress(Player player, float currProgress);

private:
	float zoom = 1.0f;
	float moveRange = 0.0f;
	glm::vec2 deltaVal = glm::vec2(0.1f, 0.1f);

	glm::vec2 currentBlurDirection = glm::vec2(1, 0);
	float drunkLevel = 0.0f;
};