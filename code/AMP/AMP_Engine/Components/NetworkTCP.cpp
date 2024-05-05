#include "pch.h"
#include "errno.h"
#include "asm-generic/socket.h"
#include "NetworkTCP.h"

amp::NetworkTCP::NetworkTCP()
{

}

void amp::NetworkTCP::SetUpSending(AddressInfo info)
{
	if (!isSetUpCalled)SubShutdown();
	isSendingFinished = false;
	UpdateSendingInfo(AddressInfo(info));
	sendingThread = std::thread(&NetworkTCP::SendingInfo, this);
}

void amp::NetworkTCP::SetUpReceiving(AddressInfo info)
{
	if (!isSetUpCalled)SubShutdown();
	isReceivingFinished = false;
	UpdateReceivingInfo(AddressInfo(info));
	receivingThread = std::thread(&NetworkTCP::ReceiveInfo, this);
}

void amp::NetworkTCP::closeSending()
{
	if (sendingThread.joinable()) {
		isSendingFinished = true;
		sendingThread.join();
	}
}

void amp::NetworkTCP::closeReceiving()
{
	if (receivingThread.joinable()) {
		isReceivingFinished = true;
		receivingThread.join();
	}
}

void amp::NetworkTCP::smoothCloseSending()
{
	if (sendingThread.joinable()) {
		isSendingFinished = true;
		smoothEndSend = true;
	}
}

void amp::NetworkTCP::smoothCloseReceiving()
{
	if (receivingThread.joinable()) {
		isReceivingFinished = true;
		smoothEndReceive = true;
	}

}

void amp::NetworkTCP::UpdateSendingInfo(AddressInfo info)
{
	this->sendingAdress.update(info);
	this->isNewSendingAdress = true;
}

void amp::NetworkTCP::UpdateReceivingInfo(AddressInfo info)
{
	this->receivingAdress.update(info);
	this->isNewReceivingAdress = true;
}

void amp::NetworkTCP::update()
{
	CheckNetworkError(NetworkError((int)errorCode));
	if(instantShutdown){
		if (isSendingFinished) {
			closeSending();
			instantShutdown = false;
		}
	}
	if (smoothEndSend) {
		if (sendEnd){
			sendingThread.join();
			sendEnd = false;
			smoothEndSend = false;
		}
	}
	if (smoothEndReceive) {
		if (receivedEnd) {
			receivingThread.join();
			receivedEnd = false;
			smoothEndReceive = false;
		}
	}
}



void amp::NetworkTCP::ReceiveInfo()
{
	int connfd, len;
	int sockfd = -1;
	struct sockaddr_in servaddr = { 0 };
	struct sockaddr_in cli = { 0 };


	while (!isReceivingFinished) {
		if (isNewReceivingAdress) {
			if (sockfd >= 0)close(sockfd);
			// socket create and verification 
			sockfd = socket(AF_INET, SOCK_STREAM, 0);
			if (sockfd == -1) {
				//Socket creation failed
				errorCode.exchange(NetworkError::Socket);
				return;
			}
			// assign IP, PORT 
			servaddr.sin_family = AF_INET;
			servaddr.sin_port = htons(receivingAdress.port);
			servaddr.sin_addr.s_addr = NetworkHelper::makeAddressV4(receivingAdress);

			//Makes sure port can be reused if tcp server is restarted with same port (e.g: new Game is started, new Level, ...)
			//Problem: TCP port stays open for a while after the socket is closed, so rebinding the port after some seconds would fail
			setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &receivingAdress.port, sizeof(receivingAdress.port));
			// Binding newly created socket to given IP and verification 
			if ((bind(sockfd, (struct sockaddr*) & servaddr, sizeof(servaddr))) != 0) {
				int err = errno;
				//Socket creation failed
				errorCode.exchange(NetworkError::Socket);
				return;
			}

			// Now server is ready to listen and verification 
			if ((listen(sockfd, 5)) != 0) {
				errorCode.exchange(NetworkError::Listening);
				return;
			}

			len = sizeof(cli);
			// Accept the data packet from client and verification 
			connfd = accept(sockfd, (struct sockaddr*) & cli, &len);
			if (connfd < 0) {
				errorCode.exchange(NetworkError::Acception);
				return;
			}
			isNewReceivingAdress = false;
		}
		if (sockfd == -1)throw "Before sending a socket has to be bind !!";
		uint8_t buffer[BufferSize] = { 0 };
		// and send that buffer to client (non blocking)
		int sendLen = read(connfd, (uint8_t*)buffer, BufferSize);
		if (sendLen > 0) {
			receiveLock.lock();
			//Data can be send in multiple packages !!!
			//if (receivingInfo.size())receivingInfo.clear();
			receivingInfo.insert(receivingInfo.end(), std::begin(buffer), std::begin(buffer) + sendLen);
			receiveLock.unlock();
			write(connfd, (char*)"ok", 2);
		}
	}
	if(sockfd >= 0)close(sockfd);
	receivedEnd = true;
}

void amp::NetworkTCP::SendingInfo()
{
	bool waitForAnswer = false;
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

			//Makes sure port can be reused if tcp server is restarted with same port (e.g: new Game is started, new Level, ...)
			//Problem: TCP port stays open for a while after the socket is closed, so rebinding the port after some seconds would fail
			//setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &sendingAdress.port, sizeof(sendingAdress.port));

			linger lin;
			lin.l_onoff = 0;
			lin.l_linger = 0;
			//connection is lost as soon as the program leaves
			if (setsockopt(sockfd, SOL_SOCKET, SO_LINGER, (const char*)&lin, sizeof(lin)) != 0) {
				int error = errno;
				errorCode.exchange(NetworkError::Socket);
				return;
			}
			// connect the client socket to server socket 
			int counter = 0;
			while (connect(sockfd, (struct sockaddr*) & servaddr, sizeof(servaddr)) != 0) {
				if (counter > connectionTries) {
					errorCode.exchange(NetworkError::InvalidConnection);
					return;
				}
				std::this_thread::sleep_for(std::chrono::seconds(1));
				++counter;
			}
			isNewSendingAdress = false;
		}
		if(isSending){
			if (sockfd == -1)throw "Before sending a socket has to be bind !!";
			//char buff[BufferSize] = { 0 };
			sendLock.lock();
			int infoLen = write(sockfd, (const uint8_t*)sendingInfo.data(), sendingInfo.size());
			sendLock.unlock();
			if (infoLen < 0) {
				errorCode.exchange(NetworkError::ReceiveError);
				return;
			}
			isSending = false;
			waitForAnswer = true;
		}
		if(waitForAnswer) {
			char buffer[BufferSize] = { 0 };
			int infoLen = read(sockfd, (char*)buffer, BufferSize);
			if (infoLen < 0 || infoLen == 2) {
				waitForAnswer = false;
				if (instantShutdown)isSendingFinished = true;
			}
		}
	}
	// close the socket 
	if (sockfd >= 0)close(sockfd);	
	sendEnd = true;
}

void amp::NetworkTCP::BeforeShutdown()
{
	closeSending();
	closeReceiving();
}

void amp::NetworkTCP::SubShutdown()
{
	engine->events.subscribeEvent(engine->beforeShutdown, &NetworkTCP::BeforeShutdown, this);
	isSetUpCalled = true;
}

void amp::NetworkTCP::CheckNetworkError(NetworkError err)
{
	if (err) {
		if (onInvalid)onInvalid(err);
	}
}