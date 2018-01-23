#include "Arduino.h"
#include "MsCAN_light.h"

MsCAN_light::MsCAN_light(
		MsCAN_handler can, 
		byte pin, 
		MsCAN_typeDef_fullNodeId nodeId, 
		MsCAN_typeDef_sensorId sensorId) : 
	MsCAN_abstractDevice(can, pin, nodeId, sensorId),
	_messageHeader(MsCAN_messageHeader(nodeId, sensorId, C_SET, V_VAR1))
{
    pinMode(_pin, OUTPUT);
	this->sendStatus();
}

void MsCAN_light::sendStatus() {
	_can.send(_messageHeader, this->getStatus() ? 1 : 0);
}

bool MsCAN_light::getStatus() {
	return digitalRead(_pin) == HIGH;
}

void MsCAN_light::setStatus(bool status) {
	digitalWrite(_pin, status ? HIGH : LOW);
	this->sendStatus();
}

void MsCAN_light::tick() {
	
}

bool MsCAN_light::parse(MsCAN_message message) {
	MsCAN_messageHeader msgHeader=message.getHeader();
  if (msgHeader.getSensorId() == 0 && msgHeader.getFullNodeId() == 0 && message.getValueAsInteger() == 1) {
      // global request : request status for lights
	  this->sendStatus();
	  return false;
  }

  if (msgHeader.getFullNodeId() != _messageHeader.getFullNodeId())
	  return false;
  if (msgHeader.getSensorId() != _messageHeader.getSensorId())
	  return false; 
  if (msgHeader.getType() != V_VAR1)
	  return false;
  switch(message.getValueAsInteger()) {
	  case 0: // light off
        Serial.println(">>>>>> OFF");
		this->setStatus(false);
		return true;
		break;
	  case 1: // light on
        Serial.println(">>>>>> ON");
		this->setStatus(true);
		return true;
		break;
	  case 2: // light toggle
        Serial.println(">>>>>> TOGGLE");
		this->setStatus(!this->getStatus());
		return true;
		break;
	default:
		Serial.print("UNPARSED VALUE : ");
		Serial.println(message.getValueAsInteger());
		break;
  }
  return false;
}
