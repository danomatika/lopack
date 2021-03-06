/*==============================================================================

	OscTypes.h

	lopack: an oscpack-inspired C++ wrapper for liblo
  
	Copyright (C) 2010 Dan Wilcox <danomatika@gmail.com>
	
	types influenced by oscpack:

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

#include <lo/lo.h>
#include <string>
#include <math.h>
#include <stdexcept>

namespace osc {

/// \section Osc Types

/// Nil value
struct Nil {
	explicit Nil() {}
};

/// Infinitum value
struct Infinitum {
	explicit Infinitum() {}
};

/// a 4 byte MIDI packet
struct MidiMessage {
	union {
		uint8_t bytes[4]; ///< 4 midi bytes
		uint32_t value; //</ midi bytes as an int32
	};
	
	/// constructor
	explicit MidiMessage() : value(0) {}
	
	/// constructor to set the byte values, set reverse = true to reverse the byte order
	explicit MidiMessage(uint8_t bytes_[4], bool reverse=false) {
		if(reverse) {
			bytes[0] = bytes_[3];
			bytes[1] = bytes_[2];
			bytes[2] = bytes_[1];
			bytes[3] = bytes_[0];
		}
		else {
			bytes[0] = bytes_[0];
			bytes[1] = bytes_[1];
			bytes[2] = bytes_[2];
			bytes[3] = bytes_[3];
		}
	}

	operator uint32_t() const {return value;} ///< operator to grab the values implicitly
};

/// an OSC TimeTag value
struct TimeTag {
	union {
		struct {
			uint32_t sec;  ///< the number of seconds since Jan 1 1900 UTC
			uint32_t frac; ///< the fractions of a second offset from above, in 1/2^32nds of a second
		};
		lo_timetag tag; ///< the liblo timetag structure (holds the same data)
	};

	/// constructor, sets immediate time (now)
	TimeTag();
	
	/// constructor to set time ahead by a number of milliseconds
	TimeTag(unsigned int ms);
	
	/// constructor to set time from liblo timetag
	TimeTag(lo_timetag timetag);

	/// constructor to set time
	explicit TimeTag(uint32_t sec_, uint32_t frac_) : sec(sec_), frac(frac_) {}
	
	/// comparison operator, returns true if the timestamps are equal
	bool operator==(const TimeTag &tag) const;

	/// subtraction operator, returns difference bewteen timestamps in seconds
	double operator-(const TimeTag &tag) const;
	
	/// set the current time (not immediate)
	void now();

	/// add the number of milliseconds to the current timestamp
	void add(unsigned int ms);
	
	/// get the difference between this timetag and the current time in seconds
	double diff() const;
	
	/// get the difference between this timetag and another in seconds
	double diff(const TimeTag &tag) const;
};

/// standard C, NULL terminated, string. Used in systems which distinguish strings and symbols./
struct Symbol {
	const char *value; ///< the C-string

	/// constructor
	explicit Symbol() : value(NULL) {}
	
	/// constructors to set the string
	explicit Symbol(const char *value_) : value(value_) {}
	explicit Symbol(const std::string value_) : value(value_.c_str()) {}

	operator const char*() const {return value;} ///< operator to grab the string implicitly
};

/// a binary area of memory
struct Blob {
	const void *data; ///< pointer to the data
	uint32_t size;    ///< size of the data
	
	/// constructor
	explicit Blob() : data(NULL), size(0) {}
	
	/// constructor to set the data pointer and data size
	explicit Blob(const void *data_, uint32_t size_) : data(data_), size(size_) {}
};

/// \section Stream Manipulators

/// start a message bundle
struct BeginBundle {

	TimeTag timetag; ///< the timetag of this bundle
	
	/// use immediate time tag
	explicit BeginBundle() : timetag() {}
	
	/// use a given time tag
	explicit BeginBundle(TimeTag timetag) : timetag(timetag) {}
};

/// end a message bundle
struct EndBundle {};

/// start a message
struct BeginMessage {

	const std::string addressPattern; ///< the message address

	/// set the message target address
	explicit BeginMessage(const std::string addressPattern_) : addressPattern(addressPattern_) {}
};

/// end a message
struct EndMessage {
	explicit EndMessage() {}
};

/// \section Received Message

/// \class TypeException
/// \brief bad type exception
class TypeException : public std::runtime_error {
	public:
		TypeException(
			const char *w="argument is of different type then requested")
			: std::runtime_error(w) {}
};

/// \class ArgException
/// \brief bad argument index exception
class ArgException : public std::runtime_error {
	public:
		ArgException(
			const char *w="argument index out of range")
			: std::runtime_error(w) {}
};

/// \class ReceivedMessage
/// \brief an osc message with built in message parsing
class ReceivedMessage {

	public:
	
		/// constructor:
		/// osc address pattern 
		/// message the liblo message
		/// note: performs a *shallow copy* of the underlying liblo message
		///       which is reference counted
		ReceivedMessage(std::string addressPattern, lo_message message);
	
	/// \section Info
	
		/// returns true if the message matches the given address and argument type string
		const bool checkAddressAndTypes(std::string addressPattern, std::string types) const;
		
		/// get the message address pattern
		const std::string address() const;
		
		/// get the argument type string
		const std::string types() const;
		
		/// get the type tag character of a given argument index
		const char typeTag(unsigned int at) const;
	
		/// get the number of arguments in the message
		const unsigned int numArgs() const;
	
		/// get the message's time tag when received
		const TimeTag getTimeTag() const;
	
	/// \section Read Arguments
		
		/// check if an argument at a given index is of a certain type
		/// throws an exception on bad index
		const bool isBool(unsigned int at) const;
		const bool isChar(unsigned int at) const;
		const bool isNil(unsigned int at) const;
		const bool isInfinitum(unsigned int at) const;
		
		const bool isInt(unsigned int at) const; ///< either int32 or int64
		const bool isInt32(unsigned int at) const;
		const bool isInt64(unsigned int at) const;
		
		const bool isFloat(unsigned int at) const;
		const bool isDouble(unsigned int at) const;
		
		const bool isText(unsigned int at) const; ///< either string or symbol
		const bool isString(unsigned int at) const;
		const bool isSymbol(unsigned int at) const;
		
		const bool isMidiMessage(unsigned int at) const;
		const bool isTimeTag(unsigned int at) const;
		const bool isBlob(unsigned int at) const;
		
		/// get an argument at a given index as a certain type
		/// throws an exception on bad index or type
		const bool asBool(unsigned int at) const;
		const unsigned char asChar(unsigned int at) const;
		
		const int32_t asInt32(unsigned int at) const;
		const int64_t asInt64(unsigned int at) const;
		
		const float	asFloat(unsigned int at) const;
		const double asDouble(unsigned int at) const;
		
		const std::string asString(unsigned int at) const;
		const Symbol asSymbol(unsigned int at) const;
		
		const MidiMessage asMidiMessage(unsigned int at) const;
		const TimeTag asTimeTag(unsigned int at) const;
		const Blob asBlob(unsigned int at) const;

		/// try to get an argument as a given type, fail silently
		const bool tryBool(bool *dest, unsigned int at) const; // include numbers
		const bool tryChar(char *dest, unsigned int at) const; // includes ints
		const bool tryNumber(int *dest, unsigned int at) const;
		const bool tryNumber(unsigned int *dest, unsigned int at) const;
		const bool tryNumber(float *dest, unsigned int at) const;
		const bool tryNumber(double *dest, unsigned int at) const;
		const bool tryString(std::string *dest, unsigned int at) const; // includes Symbol
	
	/// \section Util
	
		/// get the raw liblo argument at a given index
		/// throws an exception on bad index
		const lo_arg *arg(unsigned int at) const;
	
		/// pretty print the message to std:cout on a single line
		const void print() const;
	
		/// pretty print an argument at a given index to std::cout
		const void printArg(unsigned at) const;
		
		/// pretty print all arguments in the message to std::cout
		const void printAllArgs() const;
	
		/// get the underlying liblo message
		inline const lo_message message() const {return m_message;}
		
	private:
	
		std::string m_addressPattern; ///< osc message address pattern
		lo_message  m_message; ///< liblo message
};

/// \class MessageSource
/// \brief a class containing the host address of a message sender
class MessageSource {

	public:
	
		/// constructor:
		/// address liblo address to wrap
		MessageSource(lo_address address);
		
		const std::string getHostname() const; ///< get the hostname
		const std::string getPort() const;     ///< get the port
		const std::string getUrl() const;      ///< get the url of the host
	
		/// print to std::cout
		const void print() const;
	
	private:
		
		lo_address m_address; ///< liblo host address
};

} // namespace
