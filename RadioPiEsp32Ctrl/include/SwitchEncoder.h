#ifndef SwitchEncoder_h
#define SwitchEncoder_h

#include "Arduino.h"

// Values returned by 'check'
#define DIR_NONE 0x00
#define DIR_CW   0x10   // Clockwise step.
#define DIR_CCW  0x20   // Anti-clockwise step.

class SwitchEncoder
{
	public:
		SwitchEncoder(char CHA, char CHB, char SWI);
    
    void attachFunctionOnButtonPressedEdge(void (*p_func)());
    void attachFunctionOnButtonReleasedEdge(void (*p_func)());
    void attachFunctionOnClockwiseTurn(void (*p_func)());
    void attachFunctionOnCounterClockwiseTurn(void (*p_func)());
    
    void init();
    void check();
		
	private:
    boolean getValue();
		unsigned char state;
		unsigned char _cha;
		unsigned char _chb;
		unsigned char _swi;

    void (*p_fctButtonPressed)(void);
    void (*p_fctButtonReleased)(void);
    void (*p_fctClockwiseTurn)(void);
    void (*p_fctCounterClockwiseTurn)(void);

    boolean switchValue;
    unsigned char lastA;
    unsigned char lastB;
};

#endif
 
