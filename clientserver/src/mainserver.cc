#include "protocol.h"
#include "server.h"
#include "connection.h"
#include "connectionclosedexception.h"

using namespace std;

int readNumber(const shared_ptr<Connection>& conn) {
	unsigned char byte1 = conn->read();
	unsigned char byte2 = conn->read();
	unsigned char byte3 = conn->read();
	unsigned char byte4 = conn->read();
	return (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
}

string readString(const shared_ptr<Connection>& conn) {
	unsigned int length = readNumber(conn);
	string word(length, ' '); // Init empty string to N length
	for (unsigned int i = 0; i < length; ++i) {
		word[i] = conn->read();
	}
	return word;
}

void listNewsgroups(const shared_ptr<Connection>& conn) {
	// Fetch needed data from Database
	conn->write(ANS_LIST_NG); // Send ACK
	// Send num_p of how many Newsgroups there are
	// Send [num_p string_p]* i.e. list of Newsgroups ID and name
	conn->write(Protocol.ANS_END); // Send ANS_END
}

void createNewsgroups(const shared_ptr<Connection>& conn) {
	// Format: COM_CREATE_NG string_p COM_END
	conn->write(Protocol.ANS_CREATE_NG); // Send ACK
	name = readString(conn); // Read the string_p

	// Create the Newsgroup in Database
	if(database.createNewsgroup(name)) {
		// Success
		conn->write(Protocol.ANS_ACK);
	} else {
		// Fail
		conn->write(Protocol.ANS_NAK);
		conn->write(Protocol.ERR_NG_ALREADY_EXISTS);
	}
	conn->write(Protocol.ANS_END); // Send ANS_END
}

void deleteNewsgroup(const shared_ptr<Connection>& conn) {
	conn->write(Protocol.ANS_DELETE_NG); // Send ACK
}

void listArticles(const shared_ptr<Connection>& conn) {}

void createArticle(const shared_ptr<Connection>& conn) {}

void deleteArticle(const shared_ptr<Connection>& conn) {}

void getArticle(const shared_ptr<Connection>& conn) {}


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

	while (true) {
		auto conn = server.waitForActivity();
		if (conn != nullptr) {
			try {
				// Read commandbyte
				switch(conn->read()) {
					case Protocol.COM_LIST_NG:
						listNewsgroups(conn); break;
					case Protocol.COM_CREATE_NG:
						createNewsgroups(conn); break;
					case Protocol.COM_DELETE_NG:
						deleteNewsgroup(conn); break;
					case Protocol.COM_LIST_ART:
						listArticles(conn); break;
					case Protocol.COM_CREATE_ART:
						createArticle(conn); break;
					case Protocol.COM_DELETE_ART:
						deleteArticle(conn); break;
					case Protocol.COM_GET_ART:
						getArticle(conn); break;
					default: // Throw protocolexception
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
}
