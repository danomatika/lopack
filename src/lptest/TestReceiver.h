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

#ifndef WIN32
	#include <unistd.h>
#endif

using namespace std;

/// test receiver implementation
class TestReceiver : public osc::OscReceiver {

	public:
		
		TestReceiver() : done(false) {}
		
		/// poll for messages until "/quit" is received
		void poll() {
			done = false;
			while(!done) {
				int num = handleMessages(0);
				if(num > 0) {
					cout << "TestReceiver: received " << num << " bytes" << endl;
				}
				else {
					cout << "TestReceiver: still waiting ..." << endl;
				}
			}
		}
		
		bool done; //< if false, keep polling
		
	protected:

		/// OscReceiver new message callback
		bool process(const osc::ReceivedMessage &message, const osc::MessageSource &source) {
			cout << "TestReceiver: received message " << message.address() << " "
			     << message.types() << " from " << source.getUrl() << endl;

			// exit on /quit message
			if(message.address() == "/quit") {
				done = true;
				return true;
			}

			// print test message
			if(message.checkAddressAndTypes("/test3", "TFcNIihfdsSmtb")) {
				cout << "/test3 parsing all message types" << " " << message.typeTag(0) << endl
					 << "  bool T: " << message.asBool(0) << endl
					 << "  bool F: " << message.asBool(1) << endl
					 << "  char: '" << message.asChar(2) << "'" << endl
					 << "  nil" << endl       // message arg 3
					 << "  infinitum" << endl // message arg 4
					 << "  int32: " << message.asInt32(5) << endl
					 << "  int64: " << message.asInt64(6) << endl
					 << "  float: " << message.asFloat(7) << endl
					 << "  double: " << message.asDouble(8) << endl
					 << "  string: \"" << message.asString(9) << '"' << endl
					 << "  symbol: \"" << message.asSymbol(10) << '"' << endl
					 << "  midi: " << hex << message.asMidiMessage(11) << dec << endl
					 << "  timetag: " << message.asTimeTag(12).sec << " " << message.asTimeTag(12).frac << endl
					 << "  blob: \"" << std::string((char *) message.asBlob(13).data) << '"' << endl;
			}
			else { // print arguments manually
				for(unsigned int i = 0; i < message.numArgs(); ++i) {
					cout << "  arg " << i << " '" << message.typeTag(i) << "' ";
					message.printArg(i);
					cout << endl;
				}
			}
			return true;
		}
};
