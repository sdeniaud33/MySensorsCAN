#ifndef MsCAN_handler_h
#define MsCAN_handler_h

#include <mcp_can.h>
#include <mcp_can_dfs.h>
#include <SPI.h>
#include "MsCAN_message.h"
#include "MsCAN_messageHeader.h"
#include "MsCAN_types.h"

void receive(const MsCAN_message message)  __attribute__((weak));

class MsCAN_handler
{
	private:
		MCP_CAN *_can;
		byte sendMessage(MsCAN_messageHeader msgHeader, byte payload[], bool ack = false, byte len = 8);
		byte checkReceive();
		byte readMsgBuf(MsCAN_messageHeader *msgHeader, byte *len, byte payload[], bool *ack);
	public:
		byte sendRawMessage(
			MsCAN_typeDef_fullNodeId fullNodeId, 
			MsCAN_typeDef_sensorId sensorId, 
			MsCAN_typeDef_command command, 
			MsCAN_typeDef_type type, 
			byte payload[],
			boolean ack = false,
			byte len = 8);
		MsCAN_handler(byte spi_cs_pin);
		boolean init(byte speed);
		byte send(MsCAN_message message);
		byte send(MsCAN_messageHeader msgHeader, double value, bool ack=false);
		byte send(MsCAN_messageHeader msgHeader, int value, bool ack=false);
		byte send(MsCAN_messageHeader msgHeader, bool value, bool ack=false);

		void tick();
		void sendBatteryLevel(MsCAN_typeDef_fullNodeId fullNodeId, MsCAN_typeDef_sensorId sensorId, int batteryLevel);

};
#endif
