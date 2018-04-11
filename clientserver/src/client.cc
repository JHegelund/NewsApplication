#include "../protocol.h"
#include "../connection.h"
#include "../connectionclosedexception.h"
#include "messagehandler.h"
#include "newsgroup.h"

#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <sstream>
#include <vector>

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

/*
 * Writes and receives list newsgroups command to and from server
 */
void listNG(const Connection& conn){
	MessageHandler::sendCode(conn, Protocol::COM_LIST_NG);
	MessageHandler::sendCode(conn, Protocol::COM_END);

	if(MessageHandler::recvCode(conn) == Protocol::ANS_LIST_NG){
		vector<Newsgroup> ngs;
		int num = MessageHandler::recvIntParameter(conn);
		for(int i = 0; i < num; i++){
			int id = MessageHandler::recvIntParameter(conn);
			string name = MessageHandler::recvStringParameter(conn);
			Newsgroup ng(id, name);
			ngs.push_back(ng);
		}
	} else {

	}

	/*
	 * Output newsgroups to client
	 */
	if(MessageHandler::recvCode(conn) != Protocol::ANS_END){
		cout << "Protocol Violation" << endl;
	} else {
		for(Newsgroup& ng : ngs){
			cout << ng.id << " " << ng.name << endl;
		}
	}
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
	cout << "Create newsgroup <name> = create new newsgroup with specified name" << endl;
	cout << "Delete newsgroup <name> = delete newsgroup with specified name" << endl;
	cout << "List articles <newsgroup> = list all articles in specified newsgroup" << endl;
	cout << "Read <newsgroup> <article> = read an article in a newsgroup" << endl;
	cout << "Write <newsgroup> <article> = write article in a newsgroup" << endl;
	cout << "Delete <newsgroup> <article> = delete article in a newsgroup" << endl;
	string cmd;
	while (true) {
		getline(cin, cmd);
		try {
			if(cmd == "List newsgroups"){
				cout << "Listing newgroups...." << endl;
				listNG(conn);
			}
		} catch (ConnectionClosedException&) {
			cout << " no reply from server. Exiting." << endl;
			exit(1);
		}
	}
}

