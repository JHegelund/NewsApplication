/**
 * Message handler is the low-level message handler for messages to/from the
 * server. It handles transmission/reception of low-level message units.
 */

#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <string>
#include "connection.h"

 using namespace std;

class MessageHandler {
public:

	MessageHandler(const Connection& conn);

	/**
	 * Set the log window to use.
	 * 
	 * @param logWindow
	 *            The log window
	 */
	//void setLogWindow(Logger logWindow);

	void sendCode(const Connection& conn, int code);
	void sendInt(const Connection& conn, int value);
	void sendIntParameter(const Connection& conn, int param);
	void sendStringParameter(const Connection& conn, String param);
	int recvCode();
	int recvInt();
	int recvIntParameter();
	String recvStringParameter();		

private:
	//Connection conn; // the connection
	//Logger logWindow; // the log window

	void sendByte(const Connection& conn, int code);
	int recvByte(const Connection& conn);

	
}