#pragma once
#include "TaskController.h"
#include "AMP_Engine.h"
#include "Components/TouchGesture.h"
#include "Event/EventSystem.h"


enum Move {
	Right = 0, Left = 1, Forward = 2, NoMove = 3
};

class CharacterTouch : public amp::Task
{

public:
	amp::TouchGesture* gest = nullptr;
	
	CharacterTouch(amp::TouchGesture* gest) {
		this->gest = gest;
	};
	virtual ~CharacterTouch() {};

	const float minPixelDistanceSquared = 1000.0f;

	//EVENTS
	//INPUT
	TEXT CharacterTouchReset = TEXT("CharacterTouchReset"); //No Params: RESET has always 0 params
	TEXT startCharacterTouch = TEXT("StartCheckingCharacterTouch");		//No Params
	//OUTPUT ///1 = CalledOnce , 0 = Called update, k = called k times
	TEXT onDirectionCalculated = TEXT("onDirectionCalculated");			//1 One Param: Move (enum)  
	TEXT endTouch = TEXT("endTouchGest");								//1 No Param
	

	void OnInputAction(bool isFirst);

	void TouchEnded();

	void OnInputUpdate();
	Move GetTouchDirection(glm::vec2 dir);

	virtual void Init() override;
	virtual void Start() override;
	void EndTouch(Move m);


	virtual void Reset() override;

private:
	amp::SimpleButton inputButton;
	glm::vec2 firstTouchLocation;
	bool isDirectionCalculated = false;
	
};

