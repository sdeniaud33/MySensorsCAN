#ifndef MsCAN_shutter_h
#define MsCAN_shutter_h

#include "ACS712.h"
#include "MsCAN_handler.h"
#include "MsCAN_messageHeader.h"
#include "MsCAN_abstractDevice.h"

#define EMULATE 1

struct TargetStruct
{
	bool enabled;
	int position;
};

enum PositionEnum
{
	positionUnknown, // The shutter is neither open, nor closed
	positionOpen,	// The shutter is open
	positionClosed   // The shutter is closed
};

struct PositionStruct
{
	float percent;
	PositionEnum status;
	unsigned long lastDisplayedTime;
};

enum DirectionEnum
{
	directionNone,	// The shutter is stopped
	directionOpening, // The shutter is opening
	directionClosing  // The shutter is closing
};

struct MovementStruct
{
	DirectionEnum direction;
	unsigned long startTime;	 // The startTime of the movement start
	float startPos;				 // The position of the ruller when the movement started
	long openingDuration;		 // Duration of full 'opening' movement
	long closingDuration;		 // Duration of full 'closing' movement
	bool measureClosingDuration; // Should the 'closing' full movement duration be measured ?
	bool measureOpeningDuration; // Should the 'opening' full movement duration be measured ?
};

class MsCAN_shutter : public MsCAN_abstractDevice
{
	private:
		byte _openRelayPin;
		byte _closeRelayPin;
		byte _currentSensorPin;
		TargetStruct _target;
		PositionStruct _position;
		MovementStruct _movement;
		bool _lastDirectionWasOpening = false;
		float _consumptionThreshold = 0;
		ACS712 _sensor;
		const float _thresholdFactor = 0.8;
		
		MsCAN_messageHeader _messageHeaderPosition;
		void sendStatus();	
		int getPercentPosition(); // O = open, 100 = closed
		void calibrate();
		void setPinsForMovement();


	public:
		MsCAN_shutter(
				MsCAN_handler can, 
				byte openRelayPin, 
				byte closeRelayPin, 
				byte currentSensorPin,
				MsCAN_typeDef_fullNodeId nodeId, 
				MsCAN_typeDef_sensorId sensorId);
		bool parse(MsCAN_message message);
		void tick();
		void setTargetPosition(int percentPosition);
};

#endif
