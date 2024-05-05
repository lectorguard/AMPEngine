#include "pch.h"
#include "Sound/SoundPool.h"
#include "ThrowingBottle.h"

void ThrowingBottle::Init()
{
	engine->events.subscribeEvent(startThrowingBottle, &ThrowingBottle::StartThrowingBottle, this);
	engine->events.subscribeEvent(ThrowingBottleReset, &ThrowingBottle::Reset, this);

	throwSoundIndex = engine->sound.load(TEXT("Sound/S_throw.wav"));
}

void ThrowingBottle::StartThrowingBottle(amp::DynamicMesh* bottle, glm::vec3 targetThrowVector,glm::mat4 originThrowBottle)
{
	if (this->bottle)throw "End was not called before restarting";
	this->bottle = bottle;
	this->targetThrowVector = targetThrowVector;
	this->originThrowBottle = originThrowBottle;
	inputButton = gest->AddButton(amp::SimpleButton(displayButtonOrigin, displayButtonSizeX, displayButtonSizeY),
		[&](bool b) {OnInputAction(b); }, [&]() {OnInputUpdate(); });
}

void ThrowingBottle::OnInputAction(bool isFirst)
{
	if (isFirst) {
		startThrowLocation = glm::vec2(gest->GetX(), gest->GetY());
		lastTouch = startThrowLocation;
		startTime = TIME::GetTime();
	}
	else {
		if (!IsValidThrow()){
			EndThrowBottle();
			engine->events.publishEvent(onInvalidThrow);
			return;
		}
		glm::vec2 endThrow(gest->GetX(), gest->GetY());
		glm::vec2 dir = startThrowLocation - endThrow;
		glm::normalize(dir);

		double deltaTime = TIME::GetTime() - startTime;

		float factor = 5.0 / deltaTime;
		factor = std::min(maxForceFactor, factor);
		factor = std::max(minForceFactor, factor);

		auto force = glm::vec3(dir.y, 1, dir.x) * factor;
		float result = glm::dot(glm::normalize(force), targetThrowVector);
		if (result <= 0) force = force * downForce;
		amp::DynamicMesh* currentBottle = bottle;
		EndThrowBottle();
		engine->sound.play(throwSoundIndex, 1.0f);
		engine->events.publishEvent(onBottleThrow,currentBottle,force);
	}
}

void ThrowingBottle::OnInputUpdate()
{
	//maybe if touch is around the origin start time becomes reset !!!!
	int pressX = gest->GetX();
	int pressY = gest->GetY();
	CheckThrowMovement(pressX, pressY);

	int deltaX = pressX - bottleDisplayOrigin.x;
	int deltaY = pressY - bottleDisplayOrigin.y;

	auto deltaVec = (deltaX / 75.0f) * amp::Camera::getRightView() + (deltaY / 75.0f) * amp::Camera::getUpView();
	auto translationMatrix = glm::translate(glm::mat4(1.0f), deltaVec);
	bottle->setTransform(translationMatrix * originThrowBottle);
}

void ThrowingBottle::CheckThrowMovement(int locX, int locY)
{
	glm::vec2 currentTouch(locX, locY);
	if (glm::distance(lastTouch, currentTouch) < ValidTouchDistance) {
		startTime = TIME::GetTime();
		startThrowLocation = currentTouch;
		isValidThrow = false;
	}
	else isValidThrow = true;
	lastTouch = currentTouch;
}

void ThrowingBottle::EndThrowBottle()
{
	gest->RemoveSimpleButton(inputButton);
	this->bottle = nullptr;
	targetThrowVector = glm::vec3(0, 0, 0);
	originThrowBottle = glm::mat4(1.0f);
}

bool ThrowingBottle::IsValidThrow()
{
	if (!isValidThrow) {
		bottle->setTransform(originThrowBottle);
		return false;
	}
	return true;
}

void ThrowingBottle::ThrowBottle(glm::vec3 force)
{
	bottle->setActive(true);
	bottle->applyForceToCenter(force);
}

void ThrowingBottle::Reset()
{
	gest->TryRemoveSimpleButton(inputButton);
	this->bottle = nullptr;
	targetThrowVector = glm::vec3(0, 0, 0);
	originThrowBottle = glm::mat4(1.0f);
	isValidThrow = true;
}
