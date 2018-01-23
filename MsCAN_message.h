#ifndef MsCAN_message_h
#define MsCAN_message_h

#include "MsCAN_types.h"
#include "MsCAN_messageHeader.h"

typedef struct
{
	MsCAN_messageHeader messageHeader;
	bool ack;
} MSC_parseResult;


union TypeConverter
{
	int _integer;
	byte _bytes[8];
	double _double;
	bool _boolean;
	char _string[8];
};

class MsCAN_message
{
	private:
		bool _ack;
		MsCAN_messageHeader _header;
		TypeConverter _value;
		MsCAN_typeDef_CANId _CANId;
		byte _payloadType;
		MsCAN_message();

	public:
		MSC_parseResult static parseCANId(MsCAN_typeDef_CANId CANId);
		MsCAN_message(MsCAN_messageHeader header, double value, bool ack = false);
		MsCAN_message(MsCAN_messageHeader header, int value, bool ack = false);
		MsCAN_message(MsCAN_messageHeader header, bool value, bool ack = false);
		
		static MsCAN_payloadType MySensorType2CANType(MsCAN_typeDef_type mySensorType);

		static MsCAN_message fromBytes(MsCAN_typeDef_CANId CANId, byte bytes[], byte len);

		MsCAN_messageHeader getHeader();
		bool getAck();
		bool isValueBoolean();
		bool isValueDouble();
		bool isValueInteger();
		double getValueAsDouble();
		int getValueAsInteger();
		bool getValueAsBoolean();
		byte* getValueAsBytes();
		void setCANId(MsCAN_typeDef_CANId canId);
		MsCAN_typeDef_CANId getCANId();
		void dumpToSerial();
};

#endif
