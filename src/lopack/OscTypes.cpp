/*==============================================================================

	OscTypes.cpp

	lopack: an oscpack-inspired C++ wrapper for liblo
  
	Copyright (C) 2010 Dan Wilcox <danomatika@gmail.com>

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
#include "OscTypes.h"

#include <iostream>
#include <exception>
#include <lo/lo.h>

namespace osc {

// TIME TAG

TimeTag::TimeTag() {
	now();
}

TimeTag::TimeTag(unsigned int ms) {
	now();
	add(ms);
}

TimeTag::TimeTag(lo_timetag timetag) {
	tag = tag;
}

bool TimeTag::operator==(const TimeTag &tag) const {
	return (sec == tag.sec) && (frac == tag.frac);
}

double TimeTag::operator-(const TimeTag &tag) const {
	return lo_timetag_diff(this->tag, tag.tag);
}

void TimeTag::now() {
	lo_timetag_now(&tag);
}

void TimeTag::add(unsigned int ms) {
	tag.sec += ms / 1000; // seconds
	tag.frac += ((ms%1000)*0.001) / 0.00000000023283064365; // 1/2^32nds of a second
}

double TimeTag::diff() const {
	lo_timetag now;
	lo_timetag_now(&now);
	return lo_timetag_diff(now, this->tag);
}

double TimeTag::diff(const TimeTag &tag) const {
	return lo_timetag_diff(this->tag, tag.tag);
}

// RECEIVED MESSAGE

ReceivedMessage::ReceivedMessage(std::string addressPattern, lo_message message) :
	m_addressPattern(addressPattern), m_message(message) {
	lo_message_incref(m_message); // increment reference count
}

const bool ReceivedMessage::checkAddressAndTypes(std::string address, std::string types) const {
	return (address == m_addressPattern && types == this->types());
}

const std::string ReceivedMessage::address() const {
	return m_addressPattern;
}

const std::string ReceivedMessage::types() const {
	return lo_message_get_types(m_message);
}

const char ReceivedMessage::typeTag(unsigned int at) const {
	return (at < numArgs()) ? types()[at] : '*';
}

const unsigned int ReceivedMessage::numArgs() const {
	return lo_message_get_argc(m_message);
}

const TimeTag ReceivedMessage::getTimeTag() const {
	return TimeTag(lo_message_get_timestamp(m_message));
}

const bool ReceivedMessage::isBool(unsigned int at) const {return typeTag(at) == 'T' || typeTag(at) == 'F';}
const bool ReceivedMessage::isChar(unsigned int at) const {return typeTag(at) == 'c';}
const bool ReceivedMessage::isNil(unsigned int at) const {return typeTag(at) == 'N';}
const bool ReceivedMessage::isInfinitum(unsigned int at) const {return typeTag(at) == 'I';}

const bool ReceivedMessage::isInt(unsigned int at) const {return typeTag(at) == 'i' || typeTag(at) == 'h';}
const bool ReceivedMessage::isInt32(unsigned int at) const {return typeTag(at) == 'i';}
const bool ReceivedMessage::isInt64(unsigned int at) const {return typeTag(at) == 'h';}

const bool ReceivedMessage::isFloat(unsigned int at) const {return typeTag(at) == 'f';}
const bool ReceivedMessage::isDouble(unsigned int at) const {return typeTag(at) == 'd';}

const bool ReceivedMessage::isText(unsigned int at) const {return typeTag(at) == 's' || typeTag(at) == 'S';}
const bool ReceivedMessage::isString(unsigned int at) const {return typeTag(at) == 's';}
const bool ReceivedMessage::isSymbol(unsigned int at) const {return typeTag(at) == 'S';}

const bool ReceivedMessage::isMidiMessage(unsigned int at) const {return typeTag(at) == 'm';}
const bool ReceivedMessage::isTimeTag(unsigned int at) const {return typeTag(at) == 't';}
const bool ReceivedMessage::isBlob(unsigned int at) const {return typeTag(at) == 'b';}

const bool ReceivedMessage::asBool(unsigned int at) const {
	if(!isBool(at)) {
		throw TypeException();
	}	
	return typeTag(at) == 'T'; // false: typeTag == 'F'
}

const unsigned char ReceivedMessage::asChar(unsigned int at) const {
	if(!isChar(at)) {
		throw TypeException();
	}
	return arg(at)->c;
}

const int32_t ReceivedMessage::asInt32(unsigned int at) const {
	if(!isInt32(at)) {
		throw TypeException();
	}
	return arg(at)->i;
}

const int64_t ReceivedMessage::asInt64(unsigned int at) const {
	if(!isInt64(at)) {
		throw TypeException();
	}
	return arg(at)->h;
}

const float	ReceivedMessage::asFloat(unsigned int at) const {
	if(!isFloat(at)) {
		throw TypeException();
	}
	return arg(at)->f;
}

const double ReceivedMessage::asDouble(unsigned int at) const {
	if(!isDouble(at)) {
		throw TypeException();
	}
	return arg(at)->d;
}

const std::string ReceivedMessage::asString(unsigned int at) const {
	if(!isString(at)) {
		throw TypeException();
	}
	return std::string(&(arg(at)->s));
}

const MidiMessage ReceivedMessage::asMidiMessage(unsigned int at) const {
	if(!isMidiMessage(at)) {
		throw TypeException();
	}
	return MidiMessage((uint8_t *)arg(at)->m, true); // rev byte order
}

const Symbol ReceivedMessage::asSymbol(unsigned int at) const {
	if(!isSymbol(at)) {
		throw TypeException();
	}
	return Symbol(&(arg(at)->S));
}

const TimeTag ReceivedMessage::asTimeTag(unsigned int at) const {
	if(!isTimeTag(at)) {
		throw TypeException();
	}
	return TimeTag(arg(at)->t.sec, arg(at)->t.frac);
}

const Blob ReceivedMessage::asBlob(unsigned int at) const {
	lo_blob b = (lo_blob) arg(at);
	return Blob(lo_blob_dataptr(b), lo_blob_datasize(b));
}

const bool ReceivedMessage::tryBool(bool *dest, unsigned int at) const {
	if(isBool(at)) {
		*dest = asBool(at);
		return true;
	}
	else if(isInt32(at)) {
		*dest = (bool) asInt32(at);
		return true;
	}
	else if(isInt64(at)) {
		*dest = (bool) asInt64(at);
		return true;
	}
	else if(isFloat(at)) {
		*dest = (bool) asFloat(at);
		return true;
	}
	else if(isDouble(at)) {
		*dest = (bool) asDouble(at);
		return true;
	}
	return false;
}

const bool ReceivedMessage::tryChar(char *dest, unsigned int at) const {
	if(isChar(at)) {
		*dest = asChar(at);
		return true;
	}
	else if(isInt32(at)) {
		*dest = (char) asInt32(at);
		return true;
	}
	else if(isInt64(at)) {
		*dest = (char) asInt64(at);
		return true;
	}
	return false;
}

const bool ReceivedMessage::tryNumber(int *dest, unsigned int at) const {
	if(isBool(at)) {
		*dest = (int) asBool(at);
		return true;
	}
	else if(isInt32(at)) {
		*dest = asInt32(at);
		return true;
	}
	else if(isInt64(at)) {
		*dest = asInt64(at);
		return true;
	}
	else if(isFloat(at)) {
		*dest = (int) asFloat(at);
		return true;
	}
	else if(isDouble(at)) {
		*dest = (int) asDouble(at);
		return true;
	}
	return false;
}

const bool ReceivedMessage::tryNumber(unsigned int *dest, unsigned int at) const {
	if(isBool(at)) {
		*dest = (unsigned int) asBool(at);
		return true;
	}
	else if(isInt32(at)) {
		*dest = (unsigned int) asInt32(at);
		return true;
	}
	else if(isInt64(at)) {
		*dest = (unsigned int) asInt64(at);
		return true;
	}
	else if(isFloat(at)) {
		*dest = (unsigned int) asFloat(at);
		return true;
	}
	else if(isDouble(at)) {
		*dest = (unsigned int) asDouble(at);
		return true;
	}
	return false;
}

const bool ReceivedMessage::tryNumber(float *dest, unsigned int at) const {
	if(isBool(at)) {
		*dest = (float) asBool(at);
		return true;
	}
	else if(isInt32(at)) {
		*dest = (float) asInt32(at);
		return true;
	}
	else if(isInt64(at)) {
		*dest = (float) asInt64(at);
		return true;
	}
	else if(isFloat(at)) {
		*dest = asFloat(at);
		return true;
	}
	else if(isDouble(at)) {
		*dest = (float) asDouble(at);
		return true;
	}
	return false;
}

const bool ReceivedMessage::tryNumber(double *dest, unsigned int at) const {
	if(isBool(at)) {
		*dest = (double) asBool(at);
		return true;
	}
	else if(isInt32(at)) {
		*dest = (double) asInt32(at);
		return true;
	}
	else if(isInt64(at)) {
		*dest = (double) asInt64(at);
		return true;
	}
	else if(isFloat(at)) {
		*dest = (double) asFloat(at);
		return true;
	}
	else if(isDouble(at)) {
		*dest = asDouble(at);
		return true;
	}
	return false;
}

const bool ReceivedMessage::tryString(std::string *dest, unsigned int at) const {
	if(isString(at)) {
		*dest = asString(at);
		return true;
	}
	else if(isSymbol(at)) {
		*dest = (std::string) asSymbol(at).value;
		return true;
	}
	return false;
}

const lo_arg* ReceivedMessage::arg(unsigned int at) const {
	if(at < numArgs()) {
		lo_arg **argv = lo_message_get_argv(m_message);
		return argv[at];
	}
	throw ArgException(); // shouldn't be here
}

const void ReceivedMessage::print() const {
	std::cout << m_addressPattern << " ";
	lo_message_pp(m_message);
}

const void ReceivedMessage::printArg(unsigned at) const {
	lo_arg_pp((lo_type) typeTag(at), (lo_arg *)arg(at));
}

const void ReceivedMessage::printAllArgs() const {
	int argc = lo_message_get_argc(m_message);
	lo_arg **argv = lo_message_get_argv(m_message);
	for(int i = 0; i < argc; ++i) {
		lo_arg_pp((lo_type) typeTag(i), (lo_arg *)argv[i]);
	}
}

// MESSAGE SOURCE

MessageSource::MessageSource(lo_address address) : m_address(address) {}
		
const std::string MessageSource::getHostname() const {return lo_address_get_hostname(m_address);}
const std::string MessageSource::getPort() const {return lo_address_get_port(m_address);}
const std::string MessageSource::getUrl() const {return	lo_address_get_url(m_address);}

const void MessageSource::print() const {
	std::cout << getHostname() << " " << getPort() << std::endl;
}

} // namespace
