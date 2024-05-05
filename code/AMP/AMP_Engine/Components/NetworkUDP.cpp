#include "pch.h"
#include "NetworkUDP.h"

amp::NetworkUDP::NetworkUDP()
{
}

void amp::NetworkUDP::BeforeShutdown()
{
	closeSending();
	closeReceiving();
}

void amp::NetworkUDP::SubShutdown()
{
	engine->events.subscribeEvent(engine->beforeShutdown, &NetworkUDP::BeforeShutdown, this);
	isSetUpCalled = true;
}

void amp::NetworkUDP::SetUpSending(AddressInfo info)
{
	if (!isSetUpCalled)SubShutdown();
	isSendingFinished = false;
	UpdateSendingInfo(AddressInfo(info));
	sendingThread = std::thread(&NetworkUDP::SendingInfo, this);
}

void amp::NetworkUDP::SetUpReceiving(AddressInfo info)
{
	if (!isSetUpCalled)SubShutdown();
	isReceivingFinished = false;
	UpdateReceivingInfo(AddressInfo(info));
	receivingThread = std::thread(&NetworkUDP::ReceiveInfo, this);
}

void amp::NetworkUDP::closeSending()
{
	if (sendingThread.joinable()) {
		isSendingFinished = true;
		sendingThread.join();
	}
}

void amp::NetworkUDP::closeReceiving()
{
	if (receivingThread.joinable()) {
		isReceivingFinished = true;
		receivingThread.join();
	}
}

void amp::NetworkUDP::SendingInfo()
{
	struct sockaddr_in servaddr = { 0 };
	int sockfd = -1;
	while (!isSendingFinished)
	{
		if (isNewSendingAdress) {
			if (sockfd >= 0)close(sockfd);
			sockfd = socket(AF_INET, SOCK_DGRAM, 0);
			if (sockfd == -1) {
				//Socket creation failed
				errorCode.exchange(NetworkError::Socket);
				return;
			}
			servaddr.sin_family = AF_INET;
			servaddr.sin_port = htons(sendingAdress.port);
			servaddr.sin_addr.s_addr = NetworkHelper::makeAddressV4(sendingAdress);
			isNewSendingAdress = false;
		}
		if (isSending) {
			if (sockfd == -1)throw "error, please setUp a sending address before Sending";
			sendLock.lock();
			sendto(sockfd, (const uint8_t*)sendingInfo.data(), sendingInfo.size(), 0, (const struct sockaddr*) & servaddr, sizeof(servaddr));
			sendLock.unlock();
			isSending = false;
		}
	}
	if (sockfd >= 0)close(sockfd);
}

void amp::NetworkUDP::ReceiveInfo()
{
	struct sockaddr_in sevaddr = { 0 };
	int sockfd = -1;
	while (!isReceivingFinished)
	{
		if (isNewReceivingAdress) {
			if (sockfd >= 0)close(sockfd);
			sockfd = socket(AF_INET, SOCK_DGRAM, 0);
			if (sockfd == -1) {
				//Socket creation failed
				errorCode.exchange(NetworkError::Socket);
				return;
			}
			sevaddr.sin_family = AF_INET;
			sevaddr.sin_port = htons(receivingAdress.port);
			sevaddr.sin_addr.s_addr = NetworkHelper::makeAddressV4(receivingAdress);
			int rc = bind(sockfd, (const struct sockaddr*) & sevaddr, sizeof(sevaddr));
			if (rc == -1) {
				close(sockfd);
				//Bind error
				errorCode.exchange(NetworkError::Bind);
				return;
			}
			isNewReceivingAdress = false;
		}
		if (sockfd == -1)throw "you should bind first a receiving address";
		socklen_t len = 0;
		uint8_t buffer[BufferSize] = { 0 };
		int n = recvfrom(sockfd, (uint8_t*)buffer, BufferSize, MSG_DONTWAIT, 0, &len);
		if (n != -1) {
			receiveLock.lock();
			if (receivingInfo.size())receivingInfo.clear();
			receivingInfo.insert(receivingInfo.begin(), std::begin(buffer), std::begin(buffer) + n);
			receiveLock.unlock();
		}
	}
	if(sockfd >= 0)close(sockfd);
}

void amp::NetworkUDP::UpdateSendingInfo(AddressInfo info)
{
	this->sendingAdress.update(info);
	this->isNewSendingAdress = true;
}

void amp::NetworkUDP::UpdateReceivingInfo(AddressInfo info)
{
	this->receivingAdress.update(info);
	this->isNewReceivingAdress = true;
}

void amp::NetworkUDP::update()
{
	CheckNetworkError(NetworkError((int)errorCode));
}

void amp::NetworkUDP::CheckNetworkError(NetworkError err)
{
	if (err) {
		if (onInvalid)onInvalid(err);
	}
}