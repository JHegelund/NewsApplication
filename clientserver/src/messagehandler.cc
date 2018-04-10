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
 * 
 * @param conn
 *            The connection to use messages
*/
MessageHandler::MessageHandler(const Connection& conn) {
		
}

/**
 * Set the log window to use.
 * 
 * @param logWindow
 *            The log window
 */
/*public void setLogWindow(Logger logWindow) {
	this.logWindow = logWindow;
}*/

/**
 * Transmit a code (a constant from the Protocol class).
 * 
 * @param code
 *            The code to transmit.
 * @throws ConnectionClosedException
 *             If the server died
 */
void MessageHandler::sendCode(const Connection& conn, int code){
	sendByte(conn, code);
}

/**
 * Transmit an int value, according to the protocol.
 * 
 * @param value
 *            The value to transmit
 * @throws ConnectionClosedException
 *             If the server died
 */
void MessageHandler::sendInt(const Connection& conn, int value){
	sendByte(conn, (value >> 24) & 0xFF);
	//logWindow.logByte((value >> 24) & 0xFF);
	sendByte(conn, (value >> 16) & 0xFF);
	//logWindow.logByte((value >> 16) & 0xFF);
	sendByte(conn, (value >> 8) & 0xFF);
	//logWindow.logByte((value >> 8) & 0xFF);
	sendByte(conn, value & 0xFF);
	//logWindow.logByte(value & 0xFF);
}

/**
 * Transmit an int parameter, according to the protocol.
 * 
 * @param value
 *            The parameter to transmit
 * @throws ConnectionClosedException
 *             If the server died
 */
void MessageHandler::sendIntParameter(const Connection& conn, int param){
	sendCode(conn, Protocol::PAR_NUM);
	sendInt(conn, param);
}

/**
 * Transmit a string parameter, according to the protocol.
 * 
 * @param value
 *            The parameter to transmit
 * @throws ConnectionClosedException
 *             If the server died
 */
void MessageHandler::sendStringParameter(const Connection& conn, String param){
	sendCode(conn, Protocol::PAR_STRING);
	sendInt(conn, param.length());
	for (int i = 0; i < param.length(); i++) {
		sendByte(conn, param.charAt(i));
		//logWindow.logChar(param.charAt(i));
	}
}



/**
 * Receive a command code or an error code from the server.
 * 
 * @return The code
 * @throws ConnectionClosedException
 *             If the server died
 */
int MessageHandler::recvCode(){
	int code = recvByte();
	//logWindow.logCode(code);
	return code;
}

/**
 * Receive an int value from the server.
 * 
 * @return The int value
 * @throws ConnectionClosedException
 *             If the server died
 */
int MessageHandler::recvInt(){
	int b1 = recvByte();
	//logWindow.logByte(b1);
	int b2 = recvByte();
	//logWindow.logByte(b2);
	int b3 = recvByte();
	//logWindow.logByte(b3);
	int b4 = recvByte();
	//logWindow.logByte(b4);

	return b1 << 24 | b2 << 16 | b3 << 8 | b4;
}

/**
 * Receive an int parameter from the server.
 * 
 * @return The parameter value
 * @throws ConnectionClosedException
 *             If the server died
 */
int MessageHandler::recvIntParameter(){
	int code = recvCode();
	if (code != Protocol::PAR_NUM) {
		cout << "Protocol Violation, Receive numeric parameter" << endl;
	}
	return recvInt();
}

/**
 * Receive a string parameter from the server.
 * 
 * @return The parameter value
 * @throws ConnectionClosedException
 *             If the server died
 */
String MessageHandler::recvStringParameter(){
	int code = recvCode();
	if (code != Protocol::PAR_STRING) {
		cout << "Protocol Violation, Receive string parameter" << endl;
	}
	int n = recvInt();
	if (n < 0) {
		cout << "Receive string parameter, Number of characters < 0" << endl;
	}
	//Write stringbuffer for C++
	/*StringBuffer result = new StringBuffer(n);
	for (int i = 1; i <= n; i++) {
		char ch = (char) conn.read();
		result.append(ch);
		logWindow.logChar(ch);
	}*/
	return result.toString();
}

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