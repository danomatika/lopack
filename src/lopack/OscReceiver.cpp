/*==============================================================================

	OscReceiver.cpp

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
#include "OscReceiver.h"

#include "Log.h"
#include <algorithm>
#include <sstream>

namespace osc {

OscReceiver::OscReceiver(std::string rootAddress) :
	m_oscRootAddress(rootAddress), m_serverThread(NULL), m_isMulticast(false),
	m_isRunning(false), m_ignoreMessages(false) {}

OscReceiver::OscReceiver(unsigned int port, std::string rootAddress) :
	m_oscRootAddress(rootAddress), m_serverThread(NULL), m_isMulticast(false),
	m_isRunning(false), m_ignoreMessages(false) {
	setup(port);
}

OscReceiver::OscReceiver(std::string group, unsigned int port, std::string rootAddress) :
	m_oscRootAddress(rootAddress), m_serverThread(NULL), m_isMulticast(false),
	m_isRunning(false), m_ignoreMessages(false) {
	setupMulticast(group, port);
}

OscReceiver::~OscReceiver() {
	clear();
}

bool OscReceiver::setup(unsigned int port) {
	if(m_serverThread) {
		LOG_WARN << "OscReceiver: cannot set port while thread is running" << std::endl;
		return false;
	}
	std::stringstream stream;
	stream << port;
	m_serverThread = lo_server_thread_new(stream.str().c_str(), &errorCB); 
	if(!m_serverThread) {
		LOG_ERROR << "OscReceiver: could not create server" << std::endl;
		return false;
	}
	lo_server_thread_add_method(m_serverThread, NULL, NULL, &messageCB, this);
	m_isMulticast = false;
	return true;
}

bool OscReceiver::setupMulticast(std::string group, unsigned int port) {
	if(m_serverThread) {
		LOG_WARN << "OscReceiver: cannot set multicast group & port while thread is running" << std::endl;
		return false;
	}
	std::stringstream stream;
	stream << port;
	m_serverThread = lo_server_thread_new_multicast(group.c_str(), stream.str().c_str(), &errorCB);
	if(!m_serverThread) {
		LOG_ERROR << "OscReceiver: could not create server" << std::endl;
		return false;
	}
	lo_server_thread_add_method(m_serverThread, NULL, NULL, &messageCB, this);
	m_isMulticast = true;
	return true;
}

void OscReceiver::clear() {
	stop();
	if(m_serverThread) {
		m_serverThread = NULL;
	}
	m_isMulticast = false;
}

// THREAD CONTROL

void OscReceiver::start() {
	if(!m_serverThread) {
		LOG_ERROR << "OscReceiver: cannot start thread, address not set" << std::endl;
		return;
	}
	lo_server_thread_start(m_serverThread);
	m_isRunning = true;
}

void OscReceiver::stop() {
	if(!m_serverThread) {
		return;
	}
	lo_server_thread_stop(m_serverThread);
	m_isRunning = false;
	m_ignoreMessages = false; // reset ignore
}

// MANUAL POLLING

int OscReceiver::handleMessages(int timeoutMS) {
	if(!m_serverThread) {
		LOG_ERROR << "OscReceiver: cannot handle messages, address not set" << std::endl;
		return 0;
	}
	if(m_isRunning) {
		LOG_WARN << "OscReceiver: you don't need to handle messages manually "
		         << "when the thread is already running" << std::endl;
		return 0;
	}
	lo_server server = lo_server_thread_get_server(m_serverThread);
	return lo_server_recv_noblock(server, timeoutMS);
}

/// OBJECTS

void OscReceiver::addOscObject(OscObject *object) {
	if(object == NULL) {
		LOG_WARN << "OscReceiver: cannot add NULL object" << std::endl;
		return;
	}
	m_objects.push_back(object);
}

void OscReceiver::removeOscObject(OscObject *object) {
	if(object == NULL) {
		LOG_WARN << "OscReceiver: cannot remove NULL object" << std::endl;
		return;
	}
	// find object in list and remove it
	std::vector<OscObject *>::iterator iter;
	iter = find(m_objects.begin(), m_objects.end(), object);
	if(iter != m_objects.end()) {
		m_objects.erase(iter);
	}
}

void OscReceiver::removeAllOscObjects() {
	m_objects.clear();
}

// UTIL

const std::string OscReceiver::getHostname() const  {
	return m_serverThread ? lo_url_get_hostname(lo_server_get_url(lo_server_thread_get_server(m_serverThread))) : "";
}

const unsigned int OscReceiver::getPort() const {
	return m_serverThread ? (unsigned int) lo_server_get_port(lo_server_thread_get_server(m_serverThread)) : 0;
}

const std::string OscReceiver::getUrl() const {
	return m_serverThread ? lo_server_get_url(lo_server_thread_get_server(m_serverThread)) : "";
}

const bool OscReceiver::isMulticast() const {
	return m_isMulticast;
}

const void OscReceiver::print() const {
	if(m_serverThread) {
		std::cout << getUrl() << std::endl;
	}
}

// PRIVATE

bool OscReceiver::processMessage(const ReceivedMessage &message, const MessageSource &source) {
	// ignore any incoming messages?
	if(m_ignoreMessages) {
		return false;
	}
		
	// call any attached objects
	std::vector<OscObject *>::iterator iter;
	for(iter = m_objects.begin(); iter != m_objects.end();) {
		if((*iter) != NULL) { // try to process message
			if((*iter)->processOsc(message, source))
				return true;
			iter++; // increment iter
		}
		else { // bad object, so erase it
			iter = m_objects.erase(iter);
			LOG_WARN << "OscReceiver: removed NULL object" <<std::endl;
		}
	}

	// user callback
	if(process(message, source)) {
		return true;
	}
	return false;
}

// STATIC CALLBACKS

void OscReceiver::errorCB(int num, const char *msg, const char *where) {
	std::stringstream stream;
	stream << "OscReceiver: liblo server thread error " << num;
	if(msg) {stream << ": " << msg;}     // might be NULL
	if(where) {stream << ": " << where;} // might be NULL
	throw ReceiveException(stream.str());
}

int OscReceiver::messageCB(const char *path, const char *types, lo_arg **argv,
                           int argc, lo_message msg, void *user_data) {
	OscReceiver *receiver = (OscReceiver *)user_data;
	return receiver->processMessage(ReceivedMessage(path, msg),
	                                MessageSource(lo_message_get_source(msg)));
}

} // namespace
