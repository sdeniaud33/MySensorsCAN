#include "Arduino.h"
#include "MsCAN_pushButton.h"

MsCAN_pushButton::MsCAN_pushButton(
		MsCAN_handler can, 
		byte pin, 
		MsCAN_typeDef_fullNodeId nodeId, 
		MsCAN_typeDef_sensorId sensorId) : 
	MsCAN_abstractDevice(can, pin, nodeId, sensorId),
	MsCAN_OneButtonListener(),
	_messageHeaderSingleClick(MsCAN_messageHeader(nodeId, sensorId, C_SET, V_TRIPPED)),
	_messageHeaderLongPress(MsCAN_messageHeader(nodeId, sensorId, C_SET, V_VAR1))
{
	_oneButton = new MsCAN_OneButton(this, pin, true);
}

void MsCAN_pushButton::tick() {
	_oneButton->tick();
}

bool MsCAN_pushButton::parse(MsCAN_message message) {
	// Nothing to do
}

void MsCAN_pushButton::buttonClick() {
	Serial.print("CLICK ");
	Serial.println(_pin);
	_can.send(_messageHeaderSingleClick, true);
	// Serial.println(freeRam());
}

void MsCAN_pushButton::buttonLongPressStart() {
	Serial.print("LONG PRESS ");
	Serial.println(_pin);
	_can.send(_messageHeaderLongPress, 2);
	// Serial.println(freeRam());
}


