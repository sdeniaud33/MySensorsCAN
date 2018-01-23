#include "Arduino.h"
#include "MsCAN_shutter.h"

MsCAN_shutter::MsCAN_shutter(
		MsCAN_handler can, 
		byte openRelayPin, 
		byte closeRelayPin, 
		byte currentSensorPin,
		MsCAN_typeDef_fullNodeId nodeId, 
		MsCAN_typeDef_sensorId sensorId) : 
	MsCAN_abstractDevice(can, 0, nodeId, sensorId),
	_openRelayPin(openRelayPin),
	_closeRelayPin(closeRelayPin),
	_currentSensorPin(currentSensorPin),
	_messageHeaderPosition(MsCAN_messageHeader(nodeId, sensorId, C_SET, V_PERCENTAGE)),
	_sensor(ACS712(ACS712_05B, currentSensorPin))
{
	this->sendStatus();
	_position = {0, positionUnknown};
	_movement.direction = directionNone;
#if EMULATE == 0
	_sensor.calibrate();
	_movement.openingDuration = 24200;
	_movement.closingDuration = 21500;
#else
	_movement.openingDuration = 10000;
	_movement.closingDuration = 10000;
#endif
	_position.percent = 50;
	
}

void MsCAN_shutter::sendStatus() {
	Serial.print("Pos = ");
	Serial.print(getPercentPosition());
	if (_target.enabled) {
		Serial.print(" / ");
		Serial.print(_target.position);
	}
	Serial.println();
	_can.send(_messageHeaderPosition, getPercentPosition());
}

int MsCAN_shutter::getPercentPosition() {
	return _position.percent;
}


void MsCAN_shutter::setTargetPosition(int pos)
{
	if (pos == _position.percent)
		return;
	_target.position = pos;
	_target.enabled = true;
	if (pos < _position.percent)
	{
		_movement.direction = directionOpening;
		Serial.print("Target -> opening to ");
		Serial.print(pos);
		Serial.println("%");
	}
	else
	{
		_movement.direction = directionClosing;
		Serial.print("Target -> closing to ");
		Serial.print(pos);
		Serial.println("%");
	}
	setPinsForMovement();
}


bool MsCAN_shutter::parse(MsCAN_message message) {
	MsCAN_messageHeader msgHeader=message.getHeader();
  if (msgHeader.getSensorId() == 0 && msgHeader.getFullNodeId() == 0 && message.getValueAsInteger() == 1) {
      // global request : request status for devices
	  this->sendStatus();
	  return false;
  }
  if (msgHeader.getFullNodeId() != _messageHeaderPosition.getFullNodeId())
	  return false;
  if (msgHeader.getSensorId() != _messageHeaderPosition.getSensorId())
	  return false;  
Serial.println("Shutter message");
  switch(msgHeader.getType()) {
	  case V_UP: // up
        Serial.println(">>>>>> OFF");
		_movement.direction = directionOpening; 
		setPinsForMovement();
		return true;
		break;
	  case V_DOWN: // down
        Serial.println(">>>>>> ON");
		_movement.direction = directionClosing; 
		setPinsForMovement();
		return true;
		break;
	  case V_STOP: // stop
        Serial.println(">>>>>> STOP");
		_movement.direction = directionNone; 
		setPinsForMovement();
		return true;
		break;
	  case V_VAR1: // toggle
        Serial.println(">>>>>> TOGGLE");
		if (_movement.direction == directionNone)
			_movement.direction = (_lastDirectionWasOpening ? directionClosing : directionOpening);
		else
			_movement.direction = directionNone;
		setPinsForMovement();
		return true;
		break;
	default:
		Serial.print("UNPARSED VALUE : ");
		Serial.println(message.getValueAsInteger());
		break;
  }
  return false;
}

void MsCAN_shutter::calibrate() {
#if EMULATE == 0		
	Serial.println("Calibration");
	digitalWrite(_openRelayPin, LOW);
	digitalWrite(_closeRelayPin, LOW);
	float val0 = _sensor.getCurrentAC();
	val0 = max(val0, _sensor.getCurrentAC());
	digitalWrite(_openRelayPin, HIGH);
	delay(800);
	float valOpening = _sensor.getCurrentAC();
	valOpening = max(valOpening, _sensor.getCurrentAC());
	delay(200);
	digitalWrite(_openRelayPin, LOW);
	delay(1000);
	digitalWrite(_closeRelayPin, HIGH);
	delay(800);
	float valClosing = _sensor.getCurrentAC();
	valClosing = max(valClosing, _sensor.getCurrentAC());
	delay(200);
	digitalWrite(_closeRelayPin, LOW);
	delay(500);
	float val1 = _sensor.getCurrentAC();
	val1 = max(val1, _sensor.getCurrentAC());
	_consumptionThreshold = min(valOpening, valClosing) * _thresholdFactor;
	Serial.print("  valOpening = ");
	Serial.println(valOpening);
	Serial.print("  valClosing = ");
	Serial.println(valClosing);
	Serial.print("  val0 = ");
	Serial.println(val0);
	Serial.print("  val1 = ");
	Serial.println(val1);
	Serial.print("  consumptionThreshold = ");
	Serial.println(_consumptionThreshold);
#endif
}

void MsCAN_shutter::setPinsForMovement()
{
	switch (_movement.direction)
	{
		case directionNone:
#if EMULATE == 0		
			digitalWrite(_openRelayPin, LOW);
			digitalWrite(_closeRelayPin, LOW);
#endif
			Serial.println("Mvt : stop");
			_target.enabled = false;
			_movement.measureClosingDuration = false;
			_movement.measureOpeningDuration = false;
			sendStatus();
			break;
		case directionOpening:
			_movement.measureOpeningDuration = (_position.status == positionClosed);
			Serial.print("measureOpeningDuration = ");
			Serial.println(_movement.measureOpeningDuration);
#if EMULATE == 0		
			digitalWrite(_closeRelayPin, LOW);
			digitalWrite(_openRelayPin, HIGH);
#endif
			Serial.println("Mvt : open");
			_movement.startTime = millis();
			_movement.startPos = _position.percent;
			_lastDirectionWasOpening = true;
			break;
		case directionClosing:
			_movement.measureClosingDuration = (_position.status == positionOpen);
			Serial.print("measureClosingDuration = ");
			Serial.println(_movement.measureClosingDuration);
#if EMULATE == 0		
			digitalWrite(_openRelayPin, LOW);
			digitalWrite(_closeRelayPin, HIGH);
#endif
			Serial.println("Mvt : close");
			_movement.startTime = millis();
			_movement.startPos = _position.percent;
			_lastDirectionWasOpening = false;
			break;
	}
	// Make sure that the ACS712 will not be requested before the motor
	// consumes power
	delay(200);
}

void MsCAN_shutter::tick()
{
	if (_movement.direction == directionNone)
		return;
	unsigned long now = millis();
#if EMULATE == 0		
	float valCur = _sensor.getCurrentAC();
	// Serial.println(valCur);
	if (valCur < _consumptionThreshold)
	{
		// The motor of the shutter does not consumes any power : the shutter is
		// either fully closed or fully open
		if (_movement.direction == directionOpening)
		{
			// The shutter is fully open
			_position.status = positionOpen;
			_position.percent = 0;
			if (_movement.measureOpeningDuration)
			{
				_movement.openingDuration = now - _movement.startTime;
				Serial.print("openingDuration = ");
				Serial.println(_movement.openingDuration);
			}
		}
		else
		{
			// The shutter is fully closed
			_position.status = positionClosed;
			_position.percent = 100;
			if (_movement.measureClosingDuration)
			{
				_movement.closingDuration = now - _movement.startTime;
				Serial.print("closingDuration = ");
				Serial.println(_movement.closingDuration);
			}
		}
		_movement.direction = directionNone;
		Serial.print("Stopped : ");
		Serial.println(valCur);
		setPinsForMovement();
	}
#else
	if (_movement.direction == directionOpening) {
		if (_position.percent == 0) {
			_movement.direction = directionNone;
			setPinsForMovement();
		}
	}
	else if (_movement.direction == directionClosing) {
		if (_position.percent == 100) {
			_movement.direction = directionNone;
			setPinsForMovement();
		}
	}

#endif
	if ((_movement.direction == directionOpening) && (_movement.openingDuration > 0))
	{
		// compute the new position
		long mvtDuration = now - _movement.startTime;
		float posDelta = ((float)mvtDuration / _movement.openingDuration) * 100;
		_position.percent = _movement.startPos - posDelta;
		if (_position.percent < 0)
			_position.percent = 0;
	}
	else if ((_movement.direction == directionClosing) && (_movement.closingDuration > 0))
	{
		// compute the new position
		long mvtDuration = now - _movement.startTime;
		float posDelta = ((float)mvtDuration / _movement.closingDuration) * 100;
		_position.percent = _movement.startPos + posDelta;
		if (_position.percent > 100)
			_position.percent = 100;
	}

	if (_movement.direction != directionNone)
	{
		int posToDisplay = (int)_position.percent;
		if (now - _position.lastDisplayedTime > 500)
		{
			sendStatus();
			_position.lastDisplayedTime = now;
		}
	}

	if (_target.enabled)
	{
		if ((_movement.direction == directionClosing) && (_position.percent >= _target.position))
		{
			// The target position has been reached
			_movement.direction = directionNone;
			Serial.println("Target reached");
			setPinsForMovement();
		}
		else if ((_movement.direction == directionOpening) && (_position.percent <= _target.position))
		{
			// The target position has been reached
			_movement.direction = directionNone;
			Serial.println("Target reached");
			setPinsForMovement();
		}
	}	
}



