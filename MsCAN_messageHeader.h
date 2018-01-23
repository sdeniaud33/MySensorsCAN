#ifndef MsCAN_messageHeader_h
#define MsCAN_messageHeader_h

#include "MsCAN_types.h"
#include <mcp_can.h>
#include <mcp_can_dfs.h>
#include <SPI.h>

class MsCAN_messageHeader
{
	private:
		MsCAN_typeDef_fullNodeId _fullNodeId;
		MsCAN_typeDef_sensorId _sensorId;
		MsCAN_typeDef_command _command;
		MsCAN_typeDef_type _type; // 
	public:
		MsCAN_messageHeader();
		MsCAN_messageHeader(
			MsCAN_typeDef_fullNodeId fullNodeId, 
			MsCAN_typeDef_sensorId sensorId, 
			MsCAN_typeDef_command command, 
			MsCAN_typeDef_type type);
		MsCAN_typeDef_fullNodeId getFullNodeId();
		MsCAN_typeDef_sensorId getSensorId();
		MsCAN_typeDef_type getCommand();
		MsCAN_typeDef_type getType();
		MsCAN_typeDef_CANId static buildCANId(
			MsCAN_typeDef_fullNodeId fullNodeId, 
			MsCAN_typeDef_sensorId sensorId, 
			MsCAN_typeDef_command command, 
			MsCAN_typeDef_type type,
			boolean ack = false);
		MsCAN_typeDef_CANId getCANId(bool ack = false);
};

#endif
