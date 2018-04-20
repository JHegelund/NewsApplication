#include "protocol.h"
#include "messagehandler.h"
#include "server.h"
#include "connection.h"
#include "connectionclosedexception.h"
#include "databaseHNS.h"
#include <iostream>

using namespace std;


void listNewsGroups(MessageHandler& mh, Database& db) {
	if (mh.recvCode() != Protocol::COM_END)
		return; // FAIL
	mh.sendCode(Protocol::ANS_LIST_NG); // Send ACK
	vector<pair<int, string>> newsGroups = db.listNewsGroups();

	// Send num_p of how many Newsgroups there are
	mh.sendIntParameter(newsGroups.size());

	// Send [num_p string_p]* i.e. list of Newsgroups ID and name
	for (auto& ng : newsGroups) {
		mh.sendIntParameter(ng.first);
		mh.sendStringParameter(ng.second);
	}
	mh.sendCode(Protocol::ANS_END); // Send ANS_END
}

void createNewsGroup(MessageHandler& mh, Database& db) {
	string newsGroupName = mh.recvStringParameter();
	if (mh.recvCode() != Protocol::COM_END){
		cout << "Fail" << endl;
		return; // FAIL
	}
	mh.sendCode(Protocol::ANS_CREATE_NG);
	if(db.createNewsGroup(newsGroupName)) {
		// Success
		mh.sendCode(Protocol::ANS_ACK);
	} else {
		// Fail
		mh.sendCode(Protocol::ANS_NAK);
		mh.sendCode(Protocol::ERR_NG_ALREADY_EXISTS);
	}
	mh.sendCode(Protocol::ANS_END);
}

void deleteNewsGroup(MessageHandler& mh, Database& db) {
	int newsGroupIndex = mh.recvIntParameter();
	if (mh.recvCode() != Protocol::COM_END)
		return; // FAIL

	mh.sendCode(Protocol::ANS_DELETE_NG); // Send ACK
	if(db.deleteNewsGroup(newsGroupIndex)) {
		mh.sendCode(Protocol::ANS_ACK);
	} else {
		mh.sendCode(Protocol::ANS_NAK);
		mh.sendCode(Protocol::ERR_NG_DOES_NOT_EXIST);
	}
	mh.sendCode(Protocol::ANS_END);
}

void listArticles(MessageHandler& mh, Database& db) {
	int newsGroupIndex = mh.recvIntParameter();
	if (mh.recvCode() != Protocol::COM_END)
		return; // FAIL
	
	mh.sendCode(Protocol::ANS_LIST_ART);
	try {
		vector<pair<int, string>> articles = db.listArticles(newsGroupIndex);
		
		mh.sendCode(Protocol::ANS_ACK);
		mh.sendIntParameter(articles.size());
		for (auto& art : articles) {
			mh.sendIntParameter(art.first);
			mh.sendStringParameter(art.second);
		}
	} catch (const char* errorMessage) {
		mh.sendCode(Protocol::ANS_NAK);
		mh.sendCode(Protocol::ERR_NG_DOES_NOT_EXIST);
		std::cerr << errorMessage << std::endl;
	}
	mh.sendCode(Protocol::ANS_END);
}

void createArticle(MessageHandler& mh, Database& db) {
	int newsGroupIndex = mh.recvIntParameter();
	string title = mh.recvStringParameter();
	string author = mh.recvStringParameter();
	string text = mh.recvStringParameter();
	if (mh.recvCode() != Protocol::COM_END)
		return; // FAIL

	mh.sendCode(Protocol::ANS_CREATE_ART);
	if(db.createArticle(newsGroupIndex, title, author, text)) {
		mh.sendCode(Protocol::ANS_ACK);
	} else {
		mh.sendCode(Protocol::ANS_NAK);
		mh.sendCode(Protocol::ERR_NG_DOES_NOT_EXIST);
	}
	mh.sendCode(Protocol::ANS_END);
}

void deleteArticle(MessageHandler& mh, Database& db) {
	int newsGroupIndex = mh.recvIntParameter();
	int articleIndex = mh.recvIntParameter();
	if (mh.recvCode() != Protocol::COM_END)
		return; // FAIL

	mh.sendCode(Protocol::ANS_DELETE_ART);
	if(db.deleteArticle(newsGroupIndex, articleIndex)) {
		mh.sendCode(Protocol::ANS_ACK);
	} else {
		mh.sendCode(Protocol::ANS_NAK);
	}
	mh.sendCode(Protocol::ANS_END);
}

void getArticle(MessageHandler& mh, Database& db) {
	int newsGroupIndex = mh.recvIntParameter();
	int articleIndex = mh.recvIntParameter();
	if (mh.recvCode() != Protocol::COM_END)
		return; // FAIL
 
	mh.sendCode(Protocol::ANS_GET_ART);
	if (db.newsGroupExists(newsGroupIndex)) {
		if (db.articleExists(newsGroupIndex, articleIndex)) {
			mh.sendCode(Protocol::ANS_ACK);
			Article art = db.getArticle(newsGroupIndex, articleIndex);
			mh.sendStringParameter(art.title);
			mh.sendStringParameter(art.author);
			mh.sendStringParameter(art.article);
		} else {
			mh.sendCode(Protocol::ANS_NAK);
			mh.sendCode(Protocol::ERR_NG_DOES_NOT_EXIST);
		}
	} else {
		mh.sendCode(Protocol::ANS_NAK);
		mh.sendCode(Protocol::ERR_NG_DOES_NOT_EXIST);
	}
	mh.sendCode(Protocol::ANS_END);

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
				MessageHandler mh(conn.get());
				p = mh.recvCode();
				switch(p) {
					case Protocol::COM_LIST_NG:
						listNewsGroups(mh, db); break;
					case Protocol::COM_CREATE_NG:
						createNewsGroup(mh, db); break;
					case Protocol::COM_DELETE_NG:
						deleteNewsGroup(mh, db); break;
					case Protocol::COM_LIST_ART:
						listArticles(mh, db); break;
					case Protocol::COM_CREATE_ART:
						createArticle(mh, db); break;
					case Protocol::COM_DELETE_ART:
						deleteArticle(mh, db); break;
					case Protocol::COM_GET_ART:
						getArticle(mh, db); break;
					default: 
						throw "Protcolexception!"; break;
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
