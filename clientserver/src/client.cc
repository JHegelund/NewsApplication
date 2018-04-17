#include "../protocol.h"
#include "../connection.h"
#include "../connectionclosedexception.h"
#include "messagehandler.h"

#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <utility>

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
	MessageHandler::sendCode(conn, static_cast<size_t>(Protocol::COM_LIST_NG));
	MessageHandler::sendCode(conn, static_cast<size_t>(Protocol::COM_END));

	vector<pair<int, string>> ngs;
	if(MessageHandler::recvCode(conn) == static_cast<int>(Protocol::ANS_LIST_NG)){
		int num = MessageHandler::recvIntParameter(conn);
		for(int i = 0; i < num; i++){
			int id = MessageHandler::recvIntParameter(conn);
			string name = MessageHandler::recvStringParameter(conn);
			//ngs[i].first = id;
			//ngs[i].second = name;
			ngs.push_back(make_pair(id, name));
		}
	} else {
		cout << "Protocol Violation" << endl;
	}

	/*
	 * Output newsgroups to client
	 */
	if(MessageHandler::recvCode(conn) != static_cast<int>(Protocol::ANS_END)){
		cout << "Protocol Violation" << endl;
	} else {
		for(pair<int, string>& ng : ngs){
			cout << ng.first << " " << ng.second << endl;
		}
	}
}

/*
 * List articles within specified newgroup
 */
void listArticles(const Connection& conn, int ngID){
	MessageHandler::sendCode(conn, static_cast<size_t>(Protocol::COM_LIST_ART));
	MessageHandler::sendIntParameter(conn, ngID);
	MessageHandler::sendCode(conn, static_cast<size_t>(Protocol::COM_END));

	vector<pair<int, string>> arts;
	if(MessageHandler::recvCode(conn) == static_cast<int>(Protocol::ANS_LIST_ART)){
		int ans = MessageHandler::recvCode(conn);
		if(ans == static_cast<int>(Protocol::ANS_ACK)){
			int num = MessageHandler::recvIntParameter(conn);
			for(int i = 0; i < num; i++){
				int id = MessageHandler::recvIntParameter(conn);
				string name = MessageHandler::recvStringParameter(conn);
				arts.push_back(make_pair(id, name));
			}
		} else if(ans == static_cast<int>(Protocol::ANS_NAK)){
			if(MessageHandler::recvCode(conn) == static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST)){
				cout << "Error newsgroup does not exist" << endl;
			}
		}
		
		if(MessageHandler::recvCode(conn) != static_cast<int>(Protocol::ANS_END)){
			cout << "Protocol Violation" << endl;
		} else {
			for(pair<int, string>& a : arts){
				cout << a.first << " " << a.second << endl;
			}
		}
	}
}

/*
 * Create newgroup with specified name
 */
void createNG(const Connection& conn, string name){
	MessageHandler::sendCode(conn, static_cast<size_t>(Protocol::COM_CREATE_NG));
	MessageHandler::sendStringParameter(conn, name);
	MessageHandler::sendCode(conn, static_cast<size_t>(Protocol::COM_END));

	if(MessageHandler::recvCode(conn) == static_cast<int>(Protocol::ANS_CREATE_NG)){
		int ans = MessageHandler::recvCode(conn);
		if(ans == static_cast<int>(Protocol::ANS_ACK)){
			if(MessageHandler::recvCode(conn) == static_cast<int>(Protocol::ANS_END)){
				cout << "Article added" << endl;
			} else {
				cout << "Protocol Violation" << endl;
			}
		} else if(ans == static_cast<int>(Protocol::ANS_NAK)){
			if(MessageHandler::recvCode(conn) == static_cast<int>(Protocol::ERR_NG_ALREADY_EXISTS)){
				cout << "Newgroup already exist" << endl;
				if(MessageHandler::recvCode(conn) != static_cast<int>(Protocol::ANS_END)){
					cout << "Protocol Violation" << endl;
				}
			} else {
				cout << "Protocol Violation" << endl;
			}
		}
	}
}

void deleteNG(const Connection& conn, int id){
	MessageHandler::sendCode(conn, static_cast<size_t>(Protocol::COM_DELETE_NG));
	MessageHandler::sendIntParameter(conn, id);
	MessageHandler::sendCode(conn, static_cast<size_t>(Protocol::COM_END));

	if(MessageHandler::recvCode(conn) == static_cast<int>(Protocol::ANS_DELETE_NG)){
		int ans = MessageHandler::recvCode(conn);
		if(ans == static_cast<int>(Protocol::ANS_ACK)){
			if(MessageHandler::recvCode(conn) == static_cast<int>(Protocol::ANS_END)){
				cout << "Newsgroup deleted" << endl;
			} else {
				cout << "Protocol Violation" << endl;
			}
		} else if (ans == static_cast<int>(Protocol::ANS_NAK)){
			if(MessageHandler::recvCode(conn) == static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST)){
				cout << "Newsgroup does not exist" << endl;
				if(MessageHandler::recvCode(conn) != static_cast<int>(Protocol::ANS_END)){
					cout << "Protocol Violation" << endl;
				}
			} else {
				cout << "Protocol Violation" << endl;
				if(MessageHandler::recvCode(conn) != static_cast<int>(Protocol::ANS_END)){
					cout << "Protocol Violation" << endl;
				}
			}
		}
	} else {
		cout << "Protocol Violation" << endl;
	}
}

void deleteArt(const Connection& conn, int ngID, int artID){
	MessageHandler::sendCode(conn, static_cast<size_t>(Protocol::COM_DELETE_ART));
	MessageHandler::sendIntParameter(conn, ngID);
	MessageHandler::sendIntParameter(conn, artID);
	MessageHandler::sendCode(conn, static_cast<size_t>(Protocol::COM_END));

	if(MessageHandler::recvCode(conn) == static_cast<int>(Protocol::ANS_DELETE_ART)){
		int ans = MessageHandler::recvCode(conn);
		if(ans == static_cast<int>(Protocol::ANS_ACK)){
			if(MessageHandler::recvCode(conn) == static_cast<int>(Protocol::ANS_END)){
				cout << "Article deleted" << endl;
			}
		} else if(ans == static_cast<int>(Protocol::ANS_NAK)){
			int err = MessageHandler::recvCode(conn);
			if(err == static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST)){
				cout << "Newsgroup does not exist" << endl;
				if(MessageHandler::recvCode(conn) != static_cast<int>(Protocol::ANS_END)){
					cout << "Protocol Violation" << endl;
				}
			} else if(err == static_cast<int>(Protocol::ERR_ART_DOES_NOT_EXIST)){
				cout << "Article does not exist" << endl;
				if(MessageHandler::recvCode(conn) != static_cast<int>(Protocol::ANS_END)){
					cout << "Protocol Violation" << endl;
				}
			}
		}
	} else {
		cout << "Protocol Violation" << endl;
	}
}

void readArt(const Connection& conn, int ngID, int artID){
	MessageHandler::sendCode(conn, static_cast<size_t>(Protocol::COM_GET_ART));
	MessageHandler::sendIntParameter(conn, ngID);
	MessageHandler::sendIntParameter(conn, artID);
	MessageHandler::sendCode(conn, static_cast<size_t>(Protocol::COM_END));

	string author = "";
	string title = "";
	string text = "";

	if(MessageHandler::recvCode(conn) == static_cast<int>(Protocol::ANS_GET_ART)){
		int ans = MessageHandler::recvCode(conn);
		if(ans == static_cast<int>(Protocol::ANS_ACK)){
			string author = MessageHandler::recvStringParameter(conn);
			string title = MessageHandler::recvStringParameter(conn);
			string text = MessageHandler::recvStringParameter(conn);
			if(MessageHandler::recvCode(conn) == static_cast<int>(Protocol::ANS_END)){
				cout << "Author: " << author << "\n" << "Title: " << title << "\n" << text << endl;
			}
		} else if(ans == static_cast<int>(Protocol::ANS_NAK)){
			int nak = MessageHandler::recvCode(conn);
			if(nak == static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST)){
				cout << "Newsgroup does not exist" << endl;
			} else if(nak == static_cast<int>(Protocol::ERR_ART_DOES_NOT_EXIST)){
				cout << "Article does not exist" << endl;
			}
			if(MessageHandler::recvCode(conn) != static_cast<int>(Protocol::ANS_END)){
				cout << "Protocol Violation" << endl;
			}
		}
	}
}

void createArt(const Connection& conn, int ngID, string title, string author, string text){
	MessageHandler::sendCode(conn, static_cast<size_t>(Protocol::COM_GET_ART));
	MessageHandler::sendIntParameter(conn, ngID);
	MessageHandler::sendStringParameter(conn, title);
	MessageHandler::sendStringParameter(conn, author);
	MessageHandler::sendStringParameter(conn, text);
	MessageHandler::sendCode(conn, static_cast<size_t>(Protocol::COM_END));

	if(MessageHandler::recvCode(conn) == static_cast<int>(Protocol::ANS_CREATE_ART)){
		int ans = MessageHandler::recvCode(conn);
		if(ans == static_cast<int>(Protocol::ANS_ACK)){
			cout << "Article created" << endl;
		} else if (ans == static_cast<int>(Protocol::ANS_NAK)){
			if(MessageHandler::recvCode(conn) == static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST)){
				cout << "Error, newgroup does not exist" << endl;
			} else {
				cout << "Protocol Violation" << endl;
			}
		} else {
			cout << "Protocol Violation" << endl;
		}
		if(MessageHandler::recvCode(conn) != static_cast<int>(Protocol::ANS_END)){
			cout << "Protocol Violation" << endl;
		}
	} else {
		cout << "Protocol Violation" << endl;
	}
}

int readNgId(string ngID){
	int id = 0;
	try{
		id = stoi(ngID);
	} catch (exception& e){
		cerr << "No valid id input for newsgroup." << endl;
		cin.clear();
	}
	return id;
}

int readArtId(string artID){
	int id = 0;
	try{
		id = stoi(artID);
	} catch (exception& e){
		cerr << "No valid id input for article." << endl;
		cin.clear();
	}
	return id;
}

void listCommands(){
	cout << "Commands:" << endl;
	cout << "List newgroups = list all newsgroups." << endl;
	cout << "Create newsgroup <newsgroup_name> = create new newsgroup with specified name." << endl;
	cout << "Delete newsgroup <newsgroup_id> = delete newsgroup with specified name." << endl;
	cout << "List articles <newsgroup_id> = list all articles in specified newsgroup id." << endl;
	cout << "Read <newsgroup_id> <article_id> = read an article in a newsgroup." << endl;
	cout << "Write = write article in a newsgroup." << endl;
	cout << "Delete article <newsgroup_id> <article_id> = delete article in a newsgroup." << endl;
	cout << "Quit = stopping the application." << endl;
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

	cout << "Welcome! Use any of the commands below to use this application." << endl;
	listCommands();
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
					int ngID = readNgId(cmd);
					if(ngID != 0){
						//List articles within this newsgroup
						listArticles(conn, ngID);
					}
				}
			} else if (cmd == "Create"){
				cin >> cmd;
				if(cmd == "newsgroup"){
					cin >> cmd;
					createNG(conn, cmd);
				}
			} else if(cmd == "Delete"){
				cin >> cmd;
				if(cmd == "newsgroup"){
					cin >> cmd;
					int ngID = readNgId(cmd);
					if(ngID != 0){
						deleteNG(conn, ngID);
					}
				} else if (cmd == "article"){
					cin >> cmd;
					int ngID = readNgId(cmd);
					if(ngID != 0){
						cin >> cmd;
						int artID = readArtId(cmd);
						if(artID != 0){
							deleteArt(conn, ngID, artID);
						}
					}
				}
			} else if (cmd == "Read"){
				cin >> cmd;
				int ngID = readNgId(cmd);
				if(ngID != 0){
					cin >> cmd;
					int artID = readArtId(cmd);
					if(artID != 0){
						readArt(conn, ngID, artID);
					}
				}
			} else if(cmd == "Write"){
				cout << "Enter the newgroup ID for the newsgroup where you want to place the article" << endl;
				cin >> cmd;
				int ngID = readNgId(cmd);
				if(ngID != 0){
					cout << "Enter the title" << endl;
					cin.ignore(numeric_limits<streamsize>::max(), '\n');
					string title;
					getline(cin, title);
					cout << "Enter the author" << endl;
					string author;
					getline(cin, author);
					cout << "Enter text" << endl;
					string text;
					getline(cin, text);
					createArt(conn, ngID, title, author, text);
				}
			} else if(cmd == "Help"){
				listCommands();
			} else if(cmd == "Quit"){
				break;
				exit(1);
			} else {
				cout << "Invalid command, write Help to list commands." << endl;
			}
		} catch (ConnectionClosedException&) {
			cout << " no reply from server. Exiting." << endl;
			exit(1);
		}
	}
}
