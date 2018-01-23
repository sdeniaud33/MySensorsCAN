#ifndef MsCAN_pushButton_h
#define MsCAN_pushButton_h
#include "MsCAN_OneButton.h"

#include "MsCAN_handler.h"
#include "MsCAN_messageHeader.h"
#include "MsCAN_abstractDevice.h"
#include "MsCAN_OneButtonListener.h"

class MsCAN_pushButton : public MsCAN_OneButtonListener, public MsCAN_abstractDevice
{
	private:
		MsCAN_messageHeader _messageHeaderSingleClick;
		MsCAN_messageHeader _messageHeaderLongPress;
		MsCAN_OneButton *_oneButton;

	public:
		MsCAN_pushButton(
				MsCAN_handler can, 
				byte pin, 
				MsCAN_typeDef_fullNodeId nodeId, 
				MsCAN_typeDef_sensorId sensorId);
		void tick();
		bool parse(MsCAN_message message);
		
		void buttonClick();
		void buttonLongPressStart();
		
};

#endif
