#include "Arduino.h"
#include "MsCAN_messageHeader.h"

MsCAN_messageHeader::MsCAN_messageHeader() {
    
}

MsCAN_messageHeader::MsCAN_messageHeader(
		MsCAN_typeDef_fullNodeId fullNodeId, 
		MsCAN_typeDef_sensorId sensorId, 
		MsCAN_typeDef_command command, 
		MsCAN_typeDef_type type):
  _fullNodeId(fullNodeId), _sensorId(sensorId), _command(command), _type(type)
{
}

MsCAN_typeDef_fullNodeId MsCAN_messageHeader::getFullNodeId() {
	return _fullNodeId;
}
MsCAN_typeDef_sensorId MsCAN_messageHeader::getSensorId() {
	return _sensorId;
}
MsCAN_typeDef_command MsCAN_messageHeader::getCommand() {
	return _command;
}
MsCAN_typeDef_type MsCAN_messageHeader::getType() {
	return _type;
}

MsCAN_typeDef_CANId static MsCAN_messageHeader::buildCANId(
		MsCAN_typeDef_fullNodeId fullNodeId, 
		MsCAN_typeDef_sensorId sensorId, 
		MsCAN_typeDef_command command, 
		MsCAN_typeDef_type type,
		boolean ack = false) {	
	// CanId format :
	// nodeId   : 10 bits (0 -> 1024)
	// sensorId : 10 bits (0 -> 1024)
	// command  :  2 bits (0 -> 3) MySensors_command : C_PRESENTATION | C_SET | C_REQ | C_INTERNAL | C_STREAM
	// ack      :  1 bits (0 -> 1)
	// type     :  6 bits (0 -> 63) MySensors_data | MySensors_internal : V_TEMP | V_HUM | V_STATUS, I_BATTERY_LEVEL ....
	// ----------------------------
	// total    : 29 bits
	MsCAN_typeDef_CANId canId = fullNodeId;
	canId = canId << 10;
	canId = canId | sensorId;
	canId = canId << 2;
	canId = canId | command;
	canId = canId << 1;
	canId = canId | ack;
	canId = canId << 6;
	canId = canId | type;

	return canId;
}

MsCAN_typeDef_CANId MsCAN_messageHeader::getCANId(bool ack = false) {
    return buildCANId(_fullNodeId, _sensorId, _command, _type, ack);
}



