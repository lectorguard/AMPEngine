#include "GameUtility.h"
#include "MatchMaker.h"

void MatchMaker::OnActivate()
{
	SUB(findRandomMatch, MatchMaker::FindRandomMatch);
}

void MatchMaker::OnDeactivate()
{
	UnsubscribeEvents();
}

void MatchMaker::AfterWindowStartup()
{
	currentIp = amp::NetworkHelper::getIpV4AddressAsString();
	httpRequests->SetUp();
	httpRequests->InvalidationCallback([&](auto a) {OnNetworkError(a); });
	httpRequests->SetHost(matchMakingServerAddress);
}

void MatchMaker::FindRandomMatch()
{
	SendMatchMakingRequest();
}

void MatchMaker::SendMatchMakingRequest()
{
	using namespace nlohmann;
	json ip_json;
	ip_json["ip"] = currentIp;
	amp::RequestInfo request("/api/sendIP", amp::Request::POST, ip_json);
	httpRequests->SendRequest(request, [&](auto i) {getRequestAnswer(i); });
}

void MatchMaker::Update()
{

}

void MatchMaker::BeforeSystemShutdown()
{

}

void MatchMaker::getRequestAnswer(std::vector<char> jsonContent)
{
	using namespace nlohmann;
	json result = json::parse(jsonContent);
	bool success = result["successful"];
	if (success) {
		httpRequests->ShutDownRequests();
		std::string other = result["otherIP"];
		bool isHost = result["isHost"];
		if (isHost) {
			engine->events.publishEvent(TEXT("onlineGameServer"), other);
		}
		else {
			engine->events.publishEvent(TEXT("onlineGameClient"), other);
		}
	}
	else {
		SendMatchMakingRequest();
	}
}

void MatchMaker::OnNetworkError(amp::NetworkError err)
{
	engine->events.publishEvent(TEXT("fromMenuToMenu"));
}

