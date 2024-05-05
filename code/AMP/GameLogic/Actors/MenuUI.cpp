#include "MenuUI.h"


void MenuUI::Update()
{

}

void MenuUI::AfterWindowStartup() 
{
	image->SetUp({TEXT("Textures/Menu/OnlineGameButton.png"),TEXT("Textures/Menu/TutorialButton.png"),TEXT("Textures/Menu/Header_main_bg.png"),TEXT("Textures/menu_bg.png"),TEXT("Textures/Menu/Searching.png")});
	auto onlineImg = image->RenderImage(0, amp::PIX::P(glm::vec2(0.5, 0.5)) + glm::vec2(0, amp::PIX::CM(2)), amp::PIX::CM(5), amp::PIX::CM(1.25));
	auto tutImg = image->RenderImage(1, amp::PIX::P(glm::vec2(0.5, 0.5)) + glm::vec2(0, amp::PIX::CM(0.25)), amp::PIX::CM(5), amp::PIX::CM(1.25));
	image->RenderImage(2, glm::vec2(amp::PIX::XP(0.5),amp::PIX::Y() - amp::PIX::XP(0.2f)),amp::PIX::X(), amp::PIX::XP(0.25f));
	image->RenderImage(3, glm::vec2(amp::PIX::XP(0.5),amp::PIX::XP(0.5)), amp::PIX::X(), amp::PIX::X());
	onlineBut = touch->AddButton(amp::SimpleButton(onlineImg), [&](auto s) {OnOnlineGame(s); });
	tutorialBut = touch->AddButton(amp::SimpleButton(tutImg), [&](auto s) {OnTutorialGame(s); });

	int titleSoundIndex = engine->sound.load(TEXT("Sound/S_Titlesound.wav"));
	buttonSound = engine->sound.load(TEXT("Sound/S_button.wav"));
	engine->sound.play(titleSoundIndex, 1.0f, true);
}

void MenuUI::BeforeSystemShutdown()
{


}

void MenuUI::OnOnlineGame(bool isFirst)
{
	if (isFirst)
	{
		engine->sound.play(buttonSound, 1.0f);
		touch->RemoveAllSimpleButtons();
		image->RemoveAllImages();
		image->RenderImage(4, amp::PIX::P(glm::vec2(0.5, 0.5)), amp::PIX::XP(1), 1024);
		engine->events.publishEvent(TEXT("findRandomMatch"));
	}
}

void MenuUI::OnTutorialGame(bool isFirst)
{
	if (isFirst) {
		engine->sound.play(buttonSound, 1.0f);
		image->RemoveAllImages();
		image->RenderImage(4, amp::PIX::P(glm::vec2(0.5, 0.5)), amp::PIX::XP(1), 1024);
	}
	else {
		touch->RemoveAllSimpleButtons();
		engine->events.publishEvent(TEXT("tutorialGame"));
	}
}

