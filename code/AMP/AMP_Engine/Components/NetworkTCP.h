#pragma once
#include "Component.h"
#include "Utility/NetworkHelper.h"
#include "thread"
#include "atomic"
#include <condition_variable>
#include "string"
#include "vector"
#include "AMP_Engine.h"
#include "Event/EventSystem.h"
#include "cstring"

namespace amp
{
	/// This is an AMP default component
	/// 
	/// This component allows you to send messages with the transmission control protocol (tcp)
	/// Call the functions from this component inside the Actor class
	class NetworkTCP : public Component
	{
	public:
		NetworkTCP();
		virtual ~NetworkTCP() {

		};
		NetworkTCP(NetworkTCP&&) = delete;
		NetworkTCP(const NetworkTCP&) = delete;

		/// Preparation to send tcp  messages
		/// 
		/// @note Creates a SendingThread
		/// @param info Contains all important server information to send your message
		/// @see AdressInfo
		void SetUpSending(AddressInfo info);

		/// Preparation to receive tcp  messages
		/// 
		/// @note Creates a ReceivingThread, which acts like a server
		/// @param info Contains all important server information to receive your message
		/// @param info In most cases you want to set the IP to "0.0.0.0" and just specify the port (local ip != actual ip)
		/// @see AdressInfo
		void SetUpReceiving(AddressInfo info);

		/// Shuts down the sending Thread
		///
		/// The sending Thread becomes automatically shut down when your scene is unloaded
		void closeSending();

		/// Shuts down the receiving Thread
		///
		/// The receiving Thread becomes automatically shut down when your scene is unloaded
		void closeReceiving();

		/// Does not block until the thread has finished its work
		void smoothCloseSending();

		/// Does not block until the thread has finished its work
		void smoothCloseReceiving();

		/// Calls a function when an network error is coming up, like disconnect, ...
		/// 
		/// @param cb Function when network error arises including the error reason Signature: myErrorCallbackFunction(NetworkError err)
		/// @see NetworkError
		void InvalidationCallback(std::function<void(NetworkError)> cb) {
			onInvalid = cb;
		}

		/// Sends a message with the earlier set up Sending Thread. SetUpSending() must be called before this function 
		/// 
		/// @param info Here you specify the content of youre message. For easier usage use nlohmann::json and call the function ```nlohmann::json::to_bson(yourJson)```
		/// @param shutdownAfterSend When you just want to send one message, an automatic shutdown is performed when the server received the message
		void Send(std::vector<uint8_t> info,bool shutdownAfterSend = false) {
			sendLock.lock();
			sendingInfo = info;
			sendLock.unlock();
			isSending = true;
			instantShutdown = shutdownAfterSend;
		}

		/// Checks if new information have arrived
		/// 
		/// @param resetInfoAfterCall Makes sure you read all information just once
		/// @param resetInfoAfterCall When set to false the next messages become inserted at the end of the internal buffer 
		/// @return The current internal receive buffer, when there is no new data the vector is **empty**. You should check for it
		std::vector<uint8_t> getReceivedInfo(bool resetInfoAfterCall = true) {
			receiveLock.lock();
			auto info = receivingInfo;
			receiveLock.unlock();
			return info;
		}

		/// Clears the internal receive buffer
		///
		/// @note Can be helpful when you not want to receive data for a certain time
		void clearReceivedInfo() {
			receiveLock.lock();
			receivingInfo.clear();
			receiveLock.unlock();
		}

		/// Here you can specify a new target server (replaces old one)
		/// 
		/// @param info Contains all important server information to send your message
		void UpdateSendingInfo(AddressInfo info);

		/// Here you can specify a new client you want to communicate with (replaces old one) 
		/// 
		/// @param info Contains all important server information to receive your message
		void UpdateReceivingInfo(AddressInfo info);

	protected:
		void CheckNetworkError(NetworkError err);

		virtual void ReceiveInfo();
		virtual void SendingInfo();
		void BeforeShutdown();

		void SubShutdown();
		virtual void update() override;

		bool isSetUpCalled = false;

		std::thread sendingThread;
		AddressInfo sendingAdress;
		std::atomic<bool> isNewSendingAdress = true;
		std::atomic<bool> isSending = false;
		std::atomic<bool> isSendingFinished = false;
		std::atomic<bool> instantShutdown = false;

		std::thread receivingThread;
		AddressInfo receivingAdress;
		std::atomic<bool> isNewReceivingAdress = true;
		std::atomic<bool> isReceivingFinished = false;
		std::atomic<bool> receivedEnd = false;
		std::atomic<bool> sendEnd = false;


		std::vector<uint8_t> sendingInfo;
		std::vector<uint8_t> receivingInfo;
		std::mutex receiveLock;
		std::mutex sendLock;

		bool smoothEndReceive = false;
		bool smoothEndSend = false;
		const int connectionTries = 10;

		std::function<void(NetworkError)> onInvalid = nullptr;
		std::atomic<int> errorCode = 0;
	};
}
