#include "pch.h"
#include "Tasks/PostProcessTask.h"
#include "Tasks/WinLoose.h"
#include "PvP_Game_Streamed.h"
#include "cstdlib"

void PvP_Game_Streamed::AfterWindowStartup()
{
	//post post init subscribe
	SUB(engine->postPostInit, PvP_Game_Streamed::OnPostPostInit);
	SUB(engine->postInit, PvP_Game_Streamed::OnPostInit);

	//Attacker Subscriptions
	SUB(TEXT("endStreamAttacker"), PvP_Game_Streamed::EndStreamedAttack);
	
	//Defender Subscriptions
	SUB(TEXT("endStreamedDefender"), PvP_Game_Streamed::EndStreamedDefender);

	//Progress Bar
	SUB(TEXT("onWinningGame"), PvP_Game_Streamed::OnGameEnd);

	auto* postProcess = controller->AddTask<PostProcessTask>();
	postProcess->StartPostProcess();
}

void PvP_Game_Streamed::OnPostInit()
{
	streamedAttacker = engine->getSceneActorByName(TEXT("Client_Attacker_Streamed"));
	streamedDefender = engine->getSceneActorByName(TEXT("Client_Defender_Streamed"));
	assert(streamedAttacker);
	assert(streamedDefender);
}

void PvP_Game_Streamed::OnPostPostInit()
{
	//Streamed Game always starts with Attacking
	streamedAttacker->SetActive(false);
	streamedDefender->SetActive(false);
	StartRound(GameStagesOffline::Attacker);
}

void PvP_Game_Streamed::StartRound(GameStagesOffline stage)
{
	switch (stage)
	{
	case Attacker:
		streamedAttacker->SetActive(true);
		engine->events.publishEvent(TEXT("startStreamedAttacker"));
		break;
	case Defender:
		streamedDefender->SetActive(true);
		engine->events.publishEvent(TEXT("startStreamedDefender"));
		break;
	}
}

void PvP_Game_Streamed::EndRound(GameStagesOffline stage)
{
	engine->events.publishEvent(TEXT("endRound"));
	switch (stage)
	{
	case Attacker:
		engine->events.publishEvent(TEXT("resetStreamedAttacker"));
		streamedAttacker->SetActive(false);
		break;
	case Defender:
		engine->events.publishEvent(TEXT("resetStreamedDefender"));
		streamedDefender->SetActive(false);
		break;
	}
}

void PvP_Game_Streamed::EndStreamedAttack()
{
	EndRound(GameStagesOffline::Attacker);
	StartRound(GameStagesOffline::Defender);
}

void PvP_Game_Streamed::EndStreamedDefender()
{
	EndRound(GameStagesOffline::Defender);
	StartRound(GameStagesOffline::Attacker);
}

void PvP_Game_Streamed::OnGameEnd(Player p)
{
	if (hasGameEnded)return;
	if (p == One) {
		engine->events.publishEvent(TEXT("startDisplay"), Game::Win);
	}
	else {
		engine->events.publishEvent(TEXT("startDisplay"), Game::Loose);
	}
	hasGameEnded = true;
	amp::TimeEvent ev;
	ev.init(menuDelayTime, false, &PvP_Game_Streamed::GoBackToMenu, this);
	engine->addTimerFunction(ev);
	ev.init(menuDelayTime - 0.5f, false, &PvP_Game_Streamed::StartGoBackToMenuLoadingScreen, this);
	engine->addTimerFunction(ev);
}

void PvP_Game_Streamed::BeforeSystemShutdown()
{
	controller->ResetAllActiveTasks();
	controller->clear();
}

void PvP_Game_Streamed::GoBackToMenu()
{
	engine->events.publishEvent(TEXT("fromClientGameToMenu"));
}

void PvP_Game_Streamed::StartGoBackToMenuLoadingScreen()
{
	engine->events.publishEvent(TEXT("startLoadingScreen"));
}

