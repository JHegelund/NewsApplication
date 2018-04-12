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

/*
 * List articles within specified newgroup
 */
void listArticles(const Connection& conn, int ngID){
	MessageHandler::sendCode(conn, Protocol::COM_LIST_ART);
	MessageHandler::sendIntParameter(conn, ngID);
	MessageHandler::sendCode(conn, Protocol::COM_END);

	if(MessageHandler::recvCode(conn) == Protocol::ANS_LIST_ART){
		int ans = MessageHandler::recvCode(conn);
		if(code == Protocol::ANS_ACK){
			vector<Article> arts;
			int num = MessageHandler::recvIntParameter(conn);
			for(int i = 0; i < num; i++){
				int id = MessageHandler::recvIntParameter(conn);
				string name = MessageHandler::recvStringParameter(conn);
				Article art(id, name);
				arts.push_back(art);
			}
		} else if(code == Protocol::ANS_NACK){
			int nack = MessageHandler::recvCode(conn);
			if(nack == Protocol::ERR_NG_DOES_NOT_EXIST){
				cout << "Error NG does not exist" << endl;
			}
		}
		
		if(MessageHandler::recvCode(conn) != Protocol::ANS_END){
			cout << "Protocol Violation" << endl;
		} else {
			for(Article& a : arts){
				cout << a.id << " " << a.name << endl;
			}
		}
	}
}

/*
 * Create newgroup with specified name
 */
void createNG(const Connection& conn, string name){
	MessageHandler::sendCode(conn, Protocol::COM_CREATE_NG);
	MessageHandler::sendStringParameter(conn, name);
	MessageHandler::sendCode(conn, Protocol::COM_END);

	if(MessageHandler::recvCode(conn) == Protocol::ANS_CREATE_NG){
		int ans = MessageHandler::recvCode(conn);
		if(ans == Protocol::ANS_ACK){
			if(MessageHandler::recvCode(conn) == ANS_END){
				cout << "Article added" << endl;
			} else {
				cout << "Protocol Violation" << endl;
			}
		} else if(ans == Protocol::ANS_NACK){
			if(MessageHandler::recvCode(conn) == Protocol::ERR_NG_ALREADY_EXIST){
				cout << "Newgroup already exist" << endl;
			} else {
				cout << "Protocol Violation" << endl;
			}
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
	cout << "Create newsgroup <newsgroup_name> = create new newsgroup with specified name" << endl;
	cout << "Delete newsgroup <newsgroup_name> = delete newsgroup with specified name" << endl;
	cout << "List articles <newsgroup_id> = list all articles in specified newsgroup id" << endl;
	cout << "Read <newsgroup_id> <article_id> = read an article in a newsgroup" << endl;
	cout << "Write <newsgroup_id> <article_name> = write article in a newsgroup" << endl;
	cout << "Delete <newsgroup_id> <article_id> = delete article in a newsgroup" << endl;
	string cmd;
	while (cin >> cmd) {
		try {
			if(cmd == "List"){
				cin >> cmd;
				if(cmd == "newsgroups"){
					cout << "Listing newgroups...." << endl;
					listNG(conn);
				} else if(cmd == "articles"){
					//Read newsgroup id
					cin >> cmd;
					int id = stoi(cmd);
					//List articles within this newsgroup
					listArticles(conn, id);
				}
			} else if (cmd == "Create"){
				cin >> cmd;
				if(cmd == "newsgroup"){
					cin >> cmd;
					createNG(conn, cmd);
				}

			}
		} catch (ConnectionClosedException&) {
			cout << " no reply from server. Exiting." << endl;
			exit(1);
		}
	}
}

