/*==============================================================================

	main.cpp

	oftest: a test for oscframework
  
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

void testTimeTag();
void testSender();

int main(int argc, char *argv[]) {

	cout << endl;
	cout << "TIMETAG TEST" << endl;
	testTimeTag();
	cout << "DONE" << endl << endl;

	TestReceiver receiver;
	receiver.setup(9990);
	
	usleep(2000000); // 2 seconds
	
	cout << "RECEIVER TEST (NO THREAD)" << endl;
	testSender();
	receiver.poll();
	cout << "DONE" << endl << endl;

	cout << "RECEIVER TEST (THREAD)" << endl;
	receiver.start();
	testSender();
	usleep(1000000); // 1 second
	receiver.stop();
	cout << "DONE" << endl << endl;

	return 0;
}

void testTimeTag() {
	TimeTag tagA;
	cout << "tagA is " << tagA.sec << " " << tagA.frac << " (now)"<< endl;

	cout << "sleeping 20 milliseconds ..." << endl;
	usleep(20000);

	TimeTag tagB;
	cout << "tagB is " << tagB.sec << " " << tagB.frac << " (now)" << endl;

	// check difference (usleep is not so accurate, so difference will be ~ 5 usecs)
	cout << "tagB-tagA = " << tagB-tagA << "s" << endl;

	tagB.now();
	TimeTag tagC(25); // timestamp ahead 25 ms
	cout << "tagC is " << tagC.sec << " " << tagC.frac << endl;

	// check difference, should be now - now+25ms = 25ms or 0.025s
	cout << "tagC-tagB = " << tagC-tagB << "s" << endl; 
}

void testSender() {
	OscSender sender;
	
	sender.setup("127.0.0.1", 9990);
	
	// send a quick message
	sender << osc::BeginMessage("/test1")
		   << (bool) 1 << 40.0f << (float) 1024.3434 << Nil() << (std::string) "string one" << "string two"
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
		   << m << Blob(blobData.c_str(), sizeof(char)*(blobData.length()+1))
		   << osc::EndMessage();
	sender.send();
	
	// send a message with all types to be parsed on server
	sender << osc::BeginMessage("/test3")
		<< true                 // bool true
		<< false                // bool false
		<< 'c'                  // char
		<< Nil()                // nil
		<< Infinitum()          // infinitum
		<< (int32_t) 100        // int32
		<< (int64_t) 200        // int32
		<< (float) 123.45       // float
		<< (double) 678.90      // double
		<< "a string"           // string
		<< Symbol("a symbol")   // symbol (NULL-terminated C-string)
		<< m                    // midi message
		<< TimeTag()            // time tag (right now)
		<< Blob(blobData.c_str(), sizeof(char)*(blobData.length()+1)) // binary blob data
		<< osc::EndMessage();
	sender.send();
	
	// send quit message
	sender << osc::BeginMessage("/quit") << osc::EndMessage();
	sender.send();
}
