#include "pch.h"
#include "Static/Pixel.h"
#include "cassert"
#include "Sound/SoundPool.h"
#include "cstdlib"
#include "WinLoose.h"

void WinLoose::Init()
{
	engine->events.subscribeEvent(winLooseReset, &WinLoose::Reset, this);
	engine->events.subscribeEvent(startDisplay, &WinLoose::StartDisplay, this);
	engine->events.subscribeEvent(endDisplay, &WinLoose::EndDisplay, this);
	engine->events.subscribeEvent(startLoading, &WinLoose::StartLoadingScreen, this);

	winLoseSoundIndex[0] = engine->sound.load(TEXT("Sound/S_vomit1.wav"));
	winLoseSoundIndex[1] = engine->sound.load(TEXT("Sound/S_vomit2.wav"));
	winLoseSoundIndex[2] = engine->sound.load(TEXT("Sound/S_vomit3.wav"));
}

void WinLoose::StartDisplay(Game game)
{
	assert(!isDisplaying);
	int sIndex = winLoseSoundIndex[std::rand() % 3];
	engine->sound.play(sIndex, 1.0f);
	switch (game)
	{
	case Win:
		images->RenderImage(2, amp::PIX::P(P_location), amp::PIX::XP(P_SizeUniformFromX), amp::PIX::XP(P_SizeUniformFromX));
		break;
	case Loose:
		images->RenderImage(3, amp::PIX::P(P_location), amp::PIX::XP(P_SizeUniformFromX), amp::PIX::XP(P_SizeUniformFromX));
		break;
	}
	isDisplaying = true;
}

void WinLoose::StartLoadingScreen()
{
	images->RenderImage(1, amp::PIX::P(glm::vec2(0.5, 0.5)), amp::PIX::X(), amp::PIX::Y());
	images->RenderImage(0, amp::PIX::P(glm::vec2(0.5, 0.5)), amp::PIX::XP(1), 1024);
	engine->events.publishEvent(TEXT("PostProcessTaskReset"));
}

void WinLoose::EndDisplay()
{
	images->RemoveAllImages();
	isDisplaying = false;
}

void WinLoose::Reset()
{
	EndDisplay();
}
