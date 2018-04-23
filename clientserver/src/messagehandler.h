/**
 * Message handler is the low-level message handler for messages to/from the
 * server. It handles transmission/reception of low-level message units.
 */

#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <string>
#include <fstream>
#include <memory>
#include "connection.h"
#include "protocol.h"

 using namespace std;

class MessageHandler {
public:
	MessageHandler(const Connection* conn);
	void sendCode(const Protocol code);
	void sendIntParameter(int param);
	void sendStringParameter(string param);
	Protocol recvCode() const;
	int recvIntParameter() const;
	string recvStringParameter() const;
private:
	void sendInt(int value);	
	int recvInt() const;
	const Connection* conn;
};
#endif
