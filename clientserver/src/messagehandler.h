/**
 * Message handler is the low-level message handler for messages to/from the
 * server. It handles transmission/reception of low-level message units.
 */

#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <string>
#include <memory>
#include "connection.h"
#include "protocol.h"

 using namespace std;

class MessageHandler {
public:
	MessageHandler(const Connection* conn);
	//void setLogWindow(Logger logWindow);
	void sendCode(const Protocol code);
	void sendInt(int value);
	void sendIntParameter(int param);
	void sendStringParameter(string param);
	Protocol recvCode() const;
	int recvInt() const;
	int recvIntParameter() const;
	string recvStringParameter() const;		
private:
	const Connection* conn; // the connection
	//Logger logWindow; // the log window
	//void sendByte(const Connection& conn, int code);
	//int recvByte(const Connection& conn);

	
};
#endif