#include "pch.h"
#include "NetworkHTTP.h"



amp::NetworkHTTP::NetworkHTTP()
{

}

void amp::NetworkHTTP::SetUp()
{
	isSendingFinished = false;
	isNewSendingAdress = false;
	isSending = false;
	waitForReceive = false;
	sendingThread = std::thread(&NetworkHTTP::SendingInfo, this);
	SubShutdown();
}

//host without http(s)://
void amp::NetworkHTTP::SendRequest(RequestInfo& info, std::function<void(std::vector<char>)> cb)
{
	assert(!waitForReceive); //You can not make 2 or more request at the same time
	//Set IP and Port
	//Default Port is 80
	struct hostent* server = nullptr;
	server = gethostbyname(host.c_str());
	if (!server) {
		CheckNetworkError(InvalidAddress);
		return;
	}
	const char*	servAddr = inet_ntoa(*(struct in_addr*)(server->h_addr_list[0]));
	AddressInfo address = NetworkHelper::getAdressInfo(servAddr, defaultPort);
	UpdateSendingInfo(address);

	//Set Message
	const char* charMessage = ConstructMessage(info);
	std::vector<uint8_t> vecMessage(charMessage, charMessage + strlen(charMessage));
	Send(vecMessage);
	currentCallback = cb;
}

const char* amp::NetworkHTTP::ConstructMessage(RequestInfo& info)
{
	std::string requestType = info.requestType == 0 ? "GET" : "POST";
	std::string newLine = "\r\n";
	std::stringstream message;

	//for example:
	//POST /api/sendIP HTTP/1.1
    //Host: intense-sierra-23655.herokuapp.com
	//Content-Type : application/json
	//Accept : application/json
	//Content-Length : 18
	//
	//{ip: "127.0.0.1"}

	message << requestType << " " << info.page << " HTTP/1.1" << newLine;
	message << "Host: " << host << newLine;
	message << "Content-Type: " << contentType << newLine;
	message << "Accept: " << contentType << newLine;
	message << "Content-Length: " << info.requestContent.size() << newLine << newLine;
	message << info.requestContent << newLine;

	return message.str().c_str();
}

void amp::NetworkHTTP::SendingInfo()
{
	int sockfd = -1;
	struct sockaddr_in servaddr = { 0 };
	while (!isSendingFinished) {
		if (isNewSendingAdress) {
			if (sockfd >= 0)close(sockfd);
			// socket create and varification 
			sockfd = socket(AF_INET, SOCK_STREAM, 0);
			if (sockfd == -1) {
				//Socket creation failed
				errorCode.exchange(NetworkError::Socket);
				return;
			}
			// assign IP, PORT 
			servaddr.sin_family = AF_INET;
			servaddr.sin_port = htons(sendingAdress.port);
			servaddr.sin_addr.s_addr = NetworkHelper::makeAddressV4(sendingAdress);

			// connect the client socket to server socket 
			if (connect(sockfd, (struct sockaddr*) & servaddr, sizeof(servaddr)) != 0) {
				//connection with the server failed...
				errorCode.exchange(NetworkError::InvalidConnection);
				return;
			}
			isNewSendingAdress = false;
		}
		if (isSending) {
			if (waitForReceive)throw "Error";
			if (sockfd == -1){
				//Before sending a socket has to be bind !!
				errorCode.exchange(NetworkError::Socket);
				return;
			}
			sendLock.lock();
			int infoLen = write(sockfd, (const uint8_t*)sendingInfo.data(), sendingInfo.size());
			sendLock.unlock();
			if (infoLen < 0) {
				//Information could not by send with TCP
				errorCode.exchange(NetworkError::SendError);
				return;
			}
			isSending = false;
			waitForReceive = true;
		}
		if (waitForReceive) {
			char buffer[BufferSize] = { 0 };
			int infoLen = read(sockfd,(char*)buffer, BufferSize);
			if (infoLen > 0) {
				std::vector<char> jsonText = NetworkHelper::getJson(buffer);
				receiveLock.lock();
				if (receivingInfo.size())receivingInfo.clear();
				receivedJson = jsonText;
				receiveLock.unlock();
				receiveLength.exchange(infoLen);
				waitForReceive = false;
			}
		}
	}
	// close the socket 
	if (sockfd >= 0)close(sockfd);
}

void amp::NetworkHTTP::update()
{
	CheckNetworkError(NetworkError((int)errorCode));
	std::vector<char> latestInfo;
	if (receiveLength > 0) {
		receiveLock.lock();
		if (receivedJson.size() > 0 && currentCallback) {
			latestInfo = receivedJson;		
			receivingInfo.clear();
			receiveLock.unlock();
			currentCallback(latestInfo);
			receiveLength.exchange(0);
		}
		else receiveLock.unlock();
	}
}