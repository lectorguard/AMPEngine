#pragma once
#include "Defender_Player.h"
#include "cassert"

class Defender_Network : public Defender_Player
{
public:
	Defender_Network() {};
	Defender_Network(const Defender_Network&) = delete;
	Defender_Network(Defender_Network&&) = delete;
	virtual ~Defender_Network();

	//INPUTS
	TEXT startDefendNetwork = TEXT("startDefendingNetwork");
	TEXT resetDefendNetwork = TEXT("resetDefendNetwork");
	//OUTPUTS
	TEXT endDefendNetwork = TEXT("endDefendNetwork");

	void SubscribeGameEvents() override;
	
	void ResetLocalVariables() override;

	void StartDefending() override;

	void StartCharacterTouch() override;

	void ResetDefendNetwork(); 

	void OnRotateFinished() override;

	void OnDirCalculated(Move m) override;

	void OnBottleHit() override;
	

};

