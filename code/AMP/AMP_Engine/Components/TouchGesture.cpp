#include "pch.h"
#include "TouchGesture.h"

void amp::TouchGesture::SetUp()
{
	// Dummy function to make user clear he has to set up components 
}

void amp::TouchGesture::AddButton(Button button, std::function<void()> cb)
{
	button.buttonTouchCallback = cb;
	ToAndroidCoords(button);
	buttonVec.push_back(button);
}

amp::SimpleButton amp::TouchGesture::AddButton(SimpleButton button, std::function<void(bool)> StartEndCallback, std::function<void()> TouchUpdateCallback /*= nullptr*/)
{
	button.simpleButtonTouchCallback = StartEndCallback;
	button.touchUpdateCallback = TouchUpdateCallback;
	ToAndroidCoords(button);
	simpleButtonList.push_back(button);
	button.itr = --simpleButtonList.end();
	return button;
}

void amp::TouchGesture::AddButton(MultiButton button, std::function<void()> cb)
{
	AddButton(button.button, cb);
}



void amp::TouchGesture::RemoveSimpleButton(SimpleButton button)
{
	//Could crash when remove is called multiple times for same button
	if (!isSimpleButtonNull) {
		if (lastSimpleButton->id == button.id)isSimpleButtonNull = true;
	}
	//just when Debugging
	if(IsItrValid(button.itr, simpleButtonList))
		simpleButtonList.erase(button.itr);
}

void amp::TouchGesture::TryRemoveSimpleButton(SimpleButton button)
{
	simpleButtonList.erase(std::remove_if(simpleButtonList.begin(), simpleButtonList.end(), [&](auto A) {return A.id == button.id; }), simpleButtonList.end());
}

void amp::TouchGesture::RemoveAllSimpleButtons()
{
	simpleButtonList.clear();
}

void amp::TouchGesture::RemoveButton(Button button)
{
	if (!buttonVec.size())return;
	buttonVec.erase(
		std::remove_if(buttonVec.begin(), buttonVec.end(),

			[&](Button elem) {return elem.id == button.id; }), buttonVec.end());
}

void amp::TouchGesture::RemoveButton(MultiButton button)
{
	RemoveButton(button.button);
}

int amp::TouchGesture::GetTouchedIndex(MultiButton textInfo)
{
	int ampY = engine->ampContext.getHeight() - touchY;
	for (int i = 0; i < textInfo.YValues.size(); ++i) {
		int max = textInfo.YValues[i].first;
		int min = textInfo.YValues[i].second;
		if (ampY >= min && ampY <= max) {
			return i;
		}
	}
	return -1;
}

void amp::TouchGesture::OnActivate()
{
	startTouch = engine->events.subscribeEvent(engine->startTouch, &TouchGesture::OnStartTouch, this);
	touchUpdate = engine->events.subscribeEvent(engine->touchUpdate, &TouchGesture::OnTouch, this);
	endTouch = engine->events.subscribeEvent(engine->endTouch, &TouchGesture::OnEndTouch, this);
}

void amp::TouchGesture::OnDeactivate()
{
	engine->events.unsubscribeEvent(engine->startTouch, startTouch);
	engine->events.unsubscribeEvent(engine->touchUpdate, touchUpdate);
	engine->events.unsubscribeEvent(engine->endTouch, endTouch);
}

void amp::TouchGesture::ResetButtonTimeOut()
{
	isButtonTimeOut = false;
}

void amp::TouchGesture::ToAndroidCoords(Button& button)
{
	int DispX = button.location.x;
	int DispY = engine->ampContext.getHeight() - button.location.y;
	int HalfSizeX = button.rectX / 2.0f;
	int HalfSizeY = button.rectY / 2.0f;
	button.minX = DispX - HalfSizeX;
	button.maxX = DispX + HalfSizeX;
	button.minY = DispY - HalfSizeY;
	button.maxY = DispY + HalfSizeY;
}

void amp::TouchGesture::initOnTouch()
{
	if (!engine)throw "Engine was not initialized correctly for Touch Gesture";
	
}

void amp::TouchGesture::OnStartTouch(int32_t x, int32_t y)
{
	SetTouch(x, y);
	CheckButtons(x, y);
	CheckSimpleButton(x, y);
}

void amp::TouchGesture::OnTouch(int32_t x, int32_t y)
{
	SetTouch(x, y);
	CheckButtons(x, y);
	CheckSimpleButton(x, y);
}

void amp::TouchGesture::OnEndTouch(int32_t x, int32_t y)
{
	SetTouch(x, y);
	ResetCurrentSimpleButton();
	//Not check buttons here !!!!!!!!!!
	//Just for Gestures Later
}

void amp::TouchGesture::CheckButtons(int32_t x, int32_t y)
{
	if (!engine)throw "Engine was not initialized for Touch Gesture";
	if (isButtonTimeOut)return;
	for(auto& b : buttonVec){
		if (isPressed(b)) {
			b.buttonTouchCallback();
			SetTimeOut();
			return;
		}
	}
}

void amp::TouchGesture::SetTouch(int32_t x, int32_t y)
{
	touchX = x;
	touchY = y;
}

bool amp::TouchGesture::isPressed(Button& b)
{
	return touchX <= b.maxX && touchX >= b.minX && touchY <= b.maxY && touchY >= b.minY;
}

void amp::TouchGesture::CheckSimpleButton(int32_t x, int32_t y)
{
	auto itr = simpleButtonList.begin();
	auto endItr = simpleButtonList.end();
	for (; itr != endItr; ++itr) {
		if (isPressed(*itr)) {
			if (!isSimpleButtonNull) {
				if (itr == lastSimpleButton){
					if (lastSimpleButton->touchUpdateCallback)lastSimpleButton->touchUpdateCallback();
					return;
				}
				else lastSimpleButton->simpleButtonTouchCallback(false);
			}
			lastSimpleButton = itr;
			itr->simpleButtonTouchCallback(true);
			isSimpleButtonNull = false;
			return;
		}
	}
	ResetCurrentSimpleButton();
}

void amp::TouchGesture::ResetCurrentSimpleButton()
{
	if (!isSimpleButtonNull && simpleButtonList.size() > 0)lastSimpleButton->simpleButtonTouchCallback(false);
	isSimpleButtonNull = true;
}

void amp::TouchGesture::SetTimeOut()
{
	amp::TimeEvent timeEv;
	timeEv.init<TouchGesture>(buttonTimeOutInSec, false, &TouchGesture::ResetButtonTimeOut, this);
	engine->addTimerFunction(timeEv);
	isButtonTimeOut = true;
}

