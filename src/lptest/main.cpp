/*==============================================================================

	main.cpp

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
#include "TestReceiver.h"

// sleep in seconds
#ifdef WIN32
	#define SLEEP(seconds) Sleep(seconds)
#else
	#define SLEEP(seconds) usleep(seconds*1000000)
#endif

// OscObject subclass which recieves messages automatically when added to
// OscReceiver
class Object : public osc::OscObject {

	public:
	
		// init with /object as the osc root address
		Object() : osc::OscObject("/object") {}
	
	protected:
	
		// return true when the message was handled
		bool processOscMessage(const osc::ReceivedMessage &message, const osc::MessageSource &source) {
			if(message.checkAddressAndTypes(oscRootAddress, "ifs")) {
				cout << "Object: received message " << message.address() << " "
				     << message.types() << " from " << source.getUrl() << endl;
				cout << "  " << message.asInt32(0) << " " << message.asFloat(1) << " " << message.asString(2) << endl;
				return true;
			}
			return false;
		}
};

void testTimeTag() {
	osc::TimeTag tagA;
	cout << "tagA is " << tagA.sec << " " << tagA.frac << " (now)"<< endl;

	cout << "sleeping ~20 milliseconds ..." << endl;
	SLEEP(0.02);

	osc::TimeTag tagB;
	cout << "tagB is " << tagB.sec << " " << tagB.frac << " (now)" << endl;

	// check difference (usleep is not so accurate, so difference will be ~ 5 usecs)
	cout << "tagB-tagA = " << tagB-tagA << "s" << endl;

	tagB.now();
	osc::TimeTag tagC(25); // timestamp ahead 25 ms
	cout << "tagC is " << tagC.sec << " " << tagC.frac << endl;

	// check difference, should be now - now+25ms = 25ms or 0.025s
	cout << "tagC-tagB = " << tagC-tagB << "s" << endl; 
}

void testSender() {

	osc::OscSender sender;
	sender.setup("127.0.0.1", 9990);
	cout << "sending to " << sender.getUrl() << endl;
	
	// send a quick message
	sender << osc::BeginMessage("/test1")
	       << (bool) 1 << 40.0f << (float) 1024.3434 << osc::Nil() << (std::string) "string one" << "string two"
	       << osc::EndMessage();
	sender.send();
	
	// send a midi message and a blob (binary data)
	osc::MidiMessage m;
	m.bytes[0] = 0x7F;
	m.bytes[1] = 0x90;
	m.bytes[2] = 0x3E;
	m.bytes[3] = 0x60;
	string blobData = "this is some blob data";
	sender << osc::BeginMessage("/test2")
	       << m << osc::Blob(blobData.c_str(), sizeof(char)*(blobData.length()+1))
	       << osc::EndMessage();
	sender.send();
	
	// send a message with all types to be parsed on server
	sender << osc::BeginMessage("/test3")
	       << true                    // bool true
	       << false                   // bool false
	       << 'c'                     // char
	       << osc::Nil()              // nil
	       << osc::Infinitum()        // infinitum
	       << (int32_t) 100           // int32
	       << (int64_t) 200           // int32
	       << (float) 123.45          // float
	       << (double) 678.90         // double
	       << "a string"              // string
	       << osc::Symbol("a symbol") // symbol (NULL-terminated C-string)
	       << m                       // midi message
	       << osc::TimeTag()          // time tag (right now)
	       << osc::Blob(blobData.c_str(), sizeof(char)*(blobData.length()+1)) // binary blob data
	       << osc::EndMessage();
	sender.send();
	
	// send multiple messages nested within bundles
	sender << osc::BeginBundle();
	sender << osc::BeginMessage("/test4")
	       << false << 100 << 200.f << "kraaa"
	       << osc::EndMessage()
	       << osc::BeginBundle(osc::TimeTag(10)) // 10 ms in the future
	           << osc::BeginMessage("/test5") << "foo" << "bar" << osc::EndMessage()
	       << osc::EndBundle();
	sender << osc::EndBundle();
	sender.send();
	
	// send message to be handled by Object class
	sender << osc::BeginMessage("/object") << 123 << 456.78f << "foo bar" << osc::EndMessage();
	sender.send();
	
	// send quit message
	sender << osc::BeginMessage("/quit") << osc::EndMessage();
	sender.send();
}

int main(int argc, char *argv[]) {

	cout << endl;
	cout << "TIMETAG TEST" << endl;
	testTimeTag();
	cout << "DONE" << endl << endl;

	TestReceiver receiver;
	receiver.setup(9990);
	cout << "receiver started: " << receiver.getUrl() << endl;
	
	Object object;
	receiver.addOscObject(&object);
	
	SLEEP(2);
	
	try {
		cout << "RECEIVER TEST (NO THREAD)" << endl;
		testSender();
		SLEEP(1);
		receiver.poll();
		cout << "DONE" << endl << endl;

		cout << "RECEIVER TEST (THREAD)" << endl;
		receiver.start();
		testSender();
		SLEEP(1);
		receiver.stop();
		cout << "DONE" << endl << endl;
	}
	catch(osc::ReceiveException e) {
		cout << "CAUGHT EXCEPTION: "<< e.what() << endl;
	}
	
	return 0;
}
