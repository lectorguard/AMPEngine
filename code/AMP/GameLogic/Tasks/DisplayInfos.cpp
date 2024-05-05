#include "Static/Pixel.h"
#include "cassert"
#include "DisplayInfos.h"

void DisplayInfos::Init()
{
	engine->events.subscribeEvent(DisplayInfosReset, &DisplayInfos::Reset, this);
	engine->events.subscribeEvent(startDisplayWaitForPlayers, &DisplayInfos::StartWaitForPlayers, this);
	engine->events.subscribeEvent(endDisplay, &DisplayInfos::EndDisplay, this);
	engine->events.subscribeEvent(TEXT("NetworkingIsRunning"), &DisplayInfos::StopDisplayWaitForPlayers, this);
	//StartWaiting
	engine->events.subscribeEvent(TEXT("startSendData"), &DisplayInfos::StartWaitForPlayers, this);
	engine->events.subscribeEvent(TEXT("startReceiveData"), &DisplayInfos::StartWaitForPlayers, this);
}

void DisplayInfos::StartWaitForPlayers(std::string ipOther)
{
	assert(!isDisplayingWaitForPlayers);
	waitForPlayersImage = images->RenderImage(4, amp::PIX::P(P_location), amp::PIX::XP(P_SizeX), amp::PIX::XP(P_SizeX));
	isDisplayingWaitForPlayers = true;
}

void DisplayInfos::StopDisplayWaitForPlayers()
{
	assert(isDisplayingWaitForPlayers);
	images->RemoveImage(waitForPlayersImage);
	isDisplayingWaitForPlayers = true;
}

void DisplayInfos::EndDisplay()
{
	Reset();
}

void DisplayInfos::Reset()
{
	images->RemoveAllImages();
	isDisplayingWaitForPlayers = false;
}

