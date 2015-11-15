/*==============================================================================

	OscSender.h

	lopack: an oscpack-inspired C++ wrapper for liblo
  
	Copyright (C) 2009, 2010 Dan Wilcox <danomatika@gmail.com>
	
	interface inspired by oscpack:
	
	oscpack -- Open Sound Control packet manipulation library
	http://www.audiomulch.com/~rossb/oscpack

	Copyright (c) 2004-2005 Ross Bencina <rossb@audiomulch.com>

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

#include "OscTypes.h"
#include <vector>

namespace osc {

class SendException : public std::runtime_error {
	public:
		SendException(
			const char *w="call to send when setup or BeginMessage have not been called or bundle/message is in progress")
			: std::runtime_error(w) {}
};

class BundleNotInProgressException : public std::runtime_error {
	public:
		BundleNotInProgressException(
			const char *w="call to EndBundle when bundle is not in progress")
			: std::runtime_error(w) {}
};

class MessageInProgressException : public std::runtime_error {
	public:
		MessageInProgressException(
			const char *w="opening or closing bundle or message while message is in progress")
			: std::runtime_error(w) {}
};

class MessageNotInProgressException : public std::runtime_error {
	public:
		MessageNotInProgressException(
			const char *w="call to EndMessage when message is not in progress")
			: std::runtime_error(w) {}
};

/// \class OscSender
/// \brief send OSC packets through buffer using << stream
class OscSender {
	public:

		OscSender();
		OscSender(std::string address, unsigned int port);
		virtual ~OscSender();

		/// setup the ip address/hostname and port
		/// address can also be a multicast group
		/// see http://tldp.org/HOWTO/Multicast-HOWTO-2.html
		void setup(std::string address, unsigned int port);

		/// send the current message/bundle(s)
		void send();
	
		/// clear the current message/bundles(s)
		void clear();
	
	/// \section Message Building
	
		/// begin a message and set the osc address pattern,
		/// can be called within bundles
		void beginMessage(std::string addressPattern);
	
		void addBool(bool var);
		void addChar(char var);
		void addNil();
		void addInfinitum();
	
		void addInt32(int32_t var);
		void addInt64(int64_t var);
	
		void addFloat(float var);
		void addDouble(double var);
	
		void addString(char *var);
		void addString(std::string var);
		void addSymbol(const Symbol &var);
	
		void addMidiMessage(const MidiMessage &var);
		void addTimeTag(const TimeTag &var);
		void addBlob(const Blob &var);
	
		/// finish the message before calling send
		void endMessage();
	
	/// \section Bundle Building
	
		/// begin a bundle, can be called within other bundles
		void beginBundle();
		void beginBundle(const TimeTag &tag); //< set the timestamp
	
		/// finish the bundle before calling send or beginning a new bundle
		void endBundle();
		
	/// \section Message Building via Stream
		
		OscSender& operator<<(const BeginMessage &var);
		OscSender& operator<<(const EndMessage &var);
		
		OscSender& operator<<(const bool var);
		OscSender& operator<<(const char var);
		OscSender& operator<<(const Nil &var);
		OscSender& operator<<(const Infinitum& var);
		
		OscSender& operator<<(const int32_t var);
		OscSender& operator<<(const int64_t var);
		
		OscSender& operator<<(const float var);
		OscSender& operator<<(const double var);
		
		OscSender& operator<<(const char *var);
		OscSender& operator<<(const std::string var);
		OscSender& operator<<(const Symbol &var);
		
		OscSender& operator<<(const MidiMessage &var);
		OscSender& operator<<(const TimeTag &var);
		OscSender& operator<<(const Blob &var);
	
	/// \section Bundle Building via Stream
	
		OscSender& operator<<(const BeginBundle &var);
		OscSender& operator<<(const EndBundle &var);
	
	/// \section Util
	
		/// is a message currently in progress?
		inline bool isMessageInProgress() {return m_messageInProgress;}
	
		/// is a bundle currently in progress?
		inline bool isBundleInProgress() {return m_bundleInProgress;}
	
		/// get the host name or multicast group
		const std::string getHostname() const;

		/// get port num
		const std::string getPort() const;
	
		/// get the host osc url (protocol, address, & port)
		const std::string getUrl() const;
	
		// print the contents of the current message/bundle
		void print();

	private:
		
		lo_address	m_address; //< host address to send to
		lo_message	m_message; //< temp message object
		std::vector<lo_bundle> m_bundles; //< temp bundle object stack

		std::string m_addressPattern; //< temp osc address pattern
		
		bool m_messageInProgress; //< is a message currently being built?
		bool m_bundleInProgress;  //< is a bundle currently being built?
};

} // namespace
