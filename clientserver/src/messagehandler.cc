/**
 * Message handler is the low-level message handler for messages to/from the
 * server. It handles transmission/reception of low-level message units.
 */

#include "messagehandler.h"
#include "protocol.h"
#include "connection.h"
#include "connectionclosedexception.h"

#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>

using namespace std;
	
/**
 * Create a message handler.
 * @param conn The connection to use messages
*/
MessageHandler::MessageHandler(const shared_ptr<Connection>& conn): conn(conn) {}

/**
 * Set the log window to use.
 * @param logWindow The log window
 */
/*public void setLogWindow(Logger logWindow) {
	this.logWindow = logWindow;
}*/

/**
 * Transmit a code (a constant from the Protocol class).
 * @param code, The code to transmit.
 * @throws ConnectionClosedException, If the server died
 */
void MessageHandler::sendCode(const Protocol code){
	conn->write(static_cast<unsigned char>(code)); //Static cast?
}

/**
 * Transmit an int value, according to the protocol.
 * @param value, The value to transmit
 * @throws ConnectionClosedException, If the server died
 */
void MessageHandler::sendInt(int value){
	conn->write((value >> 24) & 0xFF);
	conn->write((value >> 16) & 0xFF);
	conn->write((value >> 8) & 0xFF);
	conn->write(value & 0xFF);
}

/**
 * Transmit an int parameter, according to the protocol.
 * @param value, The parameter to transmit
 * @throws ConnectionClosedException, If the server died
 */
void MessageHandler::sendIntParameter(int param){
	sendCode(Protocol::PAR_NUM);
	sendInt(param);
}

/**
 * Transmit a string parameter, according to the protocol.
 * @param value, The parameter to transmit
 * @throws ConnectionClosedException, If the server died
 */
void MessageHandler::sendStringParameter(string param){
	sendCode(Protocol::PAR_STRING);
	sendInt(param.length());
	for (auto& c : param) {
		conn->write(c);
	}
}



/**
 * Receive a command code or an error code from the server.
 * @return The code
 * @throws ConnectionClosedException, If the server died
 */
Protocol MessageHandler::recvCode() const{
	return static_cast<Protocol>(conn->read());
}

/**
 * Receive an int value from the server.
 * @return The int value
 * @throws ConnectionClosedException, If the server died
 */
int MessageHandler::recvInt() const{
	unsigned char byte1 = conn->read();
	unsigned char byte2 = conn->read();
	unsigned char byte3 = conn->read();
	unsigned char byte4 = conn->read();
	return (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
}


/**
 * Receive an int parameter from the server.
 * @return The parameter value
 * @throws ConnectionClosedException, If the server died
 */
int MessageHandler::recvIntParameter() const{
	if (recvCode() != Protocol::PAR_NUM) {
		// throw exception
	}
	return recvInt();
}

/**
 * Receive a string parameter from the server.
 * @return The parameter value
 * @throws ConnectionClosedException, If the server died
 */
string MessageHandler::recvStringParameter() const{
	if (recvCode() != Protocol::PAR_STRING) {
		return NULL;
	}
	unsigned int length = recvInt();
	string word(length, ' '); // Init empty string to N length
	for (unsigned int i = 0; i < length; ++i) {
		word[i] = conn->read();
	}
	return word;
}
/*
void MessageHandler::sendByte(const Connection& conn, int code){
	try {
		conn.write(code);
	} catch (ConnectionClosedException&) {
		cout << " no reply from server. Exiting." << endl;
		exit(1);
	}
}

int MessageHandler::recvByte(const Connection& conn){
	try{
		int code = conn.read();
	} catch(ConnectionClosedException&){
		cout << " no reply from server. Exiting." << endl;
		exit(1);
	}
	return code;
}
*/