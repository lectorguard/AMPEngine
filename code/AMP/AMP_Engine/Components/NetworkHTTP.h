#pragma once
#include "NetworkTCP.h"
#include "functional"
#include "sstream"
#include "netdb.h"
#include "../AssetLoader/Extern/json.hpp"

namespace amp
{
	///Request types 
	///
	/// @note Use GET when you are not sending additional information to server and are just expecting an answer
	/// @note Use POST when you are sending additional information to server, in order to receive a proper answer
	enum Request {
		GET, POST
	};

	/// This is a helper struct inside AMP
	/// 
	/// This struct contains all information about http requests
	struct RequestInfo {
		RequestInfo() {};

		/// RequestInfo constructor for POST requests
		/// 
		/// @param page The actual page without the host e.g: "/api/MySuperImportantInformation"
		/// @param type The request type
		/// @param j The request content as a json (just for post requests)
		RequestInfo(std::string page, Request type, nlohmann::json j) : page(page), requestType(type) {
			requestContent = j.dump();
		};

		/// RequestInfo constructor for GET requests
		/// 
		/// @param page The actual page without the host e.g: "/api/MySuperImportantInformation"
		/// @param type The request type
		RequestInfo(std::string page, Request type) : page(page), requestType(type) {
		};

		std::string page;
		Request requestType;
		std::string requestContent;
	};

	/// This is an AMP default component
	/// 
	/// This component allows you to make http requests
	/// Call the functions from this component inside the Actor class
	class NetworkHTTP : public NetworkTCP
	{
	public:
		NetworkHTTP();
		virtual ~NetworkHTTP() {

		};
		NetworkHTTP(NetworkHTTP&&) = delete;
		NetworkHTTP(const NetworkHTTP&) = delete;

		/// Sets up the http Request
		///
		/// @note Creates a new Thread for sending and receiving http requests
		/// @note Default port is 80, Default Content-Type = application/json
		void SetUp();

		/// Sends the request to the server
		/// 
		/// @param info Contains all necessary information to send the request
		/// @param cb The answer callback 
		/// @attention Before sending the request make sure to call the following functions first
		/// @see SetUp(), SetHost()
		/// @note Other interesting functions you could call before sending: SetPort(), SetRequestType(), SetContentType()
		void SendRequest(RequestInfo& info, std::function<void(std::vector<char>)> cb);

		/// Closes the send and receive thread
		///
		/// @note Thread is automatically closed when loading a new scene
		void ShutDownRequests() { closeSending(); };


		/// Constructs the header and the content for an http request for you
		///
		/// @param info The information to construct the http message
		const char* ConstructMessage(RequestInfo& info);


		/// Sets the port of the target server, default is 80
		void SetPort(unsigned int port) { this->defaultPort = port; };


		/// @return The current target port, the default is 80 
		unsigned int getPort() { return defaultPort; };

		/// Sets the request type
		void SetRequestType(Request type) { requestType = type; };

		/// Set the target Domain of the target Server (without page)
		///
		/// @param newHost Host is the domain of the website ! without http(s):// !
		void SetHost(std::string newHost) { this->host = newHost; };

		/// @return The data format of the content of the request. Default : "application/json"
		std::string GetContentType() { return contentType; };

		/// Sets the data format of the content of the request. Default: "application/json"
		void SetContentType(std::string type) { contentType = type; };
	
	protected:
		virtual void update() override;

	private:
		virtual void SendingInfo() override;

		//Request specific values
		Request requestType;
		unsigned int defaultPort = 80u;
		std::string host; 
		std::string contentType = "application/json";

		//Receiving
		std::atomic<int> receiveLength;
		std::vector<char> receivedJson;
		bool waitForReceive = false;

		std::function<void(std::vector<char>)> currentCallback = nullptr;
	};
	
}
