#ifndef MsCAN_OneButtonListener_h
#define MsCAN_OneButtonListener_h

#include "Arduino.h"

class MsCAN_OneButtonListener
{
public:
	virtual void buttonPress() {}
	virtual void buttonLongPressStart() {}
	virtual void buttonDoubleClick() {}
	virtual void buttonLongPressStop() {}
	virtual void buttonDuringLongPress() {}
	virtual void buttonClick() {}
};

#endif


