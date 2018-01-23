#ifndef MsCAN_light_h
#define MsCAN_light_h

#include "MsCAN_handler.h"
#include "MsCAN_messageHeader.h"
#include "MsCAN_abstractDevice.h"

class MsCAN_light : public MsCAN_abstractDevice
{
	private:
		MsCAN_messageHeader _messageHeader;
		void sendStatus();	
		void setStatus(bool status);
		bool getStatus();

	public:
		MsCAN_light(
				MsCAN_handler can, 
				byte pin, 
				MsCAN_typeDef_fullNodeId nodeId, 
				MsCAN_typeDef_sensorId sensorId);
		bool parse(MsCAN_message message);
		void tick();
};

#endif
