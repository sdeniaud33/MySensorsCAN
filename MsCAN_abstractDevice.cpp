#include "Arduino.h"
#include "MsCAN_abstractDevice.h"

MsCAN_abstractDevice::MsCAN_abstractDevice(
		MsCAN_handler can, 
		byte pin, 
		MsCAN_typeDef_fullNodeId nodeId, 
		MsCAN_typeDef_sensorId sensorId) : 
	_can(can), 
	_pin(pin),
	_nodeId(nodeId),
	_sensorId(sensorId)
{
}

