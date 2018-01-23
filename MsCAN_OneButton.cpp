#include "MsCAN_OneButton.h"

MsCAN_OneButton::MsCAN_OneButton(MsCAN_OneButtonListener *listener, int pin, int activeLow) : _listener(listener)
{
  pinMode(pin, INPUT);      // sets the MenuPin as input
  _pin = pin;

  _clickTicks = 100;        // number of millisec that have to pass by before a click is detected.
  _pressTicks = 1000;       // number of millisec that have to pass by before a long button press is detected.
 
  _state = 0; // starting with state 0: waiting for button to be pressed
  _isLongPressed = false;  // Keep track of long press state

  if (activeLow) {
    // button connects ground to the pin when pressed.
    _buttonReleased = HIGH; // notPressed
    _buttonPressed = LOW;
    digitalWrite(pin, HIGH);   // turn on pullUp resistor

  } else {
    // button connects VCC to the pin when pressed.
    _buttonReleased = LOW;
    _buttonPressed = HIGH;
  } // if

} // MsCAN_OneButton


// explicitly set the number of millisec that have to pass by before a click is detected.
void MsCAN_OneButton::setClickTicks(int ticks) { 
  _clickTicks = ticks;
} // setClickTicks


// explicitly set the number of millisec that have to pass by before a long button press is detected.
void MsCAN_OneButton::setPressTicks(int ticks) {
  _pressTicks = ticks;
} // setPressTicks

// function to get the current long pressed state
bool MsCAN_OneButton::isLongPressed(){
  return _isLongPressed;
}

void MsCAN_OneButton::tick(void)
{
  // Detect the input information 
  int buttonLevel = digitalRead(_pin); // current button signal.
  unsigned long now = millis(); // current (relative) time in msecs.

  // Implementation of the state machine
  if (_state == 0) { // waiting for menu pin being pressed.
    if (buttonLevel == _buttonPressed) {
      _state = 1; // step to state 1
      _startTime = now; // remember starting time
    } // if

  } else if (_state == 1) { // waiting for menu pin being released.

    if ((buttonLevel == _buttonReleased) && ((unsigned long)(now - _startTime) < _debounceTicks)) {
      // button was released to quickly so I assume some debouncing.
	  // go back to state 0 without calling a function.
      _state = 0;

    } else if (buttonLevel == _buttonReleased) {
      _state = 2; // step to state 2

    } else if ((buttonLevel == _buttonPressed) && ((unsigned long)(now - _startTime) > _pressTicks)) {
      _isLongPressed = true;  // Keep track of long press state
      _listener->buttonPress();
	  _listener->buttonLongPressStart();
	  _listener->buttonDuringLongPress();
      _state = 6; // step to state 6
      
    } else {
      // wait. Stay in this state.
    } // if

  } else if (_state == 2) { // waiting for menu pin being pressed the second time or timeout.
    if ((unsigned long)(now - _startTime) > _clickTicks) {
      // this was only a single short click
      _listener->buttonClick();
      _state = 0; // restart.

    } else if (buttonLevel == _buttonPressed) {
      _state = 3; // step to state 3
    } // if

  } else if (_state == 3) { // waiting for menu pin being released finally.
    if (buttonLevel == _buttonReleased) {
      // this was a 2 click sequence.
	  _listener->buttonDoubleClick();
      _state = 0; // restart.
    } // if

  } else if (_state == 6) { // waiting for menu pin being release after long press.
    if (buttonLevel == _buttonReleased) {
	  _isLongPressed = false;  // Keep track of long press state
	  _listener->buttonLongPressStop();
      _state = 0; // restart.
    } else {
	  // button is being long pressed
	  _isLongPressed = true; // Keep track of long press state
	  _listener->buttonDuringLongPress();
    } // if  

  } // if  
} // MsCAN_OneButton.tick()


// end.

