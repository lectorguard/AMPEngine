#include "pch.h"
#include "Tasks/PostProcessTask.h"
#include "Tasks/WinLoose.h"
#include "PvP_Game.h"
#include "cstdlib"

void PvP_Game::AfterWindowStartup()
{
	//post post init subscribe
	SUB(engine->postPostInit, PvP_Game::OnPostPostInit);
	SUB(engine->postInit, PvP_Game::OnPostInit);

	//Attacker Subscriptions
	SUB(TEXT("endAttackNetwork"), PvP_Game::EndAttackNetwork);
	SUB(TEXT("endAttackPlayer"), PvP_Game::EndAttackPlayer);

	//Defender Subscriptions
	SUB(TEXT("endDefendNetwork"), PvP_Game::EndDefendNetwork);
	SUB(TEXT("endDefendPlayer"), PvP_Game::EndDefendPlayer);

	//Game End
	SUB(TEXT("onWinningGame"), PvP_Game::OnWinningGame);

	auto* postProcess = controller->AddTask<PostProcessTask>();
	postProcess->StartPostProcess();
}

void PvP_Game::OnPostInit()
{
	attackerPlayer = engine->getSceneActorByName(TEXT("Server_Attacker_Player"));
	attackerNetwork = engine->getSceneActorByName(TEXT("Server_Attacker_Network"));
	defenderPlayer = engine->getSceneActorByName(TEXT("Server_Defender_Player"));
	defenderNetwork = engine->getSceneActorByName(TEXT("Server_Defender_Network"));
	assert(attackerPlayer);
	assert(attackerNetwork);
	assert(defenderPlayer);
	assert(defenderNetwork);
}

void PvP_Game::OnPostPostInit()
{
	attackerPlayer->SetActive(false);
	attackerNetwork->SetActive(false);
	defenderPlayer->SetActive(false);
	defenderNetwork->SetActive(false);
	StartRound(GameStagesOffline::Defender);
}

void PvP_Game::StartRound(GameStagesOffline stage)
{
	switch (stage)
	{
	case Attacker:
		attackerPlayer->SetActive(true);
		defenderNetwork->SetActive(true);
		engine->events.publishEvent(TEXT("startAttackPlayer"));
		engine->events.publishEvent(TEXT("startDefendingNetwork"));
		engine->events.publishEvent(TEXT("updateGameState"), GameStagesOffline::Defender);
		break;
	case Defender:
		defenderPlayer->SetActive(true);
		attackerNetwork->SetActive(true);
		engine->events.publishEvent(TEXT("startDefendPlayer"));
		engine->events.publishEvent(TEXT("startAttackNetwork"));
		engine->events.publishEvent(TEXT("updateGameState"), GameStagesOffline::Attacker);
		break;
	}
}

void PvP_Game::EndRound(GameStagesOffline stage)
{
	engine->events.publishEvent(TEXT("endRound"));
	switch (stage)
	{
	case Attacker:
		engine->events.publishEvent(TEXT("resetAttackPlayer"));
		engine->events.publishEvent(TEXT("resetDefendNetwork"));
		attackerPlayer->SetActive(false);
		defenderNetwork->SetActive(false);
		break;
	case Defender:
		engine->events.publishEvent(TEXT("resetDefendPlayer"));
		engine->events.publishEvent(TEXT("resetAttackNetwork"));
		defenderPlayer->SetActive(false);
		attackerNetwork->SetActive(false);
		break;
	}
}

void PvP_Game::EndAttackPlayer()
{
	EndRound(GameStagesOffline::Attacker);
	StartRound(GameStagesOffline::Defender);
}

void PvP_Game::EndAttackNetwork()
{
	EndRound(GameStagesOffline::Defender);
	StartRound(GameStagesOffline::Attacker);
}

void PvP_Game::EndDefendPlayer()
{
	EndRound(GameStagesOffline::Defender);
	StartRound(GameStagesOffline::Attacker);
}

void PvP_Game::EndDefendNetwork()
{
	EndRound(GameStagesOffline::Attacker);
	StartRound(GameStagesOffline::Defender);
}

void PvP_Game::OnWinningGame(Player player)
{
	if (hasGameEnded)return;
	if (player == One) {
		engine->events.publishEvent(TEXT("startDisplay"), Game::Win);
	}
	else engine->events.publishEvent(TEXT("startDisplay"), Game::Loose);
	hasGameEnded = true;
	amp::TimeEvent ev;
	ev.init(menuDelayTime, false, &PvP_Game::GoBackToMenu, this);
	engine->addTimerFunction(ev);
	ev.init(menuDelayTime - 0.5f, false, &PvP_Game::StartGoBackToMenuLoadingScreen, this);
	engine->addTimerFunction(ev);
}


void PvP_Game::BeforeSystemShutdown()
{
	controller->ResetAllActiveTasks();
	controller->clear();
}

void PvP_Game::GoBackToMenu()
{
	engine->events.publishEvent(TEXT("fromServerGameToMenu"));
}

void PvP_Game::StartGoBackToMenuLoadingScreen()
{
	engine->events.publishEvent(TEXT("startLoadingScreen"));
}

