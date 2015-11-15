/*==============================================================================

	OscSender.cpp

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
#include "OscSender.h"

#include "Log.h"
#include <sstream>

namespace osc {

OscSender::OscSender() : 
	m_address(NULL), m_message(NULL), m_addressPattern(""),
	m_messageInProgress(false), m_bundleInProgress(false) {}

OscSender::OscSender(std::string address, unsigned int port) :
	m_address(NULL), m_message(NULL), m_addressPattern(""),
	m_messageInProgress(false), m_bundleInProgress(false) {
	setup(address, port);
}

OscSender::~OscSender() {
	if(m_address) {
		lo_address_free(m_address);
	}
	clear();
}

void OscSender::setup(std::string address, unsigned int port) {
	if(m_address) {
		lo_address_free(m_address);
	}
	std::stringstream stream;
	stream << port;
	m_address = lo_address_new(address.c_str(), stream.str().c_str());
}

void OscSender::send() {
	if(!m_address || m_bundleInProgress || m_messageInProgress) {
		throw SendException();
	}
	if(m_bundles.size() > 0) {
		lo_send_bundle(m_address, m_bundles.front());
	}
	else {
		if(!m_message) {
			throw SendException();
		}
		lo_send_message(m_address, m_addressPattern.c_str(), m_message);
	}
	clear();
}

void OscSender::clear() {
	if(m_bundles.size() > 0) {
		lo_bundle_free_recursive(m_bundles.front());
		m_bundles.clear();
	}
	else if(m_message) {
		lo_message_free(m_message);
	}
	m_addressPattern = "";
	m_message = NULL;
}

// MESSAGE BUILDING

void OscSender::beginMessage(std::string addressPattern) {
	if(m_messageInProgress) {
		throw MessageInProgressException();
	}
	m_message = lo_message_new();
	m_messageInProgress = true;
	m_addressPattern = addressPattern;
}
	
void OscSender::addBool(bool var) {
	if(!m_messageInProgress) {
		throw MessageNotInProgressException();
	}
	if(var) {
		lo_message_add_true(m_message);
	}
	else {
		lo_message_add_false(m_message);
	}
}

void OscSender::addChar(char var) {
	if(!m_messageInProgress) {
		throw MessageNotInProgressException();
	}
	lo_message_add_char(m_message, var);
}

void OscSender::addNil() {
	if(!m_messageInProgress) {
		throw MessageNotInProgressException();
	}
	lo_message_add_nil(m_message);
}

void OscSender::addInfinitum() {
	if(!m_messageInProgress) {
		throw MessageNotInProgressException();
	}
	lo_message_add_infinitum(m_message);
}

void OscSender::addInt32(int32_t var) {
	if(!m_messageInProgress) {
		throw MessageNotInProgressException();
	}
	lo_message_add_int32(m_message, var);
}

void OscSender::addInt64(int64_t var) {
	if(!m_messageInProgress) {
		throw MessageNotInProgressException();
	}
	lo_message_add_int64(m_message, var);
}

void OscSender::addFloat(float var) {
	if(!m_messageInProgress) {
		throw MessageNotInProgressException();
	}
	lo_message_add_float(m_message, var);
}

void OscSender::addDouble(double var) {
	if(!m_messageInProgress) {
		throw MessageNotInProgressException();
	}
	lo_message_add_double(m_message, var);
}

void OscSender::addString(char *var) {
	if(!m_messageInProgress) {
		throw MessageNotInProgressException();
	}
	lo_message_add_string(m_message, var);
}

void OscSender::addString(std::string var) {
	if(!m_messageInProgress) {
		throw MessageNotInProgressException();
	}
	lo_message_add_string(m_message, var.c_str());
}

void OscSender::addSymbol(const Symbol &var) {
	if(!m_messageInProgress) {
		throw MessageNotInProgressException();
	}
	lo_message_add_symbol(m_message, var.value);
}

void OscSender::addMidiMessage(const MidiMessage &var) {
	if(!m_messageInProgress) {
		throw MessageNotInProgressException();
	}
	lo_message_add_midi(m_message, (uint8_t *)var.bytes);
}

void OscSender::addTimeTag(const TimeTag &var) {
	if(!m_messageInProgress) {
		throw MessageNotInProgressException();
	}
	lo_message_add_timetag(m_message, var.tag);
}

void OscSender::addBlob(const Blob &var) {
	if(!m_messageInProgress) {
		throw MessageNotInProgressException();
	}
	lo_blob blob = lo_blob_new(var.size, (void *)var.data);
	lo_message_add_blob(m_message, blob);
	lo_blob_free(blob);
}

void OscSender::endMessage() {
	if(!m_messageInProgress) {
		throw MessageNotInProgressException();
	}
	if(m_bundleInProgress) { // add message at current bundle depth
		// make a deep copy of the address pattern since liblo is using
		// a pointer internally and m_addressPattern may change by the time
		// we send the bundle
		char *addressPattern = new char[m_addressPattern.size()+1];
		std::copy(m_addressPattern.begin(), m_addressPattern.end(), addressPattern);
		addressPattern[m_addressPattern.size()] = '\0'; // null terminator
		lo_bundle_add_message(m_bundles.back(), addressPattern, m_message);
		m_message = NULL;
		m_addressPattern = "";
	}
	m_messageInProgress = false;
}

// BUNDLE BUILDING

void OscSender::beginBundle() {
	if(m_bundleInProgress) {
		throw MessageInProgressException();
	}
	lo_bundle b = lo_bundle_new(LO_TT_IMMEDIATE);
	if(m_bundles.size() > 0) { // increase bundle depth
		lo_bundle_add_bundle(m_bundles.back(), b);
	}
	m_bundles.push_back(b);
	m_bundleInProgress = true;
}

void OscSender::beginBundle(const TimeTag &tag) {
	if(m_messageInProgress) {
		throw MessageInProgressException();
	}
	lo_bundle b = lo_bundle_new(tag.tag);
	if(m_bundles.size() > 0) { // increase bundle depth
		lo_bundle_add_bundle(m_bundles.back(), b);
	}
	m_bundles.push_back(b);
	m_bundleInProgress = true;
}

void OscSender::endBundle() {
	if(!m_bundleInProgress) {
		throw BundleNotInProgressException();
	}
	if(m_bundles.size() > 1) { // don't pop top bundle
		m_bundles.pop_back();
	}
	else {
		m_bundleInProgress = false;
	}
}

// MESSAGE BUILDING VIA STREAM

OscSender& OscSender::operator<<(const BeginMessage &var) {
	beginMessage(var.addressPattern);
	return *this;
}

OscSender& OscSender::operator<<(const EndMessage &var) {
	endMessage();
	return *this;
}
		
OscSender& OscSender::operator<<(bool var) {
	addBool(var);
	return *this;
}

OscSender& OscSender::operator<<(const char var) {
	addChar(var);
	return *this;	
}

OscSender& OscSender::operator<<(const Nil &var) {
	addNil();
	return *this;	
}

OscSender& OscSender::operator<<(const Infinitum &var) {
	addInfinitum();
	return *this;	
}

OscSender& OscSender::operator<<(const int32_t var) {
	addInt32(var);
	return *this;	
}

OscSender& OscSender::operator<<(const int64_t var) {
	addInt64(var);
	return *this;
}

OscSender& OscSender::operator<<(float var) {
	addFloat(var);
	return *this;
}

OscSender& OscSender::operator<<(double var) {
	addDouble(var);
	return *this;
}

OscSender& OscSender::operator<<(const char *var) {
	addString(var);
	return *this;
}

OscSender& OscSender::operator<<(const std::string var) {
	addString(var);
	return *this;
}

OscSender& OscSender::operator<<(const Symbol &var) {
	addSymbol(var);
	return *this;
}

OscSender& OscSender::operator<<(const MidiMessage &var) {
	addMidiMessage(var);
	return *this;
}

OscSender& OscSender::operator<<(const TimeTag &var) {
	addTimeTag(var);
	return *this;
}

OscSender& OscSender::operator<<(const Blob &var) {
	addBlob(var);
	return *this;
}

// BUNDLE BUILDING VIA STREAM SENDING

OscSender& OscSender::operator<<(const BeginBundle &var) {
	beginBundle(var.timetag);
	return *this;
}

OscSender& OscSender::operator<<(const EndBundle &var) {
	endBundle();
	return *this;
}

// UTIL

const std::string OscSender::getHostname() const  {
	return m_address ? lo_address_get_hostname(m_address) : "";
}

const std::string OscSender::getPort() const {
	return m_address ? lo_address_get_port(m_address) : 0;
}

const std::string OscSender::getUrl() const {
	return m_address ? lo_address_get_url(m_address) : "";
}

void OscSender::print() {
	if(m_bundles.size() > 0) {
		lo_bundle_pp(m_bundles.back());
	}
	if(m_message) {
		lo_message_pp(m_message);
	}
}

} // namespace
