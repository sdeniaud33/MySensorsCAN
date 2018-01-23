#include "Arduino.h"
#include "MsCAN_handler.h"

MsCAN_handler::MsCAN_handler(byte spi_cs_pin)
  : _can(new MCP_CAN(spi_cs_pin))
{

}

boolean MsCAN_handler::init(byte speed)
{
	if (CAN_OK != _can->begin(MCP_ANY, speed, MCP_8MHZ))
		return false;
	_can->setMode(MCP_NORMAL);
	return true;
}

byte MsCAN_handler::send(MsCAN_messageHeader msgHeader, double value, bool ack=false) {
	MsCAN_message msg = MsCAN_message(msgHeader, value, ack);
	return sendMessage(msgHeader, msg.getValueAsBytes(), ack);
}

byte MsCAN_handler::send(MsCAN_messageHeader msgHeader, int value, bool ack=false) {
	MsCAN_message msg = MsCAN_message(msgHeader, value, ack);
	return sendMessage(msgHeader, msg.getValueAsBytes(), ack);
}

byte MsCAN_handler::send(MsCAN_messageHeader msgHeader, bool value, bool ack=false) {
	MsCAN_message msg = MsCAN_message(msgHeader, value, ack);
	return sendMessage(msgHeader, msg.getValueAsBytes(), ack);
}

byte MsCAN_handler::send(MsCAN_message message) {
	return _can->sendMsgBuf(message.getHeader().getCANId(), 1, 8, message.getValueAsBytes()); // 1 = extended ; 8 = len
}

byte MsCAN_handler::sendMessage(MsCAN_messageHeader msgHeader, byte payload[], bool ack = false, byte len = 8) {
	MsCAN_typeDef_CANId CANId = msgHeader.getCANId(ack);
/*	
	Serial.print("Len = ");
	Serial.print(len);
	Serial.print(" - ");
	for (int i = 0; i < len; i++) {
		Serial.print((byte) payload[i]);
		Serial.print(";");
	}
	Serial.println();
*/
	return _can->sendMsgBuf(CANId, 1, len, payload); // 1 = extended
}

byte MsCAN_handler::sendRawMessage(
		MsCAN_typeDef_fullNodeId fullNodeId, 
		MsCAN_typeDef_sensorId sensorId, 
		MsCAN_typeDef_command command, 
		MsCAN_typeDef_type type, 
		byte payload[],
		boolean ack = false, 
		byte len = 8) {
	MsCAN_typeDef_CANId CANId = MsCAN_messageHeader::buildCANId(fullNodeId, sensorId, command, type, ack);
/*	
	for (int i = 0; i < len; i++) {
		Serial.print((byte)payload[i]);
		Serial.print(",");
	}
	Serial.println();
*/
	MsCAN_message msg = MsCAN_message::fromBytes(CANId, payload, len);
	return send(msg);
}

byte MsCAN_handler::checkReceive() {
	return _can->checkReceive();
}

byte MsCAN_handler::readMsgBuf(MsCAN_messageHeader *msgHeader, byte *len, byte payload[], bool *ack) {
	MsCAN_typeDef_CANId canId;
	byte result =  _can->readMsgBuf(&canId, len, payload);
	canId = canId & 0x1FFFFFFF;
	MSC_parseResult parseResult = MsCAN_message::parseCANId(canId);
	*msgHeader = parseResult.messageHeader;
	*ack = parseResult.ack;
	return result;
}

void MsCAN_handler::sendBatteryLevel(MsCAN_typeDef_fullNodeId fullNodeId, MsCAN_typeDef_sensorId sensorId, int batteryLevel) {
	TypeConverter typeConverter;
	typeConverter._integer = batteryLevel;
	sendRawMessage(
		fullNodeId, 
		sensorId, 
		C_INTERNAL, 
		I_BATTERY_LEVEL, 
		typeConverter._bytes);
}



void MsCAN_handler::tick() {
	if (receive) {		
		if (CAN_MSGAVAIL == checkReceive()) { // check if data coming
			MsCAN_typeDef_CANId canId;
			byte len;
			byte bytes[8];
			byte result =  _can->readMsgBuf(&canId, &len, bytes);
			MsCAN_message msg = MsCAN_message::fromBytes(canId, bytes, len);
			receive(msg);
		}
	}
}

