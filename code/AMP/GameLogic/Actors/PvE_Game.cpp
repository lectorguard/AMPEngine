#include "pch.h"
#include "Tasks/PostProcessTask.h"
#include "Tasks/WinLoose.h"
#include "Event/EventSystem.h"
#include "PvE_Game.h"
#include "cstdlib"

void PvE_Game::AfterWindowStartup()
{
	//post post init subscribe
	SUB(engine->postPostInit, PvE_Game::OnPostPostInit);
	SUB(engine->postInit, PvE_Game::OnPostInit);

	//Attacker Subscriptions
	SUB(TEXT("endAttackerCPU"), PvE_Game::EndAttackCPU);
	SUB(TEXT("endAttackPlayer"), PvE_Game::EndAttackPlayer);

	//Defender Subscriptions
	SUB(TEXT("endDefendingCPU"), PvE_Game::EndDefendCPU);
	SUB(TEXT("endDefendPlayer"), PvE_Game::EndDefendPlayer);

	//Winning Game
	SUB(TEXT("onWinningGame"), PvE_Game::OnWinningGame);

	auto* postProcess = controller->AddTask<PostProcessTask>();
	postProcess->StartPostProcess();
}

void PvE_Game::OnPostInit()
{
	attackerPlayer = engine->getSceneActorByName(TEXT("attackPlayer"));
	attackerNetwork = engine->getSceneActorByName(TEXT("Attacker_CPU"));
	defenderPlayer = engine->getSceneActorByName(TEXT("Defender_Player"));
	defenderNetwork = engine->getSceneActorByName(TEXT("Defender_CPU"));
	assert(attackerPlayer);
	assert(attackerNetwork);
	assert(defenderPlayer);
	assert(defenderNetwork);
}

void PvE_Game::OnPostPostInit()
{
	GameStagesOffline startStage = CreateRandomStartingStage();
	attackerPlayer->SetActive(false);
	attackerNetwork->SetActive(false);
	defenderPlayer->SetActive(false);
	defenderNetwork->SetActive(false);
	StartRound(startStage);
}

void PvE_Game::StartRound(GameStagesOffline stage)
{
	switch (stage)
	{
	case Attacker:
		attackerPlayer->SetActive(true);
		defenderNetwork->SetActive(true);
		engine->events.publishEvent(TEXT("startAttackPlayer"));
		engine->events.publishEvent(TEXT("startDefendingCPU"));
		break;
	case Defender:
		defenderPlayer->SetActive(true);
		attackerNetwork->SetActive(true);
		engine->events.publishEvent(TEXT("startDefendPlayer"));
		engine->events.publishEvent(TEXT("startAttackerCPU"));
		break;
	}
}

void PvE_Game::EndRound(GameStagesOffline stage)
{
	engine->events.publishEvent(TEXT("endRound"));
	switch (stage)
	{
	case Attacker:
		engine->events.publishEvent(TEXT("resetAttackPlayer"));
		engine->events.publishEvent(TEXT("resetDefendingCPU"));
		attackerPlayer->SetActive(false);
		defenderNetwork->SetActive(false);
		break;
	case Defender:
		engine->events.publishEvent(TEXT("resetDefendPlayer"));
		engine->events.publishEvent(TEXT("resetAttackerCPU"));
		defenderPlayer->SetActive(false);
		attackerNetwork->SetActive(false);
		break;
	}
}

void PvE_Game::EndAttackPlayer()
{
	EndRound(GameStagesOffline::Attacker);
	StartRound(GameStagesOffline::Defender);
}

void PvE_Game::EndAttackCPU()
{
	EndRound(GameStagesOffline::Defender);
	StartRound(GameStagesOffline::Attacker);
}

void PvE_Game::EndDefendPlayer()
{
	EndRound(GameStagesOffline::Defender);
	StartRound(GameStagesOffline::Attacker);
}

void PvE_Game::EndDefendCPU()
{
	EndRound(GameStagesOffline::Attacker);
	StartRound(GameStagesOffline::Defender);
}

GameStagesOffline PvE_Game::CreateRandomStartingStage()
{
	return GameStagesOffline(std::rand() % 2);
}

void PvE_Game::OnWinningGame(Player player)
{
	if (hasGameEnded)return;
	if (player == One) {
		engine->events.publishEvent(TEXT("startDisplay"), Game::Win);
	}
	else engine->events.publishEvent(TEXT("startDisplay"), Game::Loose);
	hasGameEnded = true;
	amp::TimeEvent ev;
	ev.init(menuDelayTime, false, &PvE_Game::GoBackToMenu, this);
	engine->addTimerFunction(ev);
	ev.init(menuDelayTime - 0.5f, false, &PvE_Game::StartGoBackToMenuLoadingScreen, this);
	engine->addTimerFunction(ev);
}

void PvE_Game::GoBackToMenu()
{
	engine->events.publishEvent(TEXT("FromTutorialToMenu"));
}

void PvE_Game::StartGoBackToMenuLoadingScreen()
{
	engine->events.publishEvent(TEXT("startLoadingScreen"));
}

void PvE_Game::BeforeSystemShutdown()
{
	controller->ResetAllActiveTasks();
	controller->clear();
}
