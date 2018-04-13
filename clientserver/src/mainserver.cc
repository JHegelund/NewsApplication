#include "protocol.h"
#include "server.h"
#include "connection.h"
#include "connectionclosedexception.h"
#include "databaseHNS.h"
#include <iostream>

using namespace std;


int readNumber(const shared_ptr<Connection>& conn) {
	unsigned char byte1 = conn->read();
	unsigned char byte2 = conn->read();
	unsigned char byte3 = conn->read();
	unsigned char byte4 = conn->read();
	return (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
}

string readString(const shared_ptr<Connection>& conn) {
	if (static_cast<Protocol>(conn->read()) != Protocol::PAR_STRING) {
		return NULL;
	}
	unsigned int length = readNumber(conn);
	string word(length, ' '); // Init empty string to N length
	for (unsigned int i = 0; i < length; ++i) {
		word[i] = conn->read();
	}
	return word;
}

int readNumP(const shared_ptr<Connection>& conn) {
	if (static_cast<Protocol>(conn->read()) != Protocol::PAR_NUM) {
		// throw exception
	}
	return readNumber(conn);
}

// Sending a raw number N, i.e. without the PAR_NUM
void sendNumber(const shared_ptr<Connection>& conn, int number) {
	unsigned char byte1 = number >> 24;
	unsigned char byte2 = number >> 16;
	unsigned char byte3 = number >> 8;
	unsigned char byte4 = number;
	conn->write(byte1);
	conn->write(byte2);
	conn->write(byte3);
	conn->write(byte4);
}

void sendString(const shared_ptr<Connection>& conn, string s) {
	conn->write(static_cast<unsigned char>(Protocol::PAR_STRING));
	sendNumber(conn, s.length());
	for (auto& c : s) {
		conn->write(c);
	}
}

void sendNumP(const shared_ptr<Connection>& conn, int number) {
	conn->write(static_cast<unsigned char>(Protocol::PAR_NUM));
	sendNumber(conn, number);
}

void listNewsGroups(const shared_ptr<Connection>& conn, Database& db) {
	if (static_cast<Protocol>(conn->read()) != Protocol::COM_END)
		return; // FAIL
	conn->write(static_cast<unsigned char>(Protocol::ANS_LIST_NG)); // Send ACK
	vector<pair<int, string>> newsGroups = db.listNewsGroups();

	// Send num_p of how many Newsgroups there are
	sendNumP(conn, newsGroups.size());

	// Send [num_p string_p]* i.e. list of Newsgroups ID and name
	for (auto& ng : newsGroups) {
		sendNumP(conn, ng.first);
		sendString(conn, ng.second);
	}
	conn->write(static_cast<unsigned char>(Protocol::ANS_END)); // Send ANS_END
}

void createNewsGroup(const shared_ptr<Connection>& conn, Database& db) {
	string newsGroupName = readString(conn);
	if (static_cast<Protocol>(conn->read()) != Protocol::COM_END){
		cout << "Fail" << endl;
		return; // FAIL
	}
	conn->write(static_cast<unsigned char>(Protocol::ANS_CREATE_NG));
	if(db.createNewsGroup(newsGroupName)) {
		// Success
		conn->write(static_cast<unsigned char>(Protocol::ANS_ACK));
	} else {
		// Fail
		conn->write(static_cast<unsigned char>(Protocol::ANS_NAK));
		conn->write(static_cast<unsigned char>(Protocol::ERR_NG_ALREADY_EXISTS));
	}
	cout << "Finished" << endl;
	conn->write(static_cast<unsigned char>(Protocol::ANS_END));
}

void deleteNewsGroup(const shared_ptr<Connection>& conn, Database& db) {
	int newsGroupIndex = readNumP(conn);
	if (static_cast<Protocol>(conn->read()) != Protocol::COM_END)
		return; // FAIL

	conn->write(static_cast<unsigned char>(Protocol::ANS_DELETE_NG)); // Send ACK
	if(db.deleteNewsGroup(newsGroupIndex)) {
		conn->write(static_cast<unsigned char>(Protocol::ANS_ACK));
	} else {
		conn->write(static_cast<unsigned char>(Protocol::ANS_NAK));
		conn->write(static_cast<unsigned char>(Protocol::ERR_NG_DOES_NOT_EXIST));
	}
	conn->write(static_cast<unsigned char>(Protocol::ANS_END));
}

void listArticles(const shared_ptr<Connection>& conn, Database& db) {
	int newsGroupIndex = readNumber(conn);
	if (static_cast<Protocol>(conn->read()) != Protocol::COM_END)
		return; // FAIL

	vector<pair<int, string>> articles = db.listArticles(newsGroupIndex);
	if (articles.size()) { // Have to throw exception
		conn->write(static_cast<unsigned char>(Protocol::ANS_NAK));
		conn->write(static_cast<unsigned char>(Protocol::ERR_NG_DOES_NOT_EXIST));
	} else {
		conn->write(static_cast<unsigned char>(Protocol::ANS_ACK));
		sendNumP(conn, articles.size());
		for (auto& art : articles) {
			sendNumP(conn, art.first);
			sendString(conn, art.second);
		}
	}
	conn->write(static_cast<unsigned char>(Protocol::ANS_END));
}

void createArticle(const shared_ptr<Connection>& conn, Database& db) {
	int newsGroupIndex = readNumP(conn);
	string title = readString(conn);
	string author = readString(conn);
	string text = readString(conn);
	if (static_cast<Protocol>(conn->read()) != Protocol::COM_END)
		return; // FAIL

	conn->write(static_cast<unsigned char>(Protocol::ANS_CREATE_ART));
	if(db.createArticle(newsGroupIndex, title, author, text)) {
		conn->write(static_cast<unsigned char>(Protocol::ANS_ACK));
	} else {
		conn->write(static_cast<unsigned char>(Protocol::ANS_NAK));
		conn->write(static_cast<unsigned char>(Protocol::ERR_NG_DOES_NOT_EXIST));
	}
	conn->write(static_cast<unsigned char>(Protocol::ANS_END));
}

void deleteArticle(const shared_ptr<Connection>& conn, Database& db) {
	int newsGroupIndex = readNumP(conn);
	int articleIndex = readNumP(conn);
	if (static_cast<Protocol>(conn->read()) != Protocol::COM_END)
		return; // FAIL

	conn->write(static_cast<unsigned char>(Protocol::ANS_DELETE_ART));
	if(db.deleteArticle(newsGroupIndex, articleIndex)) {
		conn->write(static_cast<unsigned char>(Protocol::ANS_ACK));
	} else {
		conn->write(static_cast<unsigned char>(Protocol::ANS_NAK));
		// Need exceptions to know if it's the article or NG that does not exist.
	}
	conn->write(static_cast<unsigned char>(Protocol::ANS_END));
}

void getArticle(const shared_ptr<Connection>& conn, Database& db) {
	int newsGroupIndex = readNumP(conn);
	int articleIndex = readNumP(conn);
	if (static_cast<Protocol>(conn->read()) != Protocol::COM_END)
		return; // FAIL

	conn->write(static_cast<unsigned char>(Protocol::ANS_GET_ART));
	// Fetch, handle and send article
	db.getArticle(newsGroupIndex, articleIndex);
}


int main(int argc, char* argv[]){
	if (argc != 2) {
		cerr << "Usage: myserver port-number" << endl;
		exit(1);
	}

	int port = -1;
	try {
		port = stoi(argv[1]);
	} catch (exception& e) {
		cerr << "Wrong port number. " << e.what() << endl;
		exit(1);
	}

	Server server(port);
	if (!server.isReady()) {
		cerr << "Server initialization error." << endl;
		exit(1);
	}
	DatabaseHNS db = DatabaseHNS();

	while (true) {
		auto conn = server.waitForActivity();
		Protocol p = Protocol::UNDEFINED;
		if (conn != nullptr) {
			try {
				// Read commandbyte
				p = static_cast<Protocol>(conn->read());
				switch(p) {
					case Protocol::COM_LIST_NG:
						listNewsGroups(conn, db); break;
					case Protocol::COM_CREATE_NG:
						createNewsGroup(conn, db); break;
					case Protocol::COM_DELETE_NG:
						deleteNewsGroup(conn, db); break;
					case Protocol::COM_LIST_ART:
						listArticles(conn, db); break;
					case Protocol::COM_CREATE_ART:
						createArticle(conn, db); break;
					case Protocol::COM_DELETE_ART:
						deleteArticle(conn, db); break;
					case Protocol::COM_GET_ART:
						getArticle(conn, db); break;
					default: break; // Throw protocolexception
				}
			} catch (ConnectionClosedException&) {
				server.deregisterConnection(conn);
				cout << "Client closed connection" << endl;
			}
		} else {
			conn = make_shared<Connection>();
			server.registerConnection(conn);
			cout << "New client connects" << endl;
		}
	}
}
