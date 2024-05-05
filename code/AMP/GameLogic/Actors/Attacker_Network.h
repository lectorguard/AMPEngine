#pragma once
#include "Actors/Attacker_Player.h"

class Attacker_Network : public Attacker_Player
{
public:
	Attacker_Network() {};
	Attacker_Network(const Attacker_Network&) = delete;
	Attacker_Network(Attacker_Network&&) = delete;
	virtual ~Attacker_Network() {};

	//EVENTS
	//INPUTS
	TEXT startAttackNetwork = TEXT("startAttackNetwork");
	TEXT resetAttackNetwork = TEXT("resetAttackNetwork");
	//OUTPUTS
	TEXT endAttackNetwork = TEXT("endAttackNetwork");

	void SubscribeGameEvents() override;

	void StartAttackNetwork();

	void StartAttackDefendingPlayerState() override;

	void onThrowNetworkInput(glm::vec3 force);

	void onIconTouch() override;

	void onInvalid(amp::DynamicMesh* mesh, InvalidType type) override;

	void ResetAttackNetwork();

private:
	unsigned char CheckUpdateFlags = 0;
};

