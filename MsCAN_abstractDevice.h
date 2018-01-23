#ifndef MsCAN_abstractDevice_h
#define MsCAN_abstractDevice_h
#include "MsCAN_handler.h"

class MsCAN_abstractDevice
{
	protected:
		MsCAN_handler _can;
		byte _pin;
		MsCAN_typeDef_fullNodeId _nodeId;
		MsCAN_typeDef_sensorId _sensorId;

	public:
		MsCAN_abstractDevice(
				MsCAN_handler can, 
				byte pin, 
				MsCAN_typeDef_fullNodeId nodeId, 
				MsCAN_typeDef_sensorId sensorId);
		virtual bool parse(MsCAN_message message);
		virtual void tick();
};

#endif
