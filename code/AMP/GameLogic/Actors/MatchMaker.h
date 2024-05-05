#pragma once
#include "Actor.h"
#include "AMP_Engine.h"
#include "Components/NetworkHTTP.h"
#include "GameUtility.h"
#include "Event/EventSystem.h"
#include "Utility/NetworkHelper.h"

class MatchMaker : public amp::Actor
{
public:
	amp::NetworkHTTP * httpRequests;

	MatchMaker() {
		httpRequests = AddComponent<amp::NetworkHTTP>(TEXT("MatchMaker_httpRequests"));
	};
	MatchMaker(const MatchMaker&) = delete;
	MatchMaker(MatchMaker&&) = delete;
	virtual ~MatchMaker() {
	};

	const std::string matchMakingServerAddress = "matchmakings.herokuapp.com";

	//EVENTS
	//INPUT
	TEXT findRandomMatch = TEXT("findRandomMatch"); //No Params

	virtual void OnActivate() override;

	virtual void OnDeactivate() override;

	virtual void AfterWindowStartup() override;

	void FindRandomMatch();

	void SendMatchMakingRequest();

	virtual void Update() override;

	virtual void BeforeSystemShutdown() override;

	void getRequestAnswer(std::vector<char> jsonContent);

	void OnNetworkError(amp::NetworkError err);
private:
	std::string currentIp;

};