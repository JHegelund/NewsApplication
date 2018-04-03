#include "../connection.h"
#include "../connectionclosedexception.h"

#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>

using namespace std;

/*
 * Send an integer to the server as four bytes.
 */
void writeNumber(const Connection& conn, int value) {
	conn.write((value >> 24) & 0xFF);
	conn.write((value >> 16) & 0xFF);
	conn.write((value >> 8)	 & 0xFF);
	conn.write(value & 0xFF);
}

/*
 * Read a string from the server.
 */
string readString(const Connection& conn) {
	string s;
	char ch;
	while ((ch = conn.read()) != '$') {
		s += ch;
	}
	return s;
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		cerr << "Usage: myclient host-name port-number" << endl;
		exit(1);
	}
	
	int port = -1;
	try {
		port = stoi(argv[2]);
	} catch (exception& e) {
		cerr << "Wrong port number. " << e.what() << endl;
		exit(1);
	}
	
	Connection conn(argv[1], port);
	if (!conn.isConnected()) {
		cerr << "Connection attempt failed" << endl;
		exit(1);
	}
	
	cout << "Command:" << endl;
	cout << "List newgroups = list all newsgroups" << endl;
	cout << "Create newsgroup name = create new newsgroup with specified name" << endl;
	cout << "Delete newsgroup name = delete newsgroup with specified name" << endl;
	cout << "List articles newsgroup = list all articles in specified newsgroup" << endl;
	cout << "Read article = read an article in a newsgroup" << endl;
	cout << "Write article = write article in a newsgroup" << endl;
	cout << "Delete article = delete article in a newsgroup" << endl;
	string cmd;
	while (cin >> cmd) {
		try {
			if(cmd == "List newsgroup"){
				cout << "Listing newgroups...." << endl;
			}
		} catch (ConnectionClosedException&) {
			cout << " no reply from server. Exiting." << endl;
			exit(1);
		}
	}
}

