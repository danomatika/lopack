/*==============================================================================

	OscObject.cpp

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
#include "OscObject.h"

#include "Log.h"
#include <algorithm>

namespace osc {

bool OscObject::processOsc(const ReceivedMessage &message, const MessageSource &source) {
	// call any attached objects
	std::vector<OscObject*>::iterator iter;
	for(iter = m_objects.begin(); iter != m_objects.end();) {
		if((*iter) != NULL) { // try to process message
			if((*iter)->processOsc(message, source)) {
				return true;
			}
			iter++; // increment iter
		}
		else { // bad object, so erase it
			iter = m_objects.erase(iter);
			LOG_WARN << "OscObject: removed NULL object" << std::endl;
		}
	}
	return processOscMessage(message, source);
}

void OscObject::addOscObject(OscObject *object) {
	if(object == NULL) {
		LOG_WARN << "OscObject: cannot add NULL object" << std::endl;
		return;
	}
	m_objects.push_back(object);
}

void OscObject::removeOscObject(OscObject *object) {
	if(object == NULL) {
		LOG_WARN << "OscObject: cannot remove NULL object" << std::endl;
		return;
	}
	// find object in list and remove it
	std::vector<OscObject *>::iterator iter;
	iter = find(m_objects.begin(), m_objects.end(), object);
	if(iter != m_objects.end()) {
		m_objects.erase(iter);
	}
}

void OscObject::removeAllOscObjects() {
	m_objects.clear();
}

} // namespace
