#include "protocol.h"
#include "connection.h"
#include "connectionclosedexception.h"
#include "messagehandler.h"

#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <utility>
#include <limits>

using namespace std;


/*
 * Writes and receives list newsgroups command to and from server
 */
void listNG(MessageHandler& mh){
	mh.sendCode(Protocol::COM_LIST_NG);
	mh.sendCode(Protocol::COM_END);

	vector<pair<int, string>> ngs;
	if(mh.recvCode() == Protocol::ANS_LIST_NG){
		int num = mh.recvIntParameter();
		for(int i = 0; i < num; i++){
			int id = mh.recvIntParameter();
			string name = mh.recvStringParameter();
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
	if(mh.recvCode() != Protocol::ANS_END){
		cout << "Protocol Violation" << endl;
	} else {
		for(pair<int, string>& ng : ngs){
			cout << "ID: " << ng.first << " Newsgroup: " << ng.second << endl;
		}
	}
}

/*
 * List articles within specified newgroup
 */
void listArticles(MessageHandler& mh, int ngID){
	mh.sendCode(Protocol::COM_LIST_ART);
	mh.sendIntParameter(ngID);
	mh.sendCode(Protocol::COM_END);

	vector<pair<int, string>> arts;
	if(mh.recvCode() == Protocol::ANS_LIST_ART){
		Protocol ans = mh.recvCode();
		if(ans == Protocol::ANS_ACK){
			int num = mh.recvIntParameter();
			for(int i = 0; i < num; i++){
				int id = mh.recvIntParameter();
				string name = mh.recvStringParameter();
				arts.push_back(make_pair(id, name));
			}
		} else if(ans == Protocol::ANS_NAK){
			if(mh.recvCode() == Protocol::ERR_NG_DOES_NOT_EXIST){
				cout << "Error newsgroup does not exist" << endl;
			}
		}

		if(mh.recvCode() != Protocol::ANS_END){
			cout << "Protocol Violation" << endl;
		} else {
			for(pair<int, string>& a : arts){
				cout << "ID: " << a.first << " Article: " << a.second << endl;
			}
		}
	}
}

/*
 * Create newgroup with specified name
 */
void createNG(MessageHandler& mh, string name){
	mh.sendCode(Protocol::COM_CREATE_NG);
	mh.sendStringParameter(name);
	mh.sendCode(Protocol::COM_END);

	if(mh.recvCode() == Protocol::ANS_CREATE_NG){
		Protocol ans = mh.recvCode();
		if(ans == Protocol::ANS_ACK){
			if(mh.recvCode() == Protocol::ANS_END){
				cout << "Newsgroup added" << endl;
			} else {
				cout << "Protocol Violation" << endl;
			}
		} else if(ans == Protocol::ANS_NAK){
			if(mh.recvCode() == Protocol::ERR_NG_ALREADY_EXISTS){
				cout << "Newgroup already exist" << endl;
				if(mh.recvCode() != Protocol::ANS_END){
					cout << "Protocol Violation" << endl;
				}
			} else {
				cout << "Protocol Violation" << endl;
			}
		}
	}
}

void deleteNG(MessageHandler& mh, int id){
	mh.sendCode(Protocol::COM_DELETE_NG);
	mh.sendIntParameter(id);
	mh.sendCode(Protocol::COM_END);

	if(mh.recvCode() == Protocol::ANS_DELETE_NG){
		Protocol ans = mh.recvCode();
		if(ans == Protocol::ANS_ACK){
			if(mh.recvCode() == Protocol::ANS_END){
				cout << "Newsgroup deleted" << endl;
			} else {
				cout << "Protocol Violation" << endl;
			}
		} else if (ans == Protocol::ANS_NAK){
			if(mh.recvCode() == Protocol::ERR_NG_DOES_NOT_EXIST){
				cout << "Newsgroup does not exist" << endl;
				if(mh.recvCode() != Protocol::ANS_END){
					cout << "Protocol Violation" << endl;
				}
			} else {
				cout << "Protocol Violation" << endl;
				if(mh.recvCode() != Protocol::ANS_END){
					cout << "Protocol Violation" << endl;
				}
			}
		}
	} else {
		cout << "Protocol Violation" << endl;
	}
}

void deleteArt(MessageHandler& mh, int ngID, int artID){
	mh.sendCode(Protocol::COM_DELETE_ART);
	mh.sendIntParameter(ngID);
	mh.sendIntParameter(artID);
	mh.sendCode(Protocol::COM_END);

	if(mh.recvCode() == Protocol::ANS_DELETE_ART){
		Protocol ans = mh.recvCode();
		if(ans == Protocol::ANS_ACK){
			if(mh.recvCode() == Protocol::ANS_END){
				cout << "Article deleted" << endl;
			}
		} else if(ans == Protocol::ANS_NAK){
			Protocol err = mh.recvCode();
			if(err == Protocol::ERR_NG_DOES_NOT_EXIST){
				cout << "Newsgroup does not exist" << endl;
				if(mh.recvCode() != Protocol::ANS_END){
					cout << "Protocol Violation" << endl;
				}
			} else if(err == Protocol::ERR_ART_DOES_NOT_EXIST){
				cout << "Article does not exist" << endl;
				if(mh.recvCode() != Protocol::ANS_END){
					cout << "Protocol Violation" << endl;
				}
			}
		}
	} else {
		cout << "Protocol Violation" << endl;
	}
}

void readArt(MessageHandler& mh, int ngID, int artID){
	mh.sendCode(Protocol::COM_GET_ART);
	mh.sendIntParameter(ngID);
	mh.sendIntParameter(artID);
	mh.sendCode(Protocol::COM_END);

	string author = "";
	string title = "";
	string text = "";

	if(mh.recvCode() == Protocol::ANS_GET_ART){
		Protocol ans = mh.recvCode();
		if(ans == Protocol::ANS_ACK){
			string author = mh.recvStringParameter();
			string title = mh.recvStringParameter();
			string text = mh.recvStringParameter();
			if(mh.recvCode() == Protocol::ANS_END){
				cout << "Author: " << author << "\n" << "Title: " << title << "\n" << text << endl;
			}
		} else if(ans == Protocol::ANS_NAK){
			Protocol nak = mh.recvCode();
			if(nak == Protocol::ERR_NG_DOES_NOT_EXIST){
				cout << "Newsgroup does not exist" << endl;
			} else if(nak == Protocol::ERR_ART_DOES_NOT_EXIST){
				cout << "Article does not exist" << endl;
			}
			if(mh.recvCode() != Protocol::ANS_END){
				cout << "Protocol Violation" << endl;
			}
		}
	}
}

void createArt(MessageHandler& mh, int ngID, string title, string author, string text){
	mh.sendCode(Protocol::COM_CREATE_ART);
	mh.sendIntParameter(ngID);
	mh.sendStringParameter(title);
	mh.sendStringParameter(author);
	mh.sendStringParameter(text);
	mh.sendCode(Protocol::COM_END);

	if(mh.recvCode() == Protocol::ANS_CREATE_ART){
		Protocol ans = mh.recvCode();
		if(ans == Protocol::ANS_ACK){
			cout << "Article created" << endl;
		} else if (ans == Protocol::ANS_NAK){
			if(mh.recvCode() == Protocol::ERR_NG_DOES_NOT_EXIST){
				cout << "Error, newgroup does not exist" << endl;
			} else {
				cout << "Protocol Violation" << endl;
			}
		} else {
			cout << "Protocol Violation" << endl;
		}
		if(mh.recvCode() != Protocol::ANS_END){
			cout << "Protocol Violation" << endl;
		}
	} else {
		cout << "Protocol Violation" << endl;
	}
}

int readNgId(string ngID){
	int id = -1;
	try{
		id = stoi(ngID);
	} catch (exception& e){
		cerr << "No valid id input for newsgroup." << endl;
		cin.clear();
	}
	return id;
}

int readArtId(string artID){
	int id = -1;
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
	MessageHandler mh(&conn);
	string cmd;
	while (cin >> cmd) {
		try {
			if(cmd == "List"){
				cin >> cmd;
				if(cmd == "newsgroups"){
					listNG(mh);
				} else if(cmd == "articles"){
					//Read newsgroup id
					cin >> cmd;
					int ngID = readNgId(cmd);
					if(ngID != -1){
						//List articles within this newsgroup
						listArticles(mh, ngID);
					}
				}
			} else if (cmd == "Create"){
				cin >> cmd;
				if(cmd == "newsgroup"){
					string ngname;
					getline(cin, ngname);
					createNG(mh, ngname);
				}
			} else if(cmd == "Delete"){
				cin >> cmd;
				if(cmd == "newsgroup"){
					cin >> cmd;
					int ngID = readNgId(cmd);
					if(ngID != -1){
						deleteNG(mh, ngID);
					}
				} else if (cmd == "article"){
					cin >> cmd;
					int ngID = readNgId(cmd);
					if(ngID != -1){
						cin >> cmd;
						int artID = readArtId(cmd);
						if(artID != -1){
							deleteArt(mh, ngID, artID);
						}
					}
				}
			} else if (cmd == "Read"){
				cin >> cmd;
				int ngID = readNgId(cmd);
				if(ngID != -1){
					cin >> cmd;
					int artID = readArtId(cmd);
					if(artID != -1){
						readArt(mh, ngID, artID);
					}
				}
			} else if(cmd == "Write"){
				cout << "Enter the newgroup ID for the newsgroup where you want to place the article" << endl;
				cin >> cmd;
				cin.ignore();
				int ngID = readNgId(cmd);
				if(ngID != -1){
					cout << "Enter the title" << endl;
					//cin.ignore(numeric_limits<streamsize>::max(), '\n');
					string title;
					getline(cin, title);
					cout << "Enter the author" << endl;
					string author;
					getline(cin, author);
					cout << "Enter text" << endl;
					string text;
					getline(cin, text);
					createArt(mh, ngID, title, author, text);
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
