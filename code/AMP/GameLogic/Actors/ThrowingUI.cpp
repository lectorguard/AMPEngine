#include "pch.h"
#include "ThrowingUI.h"
#include "Static/Pixel.h"
#include <cstdlib>

void ThrowingUI::Update()
{
	controller->CallUpdateForTasks();
}

void ThrowingUI::AfterWindowStartup()
{
	one->SetUp(TEXT("Textures/CoronaBottleAlpha.png"), TEXT("Textures/CoronaBottleFrameYellow.png"));
	one->SetLimits(0.92f, 0.079f);
	using namespace amp;
	one->CreateProgressBar(glm::vec2(PIX::XP(0.9), PIX::XP(0.3)), PIX::XP(0.4), PIX::XP(0.6), 1.0f);

	two->SetUp(TEXT("Textures/CoronaBottleAlpha.png"), TEXT("Textures/CoronaBottleFrameRed.png"));
	two->SetLimits(0.92f, 0.079f);
	two->CreateProgressBar(glm::vec2(PIX::XP(0.1f), PIX::XP(0.3f)), PIX::XP(0.4f), PIX::XP(0.6f), 1.0f);

	finishImages->SetUp({ TEXT("Textures/Menu/Searching.png"),TEXT("Textures/black.png"),TEXT("Textures/WIN.png"),TEXT("Textures/WASTED.png"), TEXT("Textures/WaitForPlayer.png") });
	image->SetUp({TEXT("Textures/beerIconAlpha.png")});
	touch->SetUp();

	controller->AddTask<IconTouch>(image, touch);
	controller->AddTask<WinLoose>(finishImages);
	controller->AddTask<DisplayInfos>(finishImages);
	auto* progress = controller->AddTask<ProgressBars>(one, two);
	//Always starts at the beginning of the game !!!!
	progress->Start();

}

void ThrowingUI::BeforeSystemShutdown()
{
	controller->clear();
}

