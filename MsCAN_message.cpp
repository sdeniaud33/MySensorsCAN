#include "Arduino.h"
#include "MsCAN_message.h"
#include "MsCAN_messageHeader.h"
    MsCAN_message::MsCAN_message()
	{
	}

    MsCAN_message::MsCAN_message(MsCAN_messageHeader header, double value, bool ack = false) :
	_header(header), _ack(ack), _payloadType(P_DOUBLE)
	{
		_value._double = value;	
	}
    MsCAN_message::MsCAN_message(MsCAN_messageHeader header, int value, bool ack = false) :
	_header(header), _ack(ack), _payloadType(P_INTEGER)
	{
		_value._integer = value;	
	}
    MsCAN_message::MsCAN_message(MsCAN_messageHeader header, bool value, bool ack = false) :
	_header(header), _ack(ack), _payloadType(P_BOOLEAN)
	{
		_value._boolean = value;	
	}
	
	static MsCAN_payloadType MsCAN_message::MySensorType2CANType(MsCAN_typeDef_type mySensorType) {
		switch(mySensorType) {
			case V_TEMP: 
			case V_HUM: 
				return P_DOUBLE;
			case V_STATUS: 
			case V_TRIPPED: 
				return P_BOOLEAN;
			default:
				return P_INTEGER;
		}
	}
	
	static MsCAN_message MsCAN_message::fromBytes(MsCAN_typeDef_CANId CANId, byte bytes[], byte len) {
		union TypeConverter converter;
/*		
		for (int i = 0; i < len; i++) {
			Serial.print(bytes[i]);
			Serial.print(";");
		}
		Serial.println();
*/
		memcpy(converter._bytes, bytes, len);
		CANId = CANId & 0x1FFFFFFF;
		MSC_parseResult parseResult = MsCAN_message::parseCANId(CANId);
		MsCAN_message msg = MsCAN_message();
		msg._header = parseResult.messageHeader;
		msg._ack = parseResult.ack;
		msg._value = converter;
		msg._CANId = CANId;
		if (parseResult.messageHeader.getCommand() == C_INTERNAL) {
			// For now, consider all internal messages as integer
			msg._payloadType = P_INTEGER;
		}
		else {
			msg._payloadType = MsCAN_message::MySensorType2CANType(parseResult.messageHeader.getType());
		}
		return msg;
	}
	
	bool MsCAN_message::isValueBoolean() { return _payloadType == P_BOOLEAN; }
	bool MsCAN_message::isValueDouble() { return _payloadType == P_DOUBLE; }
	bool MsCAN_message::isValueInteger() { return _payloadType == P_INTEGER; }
	
	MsCAN_messageHeader MsCAN_message::getHeader() {
		return _header;
	}
	bool MsCAN_message::getAck() {
		return _ack;
	}
	double MsCAN_message::getValueAsDouble() {
		return _value._double;
	}
	int MsCAN_message::getValueAsInteger() {
		return _value._integer;
	}
	bool MsCAN_message::getValueAsBoolean() {
		return _value._boolean;
	}
	byte* MsCAN_message::getValueAsBytes() {
		return _value._bytes;
	}
	
	void MsCAN_message::setCANId(MsCAN_typeDef_CANId CANId) {
		_CANId = CANId;
	}
	MsCAN_typeDef_CANId MsCAN_message::getCANId() {
		return _CANId;
	}
	
MSC_parseResult static MsCAN_message::parseCANId(MsCAN_typeDef_CANId CANId) {
	MSC_parseResult out;

	CANId = CANId & 0x1FFFFFFF;
	// CanId format :
	// nodeId   : 10 bits (0 -> 1024)
	// sensorId : 10 bits (0 -> 1024)
	// command  :  2 bits (0 -> 3)
	// ack      :  1 bits (0 -> 1)
	// type     :  6 bits (0 -> 63)
	// ----------------------------
	// total    : 29 bits
	MsCAN_typeDef_fullNodeId fullNodeId;
	MsCAN_typeDef_sensorId sensorId;
	MsCAN_typeDef_command command;
	byte type;

	type = CANId & 0x3F; // 6 bits
	CANId = CANId >> 6;
	out.ack = CANId & 0x1; // 1 bit
	CANId = CANId >> 1;
	command = CANId & 0x3; // 2 bit
	CANId = CANId >> 2;
	sensorId = CANId & 0x3FF; // 10 bit
	CANId = CANId >> 10;
	fullNodeId = CANId & 0x3FF; // 10 bit;
	out.messageHeader = MsCAN_messageHeader(fullNodeId, sensorId, command, type);
	return out;
}


	
void MsCAN_message::dumpToSerial() {
	#if FULL_DUMP
		Serial.print("- FullNodeId : ");
		Serial.print(getHeader().getFullNodeId());
		Serial.println();
		Serial.print("- SensorId   : ");
		Serial.print(getHeader().getSensorId());
		Serial.println();
		Serial.print("- Command    : ");
		Serial.print(getHeader().getCommand());
		Serial.println();
		Serial.print("- Ack        : ");
		Serial.print(getAck());
		Serial.println();
		Serial.print("- Type       : ");
		Serial.print(getHeader().getType());
		Serial.println();
		if (isValueBoolean()) {
			Serial.print("- Value (boolean) : ");
			Serial.print(getValueAsBoolean());
		}
		else if (isValueDouble()) {
			Serial.print("- Value (double) : ");
			Serial.print(getValueAsDouble());
		}
		else if (isValueInteger()) {
			Serial.print("- Value (integer) : ");
			Serial.print(getValueAsInteger());
		}
		Serial.println();
	#else
		Serial.print(getHeader().getFullNodeId());
		Serial.print(";");
		Serial.print(getHeader().getSensorId());
		Serial.print(";");
		Serial.print(getHeader().getCommand());
		Serial.print(";");
		Serial.print(getAck());
		Serial.print(";");
		Serial.print(getHeader().getType());

		Serial.print(";");
		if (isValueBoolean()) {
			Serial.print(getValueAsBoolean());
//			Serial.print(" - boolean");
		}
		else if (isValueDouble()) {
			Serial.print(getValueAsDouble());
//			Serial.print(" - double");
		}
		else if (isValueInteger()) {
			Serial.print(getValueAsInteger());
//			Serial.print(" - integer");
		}

		Serial.println();
	#endif
}

