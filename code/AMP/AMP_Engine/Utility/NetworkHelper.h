#pragma once
#include "atomic"
#include "array"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "arpa/inet.h"
#include "netinet/in.h"
#include "ifaddrs.h"
#include "string"
#include "sstream"
#include "vector"
#include "utility"

#ifndef BufferSize
	#define  BufferSize 8000
#endif


namespace amp
{
	/// This enum defines possible network errors
	enum NetworkError {
		NetworkOkay = 0, Socket = 1, InvalidConnection = 2, SendError = 3, InvalidAddress = 4, Listening = 5, Acception = 6, ReceiveError = 7, Bind = 8
	};
	
	/// This is a amp helper network struct containing address infos
	/// 
	/// This struct is thread safe
	struct AddressInfo {
		AddressInfo() {};
		
		/// Copy constructor
		AddressInfo(const AddressInfo& other)
		{
			ip0.exchange(other.ip0);
			ip1.exchange(other.ip1);
			ip2.exchange(other.ip2);
			ip3.exchange(other.ip3);
			port.exchange(other.port);
		};
		
		/// Creates a new Address Info
		/// 
		/// @param ip0 The first part of an ipv4 address
		/// @param ip1 The second part of an ipv4 address
		/// @param ip2 The third part of an ipv4 address
		/// @param ip3 The fourth part of an ipv4 address
		/// @param port The sending or receiving port
		/// @return The address port
		AddressInfo(uint ip0, uint ip1, uint ip2, uint ip3, uint port) {
			this->ip0 = ip0;
			this->ip1 = ip1;
			this->ip2 = ip2;
			this->ip3 = ip3;
			this->port = port;
		}
		
		/// Creates a new Address Info
		/// 
		/// @note port is "0.0.0.0"
		/// @return The address port
		AddressInfo(uint port) {
			this->port = port;
		}
		
		/// @param info Updates current info with a new one
		void update(const AddressInfo& info) {
			ip0.exchange(info.ip0);
			ip1.exchange(info.ip1);
			ip2.exchange(info.ip2);
			ip3.exchange(info.ip3);
			port.exchange(info.port);
		};
		
		/// @return A copy of the AddressInfo
		const AddressInfo getCopy() {
			return AddressInfo(ip0, ip1, ip2, ip3, port);
		}
	
		std::atomic<uint> ip0 = 0u;
		std::atomic<uint> ip1 = 0u;
		std::atomic<uint> ip2 = 0u;
		std::atomic<uint> ip3 = 0u;
		std::atomic<uint> port = 80u;
	};
	
	
	/// This is the amp Network Helper
	/// 
	/// This struct contains utility functions for the amp Networking system
	/// @note This struct is static and can be called everywhere in code
	struct NetworkHelper {
		/// Splits an input string by an delimeter and returns a result vector (casts strings to integer)
		/// 
		/// @note This function can be used to split an ip address
		/// @param input The input string
		/// @param delimeter The split parameter 
		/// @return The split elements as an int vector
		static std::vector<int> split(std::string input, char delimeter) {
			std::istringstream stream(input);
			std::vector<int> result;
			std::string current;
			while (std::getline(stream, current, delimeter))
			{
				result.push_back(std::stoi(current));
			}
			return result;
		}

		/// Removes the header from the answer http request
		/// 
		/// @return the json content answer file
		/// @param input The input string from http answer
		static std::vector<char> getJson(std::string input) {
			std::vector<char> json;
			bool isAdding = false;
			for (auto elem : input) {
				if (elem == '{')isAdding = true;
				if (isAdding)json.push_back(elem);
				if (elem == '}')return json;
			}
			throw "Could not find Json !!";
		}

		/// Creates a AddressInfo from an ipv4 address as string and a port
		/// 
		/// @param addr The ipv4 address as c string
		/// @param port The port 
		/// @return AddressInfo created from the passed input variables
		static AddressInfo getAdressInfo(const char* addr, unsigned int port){
			auto arr = split(addr, '.');
			return AddressInfo(arr[0], arr[1], arr[2], arr[3], port);
		}
	
		/// Gets the IPv4 address from the operating system (unix)
		/// 
		/// @return The devices ipv4 address as int array
		static std::array<uint, 4> getIpV4Address(){
			struct ifaddrs* ifAddrStruct = nullptr;
			struct ifaddrs* ifa = nullptr;
			void* tmpAddrPtr = nullptr;
			std::array<uint, 4> returnArr{ 0u,0u,0u,0u };
			std::vector<int> local{ 127,0,0,1 };

			getifaddrs(&ifAddrStruct);

			for (ifa = ifAddrStruct; ifa; ifa = ifa->ifa_next) {
				if (!ifa->ifa_addr || ifa->ifa_addr->sa_family == AF_INET6) {
					continue;
				}
				if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
					// is a valid IP4 Address
					tmpAddrPtr = &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;
					char addressBuffer[INET_ADDRSTRLEN];
					inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
					auto resVec =  split(std::string(addressBuffer), '.');
					if (resVec != local) {
						std::copy_n(resVec.begin(), 4, returnArr.begin());
					}	
				}
			}
			if (ifAddrStruct) freeifaddrs(ifAddrStruct);
			return returnArr;
		}

		/// Gets the IPv4 address from the operating system (unix)
		/// 
		/// @return The devices ipv4 address as string
		static std::string getIpV4AddressAsString(){
			auto addr = getIpV4Address();
			std::stringstream ss;
			ss << addr[0] << "." << addr[1] << "." << addr[2] << "." << addr[3];
			return ss.str();
		}

		/// Gets the IPv6 address from the operating system (unix)
		/// 
		/// @return The devices ipv6 address as a pair vector consisting of the name of the address (first) and the actual address (second)
		static std::vector<std::pair<std::string, std::string>> getIPv6Addresses() {
			struct ifaddrs* ifAddrStruct = nullptr;
			struct ifaddrs* ifa = nullptr;
			void* tmpAddrPtr = nullptr;
			std::vector<std::pair<std::string, std::string>> ipV6Addr;

			getifaddrs(&ifAddrStruct);

			for (ifa = ifAddrStruct; ifa; ifa = ifa->ifa_next) {
				if (!ifa->ifa_addr || ifa->ifa_addr->sa_family == AF_INET) {
					continue;
				}
				else if (ifa->ifa_addr->sa_family == AF_INET6) { // check it is IP6
				 // is a valid IP6 Address
					tmpAddrPtr = &((struct sockaddr_in6*)ifa->ifa_addr)->sin6_addr;
					char addressBuffer[INET6_ADDRSTRLEN];
					inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
					std::string name = ifa->ifa_name;
					std::string addr = addressBuffer;
					ipV6Addr.push_back(std::make_pair(name, addr));
				}
			}
			if (ifAddrStruct) freeifaddrs(ifAddrStruct);
			return ipV6Addr;
		}

		/// Creates a ipv4 address for a socket from a AddressInfo struct
		/// 
		/// @param info The desired AddressInfo
		/// @return The in_addr_t for your socket
		static in_addr_t makeAddressV4(AddressInfo& info)
		{
			std::string address = std::to_string(info.ip0) + "." + std::to_string(info.ip1) + "." + std::to_string(info.ip2) + "." + std::to_string(info.ip3);
			return inet_addr(address.c_str());
		}
	};


} //namespace amp end
