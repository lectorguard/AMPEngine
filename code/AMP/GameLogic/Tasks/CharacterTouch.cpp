#include "pch.h"
#include "CharacterTouch.h"


void CharacterTouch::Init()
{
	engine->events.subscribeEvent(startCharacterTouch, &CharacterTouch::Start, this);
	engine->events.subscribeEvent(CharacterTouchReset, &CharacterTouch::Reset, this);
}

void CharacterTouch::Start()
{
	assert(!isDirectionCalculated);
	inputButton = gest->AddButton(amp::SimpleButton(glm::vec2(600, 600), 1200, 1200), [&](bool b) {OnInputAction(b); }, [&]() {OnInputUpdate(); });
}

void CharacterTouch::OnInputAction(bool isFirst)
{
	if (isFirst) {
		firstTouchLocation = gest->GetTouch();
	}
	else {
		TouchEnded();
	}
}

void CharacterTouch::TouchEnded()
{
	gest->RemoveSimpleButton(inputButton);
	//No Direction was ever calculated, so for consistency call NoMove
	if (!isDirectionCalculated)EndTouch(Move::NoMove);
	isDirectionCalculated = false;
	engine->events.publishEvent(endTouch);
}

void CharacterTouch::OnInputUpdate()
{
	auto direction = gest->GetTouch() - firstTouchLocation;
	float distanceSquared = glm::dot(direction, direction);
	if (distanceSquared > minPixelDistanceSquared && !isDirectionCalculated) {
		Move move = GetTouchDirection(direction);
		isDirectionCalculated = true;
		EndTouch(move);
	}
}

void CharacterTouch::EndTouch(Move m)
{
	engine->events.publishEvent(onDirectionCalculated,m);
}

Move CharacterTouch::GetTouchDirection(glm::vec2 dir)
{
	//---------------------------Right--------------Left----------Up----------
	glm::vec2 Directions[3] = { glm::vec2(1,0), glm::vec2(-1,0),glm::vec2(0,1) };
	for (int i = 0; i < 3; ++i) {
		if (glm::dot(glm::normalize(dir), Directions[i]) > 0.5f) {
			return Move(i);
		}
	}
	return Move::NoMove;
}

void CharacterTouch::Reset()
{
	gest->TryRemoveSimpleButton(inputButton);
	isDirectionCalculated = false;
}

