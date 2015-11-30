/*==============================================================================

	OscReceiver.h

	lopack: an oscpack-inspired C++ wrapper for liblo
  
	Copyright (C) 2009, 2010 Dan Wilcox <danomatika@gmail.com>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.

==============================================================================*/
#pragma once

#include "OscObject.h"

namespace osc {

/// \class ReceiveException
/// \brief an OscReceiver exception
///
/// thrown by errors causes during OscReceiver set up (port in use) or running
/// (data type problems)
class ReceiveException : public std::runtime_error {
	public:
		ReceiveException(
			std::string w="liblo receive error")
			: std::runtime_error(w) {}
		ReceiveException(
			const char *w="liblo receive error")
			: std::runtime_error(w) {}
};

/// \class OscReceiver
/// \brief a threaded osc receiver
///
/// set the processing function to match messages or add OscObjects
class OscReceiver {
	
	public:

		OscReceiver(std::string rootAddress="");
		virtual ~OscReceiver();

		/// calls setup() automatically
		OscReceiver(unsigned int port, std::string rootAddress="");
	
		/// calls setupMulticast() automatically
		OscReceiver(std::string group, unsigned int port, std::string rootAddress="");

		/// setup the udp socket using the given port
		/// returns true on success
		bool setup(unsigned int port);
	
		/// setup the udp socket for a multicast group using the given port
		/// see http://tldp.org/HOWTO/Multicast-HOWTO-2.html
		/// returns true on success
		bool setupMulticast(std::string group, unsigned int port);
	
		/// stop thread & release socket
		void clear();

	/// \section Thread Control

		/// start the listening thread, opens connection
		void start();

		/// stop the listening thread, closes connection
		void stop();
		
	/// \section Manual Polling
		
		/// manually check for incoming messages, nonblocking
		/// timeoutMS is the number of ms to wait for a message, 0 = immediately
		///
		/// returns number of bytes received
		///
		/// note: the address must be set using setup(), this cannot be called
		/// while the thread is running
		int handleMessages(int timeoutMS=0);

	/// \section Objects

		/// add an OscObject to send received messages to
		void addOscObject(OscObject *object);

		/// remove an OscObject
		void removeOscObject(OscObject *object);
	
		/// remove all OscObjects
		void removeAllOscObjects();

	/// \section Util

		/// is the thread running?
		bool isListening() {return true;}

		/// get/set the root address of this object
		inline void setOscRootAddress(std::string rootAddress)	{m_oscRootAddress = rootAddress;}
		inline std::string &getOscRootAddress()	{return m_oscRootAddress;}

		/// ignore incoming messages while keeping port open (thread running)?
		inline void ignoreMessages(bool yesno) {m_ignoreMessages = yesno;}
	
		/// get the server host name or multicast group if using multicast
		const std::string getHostname() const;

		/// get port num
		const unsigned int getPort() const;
	
		/// get the osc url of this server (protocol, address, & port)
		const std::string getUrl() const;
	
		/// returns true if the server is using multicast
		const bool isMulticast() const;
	
		/// print object info to std::cout
		const void print() const;

	protected:

		/// callback, returns true if message handled
		virtual bool process(const ReceivedMessage &message, const MessageSource &source) {return false;}

		/// the root address of this object, aka something like "/root/test1/string2"
		std::string m_oscRootAddress;

	private:

		/// virtual callback from oscpack
		bool processMessage(const ReceivedMessage &message, const MessageSource &source);

		// static liblo callbacks
		static void errorCB(int num, const char *msg, const char *where);
		static int messageCB(const char *path, const char *types, lo_arg **argv,
		                     int argc, lo_message msg, void *user_data);
		
		lo_server_thread m_serverThread; //< liblo server thread handle
		bool m_isMulticast; //< is the server listening to a multicast group?

		bool m_isRunning; //< should the thread be running?
		bool m_ignoreMessages; //< ignore incoming messages?

		std::vector<OscObject*> m_objects; //< osc objects to send messages to
};

} // namespace
