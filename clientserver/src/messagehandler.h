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

	static void sendCode(const Connection& conn, const size_t code);
	static void sendInt(const Connection& conn, int value);
	static void sendIntParameter(const Connection& conn, int param);
	static void sendStringParameter(const Connection& conn, string param);
	static int recvCode(const Connection& conn);
	static int recvInt(const Connection& conn);
	static int recvIntParameter(const Connection& conn);
	static string recvStringParameter(const Connection& conn);		

private:
	//Connection conn; // the connection
	//Logger logWindow; // the log window

	static void sendByte(const Connection& conn, int code);
	static int recvByte(const Connection& conn);

	
};
#endif