/*==============================================================================

	TestReceiver.h

	lptest: lopack test program
  
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
#include <lopack/lopack.h>
#include <iostream>
#include <unistd.h>

using namespace std;
using namespace osc;

/// test receiver implementation
class TestReceiver : public OscReceiver {

	public:
		
		TestReceiver() : OscReceiver(), bDone(false) {}
		
		// poll for messages until "/quit" is received
		void poll() {
			bDone = false;
			while(!bDone) {
				int num = handleMessages(0);
				if(num > 0) {
					cout << "TestReceiver: received " << num << " bytes" << endl;
				}
				else {
					cout << "TestReceiver: still waiting ..." << endl;
				}
			}
		}
		
		bool bDone; //< if false, keep polling
		
	protected:

		bool process(const ReceivedMessage& message, const MessageSource& source) {
			cout << "TestReceiver: received message " << message.path() << " " << message.types() << endl;

			/// print test message
			if(message.checkPathAndTypes("/test3", "TFcNIihfdsSmtb")) {
				cout << "/test3 parsing all mesage types" << " " << message.typeTag(0) << endl
					 << " bool T: " << message.asBool(0) << endl
					 << " bool F: " << message.asBool(1) << endl
					 << " char: '" << message.asChar(2) << "'" << endl
					 << " nil" << endl       // message arg 3
					 << " infinitum" << endl // message arg 4
					 << " int32: " << message.asInt32(5) << endl
					 << " int64: " << message.asInt64(6) << endl
					 << " float: " << message.asFloat(7) << endl
					 << " double: " << message.asDouble(8) << endl
					 << " string: \"" << message.asString(9) << '"' << endl
					 << " symbol: \"" << message.asSymbol(10) << '"' << endl
					 << " midi: " << hex << message.asMidiMessage(11) << dec << endl
					 << " timetag: " << message.asTimeTag(12).sec << " " << message.asTimeTag(12).frac << endl
					 << " blob: \"" << std::string((char*) message.asBlob(13).data) << '"' << endl;
				return true;
			}
			
			/// print arguments
			for(unsigned int i = 0; i < message.numArgs(); ++i) {
				cout << "arg " << i << " '" << message.typeTag(i) << "' ";
				message.printArg(i);
				cout << endl;
			}
			
			/// exit on /quit message
			if(message.path() == "/quit") {
				bDone = true;
			}
 
			return true;
		}
};
