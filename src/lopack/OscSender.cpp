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
	m_address(0), m_message(0), m_path(""), m_messageInProgress(false) {}

OscSender::OscSender(std::string address, unsigned int port) :
	m_address(0), m_message(0), m_path(""), m_messageInProgress(false) {
	setup(address, port);
}

OscSender::~OscSender() {
	if(m_address) {
		lo_address_free(m_address);
	}
}

void OscSender::send() {
	if(!m_address || !m_message || isMessageInProgress()) {
		throw SendException();
	}
	lo_send_message(m_address, m_path.c_str(), m_message);
	lo_message_free(m_message);
	m_path = "";
}

void OscSender::setup(std::string address, unsigned int port) {
	if(m_address) {
		lo_address_free(m_address);
	}
	std::stringstream stream;
	stream << port;
	m_address = lo_address_new(address.c_str(), stream.str().c_str());
}

std::string OscSender::getAddr() {
	return m_address ? (std::string) lo_address_get_hostname(m_address) : "";
}

std::string OscSender::getPort() {
	return m_address ? (std::string) lo_address_get_port(m_address) : "";
}

// MESSAGE BUILDING

void OscSender::beginMessage(std::string addressPattern) {
	if(isMessageInProgress()) {
		throw MessageInProgressException();
	}
	m_message = lo_message_new();
	m_messageInProgress = true;
	m_path = addressPattern;
}
	
void OscSender::addBool(bool var) {
	if(!isMessageInProgress()) {
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
	if(!isMessageInProgress()) {
		throw MessageNotInProgressException();
	}
	lo_message_add_char(m_message, var);
}

void OscSender::addNil() {
	if(!isMessageInProgress()) {
		throw MessageNotInProgressException();
	}
	lo_message_add_nil(m_message);
}

void OscSender::addInfinitum() {
	if(!isMessageInProgress()) {
		throw MessageNotInProgressException();
	}
	lo_message_add_infinitum(m_message);
}

void OscSender::addInt32(int32_t var) {
	if(!isMessageInProgress()) {
		throw MessageNotInProgressException();
	}
	lo_message_add_int32(m_message, var);
}

void OscSender::addInt64(int64_t var) {
	if(!isMessageInProgress()) {
		throw MessageNotInProgressException();
	}
	lo_message_add_int64(m_message, var);
}

void OscSender::addFloat(float var) {
	if(!isMessageInProgress()) {
		throw MessageNotInProgressException();
	}
	lo_message_add_float(m_message, var);
}

void OscSender::addDouble(double var) {
	if(!isMessageInProgress()) {
		throw MessageNotInProgressException();
	}
	lo_message_add_double(m_message, var);
}

void OscSender::addString(char *var) {
	if(!isMessageInProgress()) {
		throw MessageNotInProgressException();
	}
	lo_message_add_string(m_message, var);
}

void OscSender::addString(std::string var) {
	if(!isMessageInProgress()) {
		throw MessageNotInProgressException();
	}
	lo_message_add_string(m_message, var.c_str());
}

void OscSender::addSymbol(const Symbol& var) {
	if(!isMessageInProgress()) {
		throw MessageNotInProgressException();
	}
	lo_message_add_symbol(m_message, var.value);
}

void OscSender::addMidiMessage(const MidiMessage& var) {
	if(!isMessageInProgress()) {
		throw MessageNotInProgressException();
	}
	lo_message_add_midi(m_message, (uint8_t *)var.bytes);
}

void OscSender::addTimeTag(const TimeTag& var) {
	if(!isMessageInProgress()) {
		throw MessageNotInProgressException();
	}
	lo_message_add_timetag(m_message, var.tag);
}

void OscSender::addBlob(const Blob& var) {
	if(!isMessageInProgress()) {
		throw MessageNotInProgressException();
	}
	lo_blob blob = lo_blob_new(var.size, (void *)var.data);
	lo_message_add_blob(m_message, blob);
	lo_blob_free(blob);
}

void OscSender::endMessage() {
	if(!isMessageInProgress()) {
		throw MessageNotInProgressException();
	}
	m_messageInProgress = false;
}

// MESSAGE BUILDING VIA STREAM SENDING

OscSender& OscSender::operator<<(const BeginMessage& var) {
	beginMessage(var.addressPattern);
	return *this;
}

OscSender& OscSender::operator<<(const EndMessage& var) {
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

OscSender& OscSender::operator<<(const Nil& var) {
	addNil();
	return *this;	
}

OscSender& OscSender::operator<<(const Infinitum& var) {
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

OscSender& OscSender::operator<<(const Symbol& var) {
	addSymbol(var);
	return *this;
}

OscSender& OscSender::operator<<(const MidiMessage& var) {
	addMidiMessage(var);
	return *this;
}

OscSender& OscSender::operator<<(const TimeTag& var) {
	addTimeTag(var);
	return *this;
}

OscSender& OscSender::operator<<(const Blob& var) {
	addBlob(var);
	return *this;
}

// UTIL

void OscSender::print() {
	if(m_message) {
		lo_message_pp(m_message);
	}
}

} // namespace
