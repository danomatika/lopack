/*==============================================================================

	OscObject.h

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

#include <vector>
#include "OscTypes.h"

namespace osc {

/// \class OscObject
/// \brief derive this class to add to an OscListener
///
/// subclass and implement the processing function to receive incoming messages
class OscObject {
	
	public:

		OscObject(std::string rootAddress="") : oscRootAddress(rootAddress) {}

		/// \section Message Processing

		/// process attached objects, then call processOscMessage
		/// returns true if message handled
		bool processOsc(const ReceivedMessage& message, const MessageSource& source);

		/// \section Objects

		/// attach/remove an OscObject to this one
		void addOscObject(OscObject* object);
		void removeOscObject(OscObject* object);

		/// \section Util

		/// get/set the root address of this object
		inline void setOscRootAddress(std::string rootAddress) {oscRootAddress = rootAddress;}
		inline std::string& getOscRootAddress() {return oscRootAddress;}
		inline void prependOscRootAddress(std::string prepend) {oscRootAddress = prepend + oscRootAddress;}

	protected:

		/// callback to implement, returns true if message handled
		virtual bool processOscMessage(const ReceivedMessage& message, const MessageSource& source) {return false;}

		/// the root address of this object, aka something like "/root/test1/string2"
		std::string oscRootAddress;

	private:

		std::vector<OscObject*> _objectList; //< currently nested objects
};

} // namespace
