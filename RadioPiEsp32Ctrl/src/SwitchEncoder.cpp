#include <SwitchEncoder.h>

/* Rotary encoder handler for arduino. v1.1
 *
 * Copyright 2011 Ben Buxton. Licenced under the GNU GPL Version 3.
 * Contact: bb@cactii.net
 *
 * A typical mechanical rotary encoder emits a two bit gray code
 * on 3 output pins. Every step in the output (often accompanied
 * by a physical 'click') generates a specific sequence of output
 * codes on the pins.
 *
 * There are 3 pins used for the rotary encoding - one common and
 * two 'bit' pins.
 *
 * The following is the typical sequence of code on the output when
 * moving from one step to the next:
 *
 *   Position   Bit1   Bit2
 *   ----------------------
 *     Step1     0      0
 *      1/4      1      0
 *      1/2      1      1
 *      3/4      0      1
 *     Step2     0      0
 *
 * From this table, we can see that when moving from one 'click' to
 * the next, there are 4 changes in the output code.
 *
 * - From an initial 0 - 0, Bit1 goes high, Bit0 stays low.
 * - Then both bits are high, halfway through the step.
 * - Then Bit1 goes low, but Bit2 stays high.
 * - Finally at the end of the step, both bits return to 0.
 *
 * Detecting the direction is easy - the table simply goes in the other
 * direction (read up instead of down).
 *
 * To decode this, we use a simple state machine. Every time the output
 * code changes, it follows state, until finally a full steps worth of
 * code is received (in the correct order). At the final 0-0, it returns
 * a value indicating a step in one direction or the other.
 *
 * It's also possible to use 'half-step' mode. This just emits an event
 * at both the 0-0 and 1-1 positions. This might be useful for some
 * encoders where you want to detect all positions.
 *
 * If an invalid state happens (for example we go from '0-1' straight
 * to '1-0'), the state machine resets to the start until 0-0 and the
 * next valid codes occur.
 *
 * The biggest advantage of using a state machine over other algorithms
 * is that this has inherent debounce built in. Other algorithms emit spurious
 * output with switch bounce, but this one will simply flip between
 * sub-states until the bounce settles, then continue along the state
 * machine.
 * A side effect of debounce is that fast rotations can cause steps to
 * be skipped. By not requiring debounce, fast rotations can be accurately
 * measured.
 * Another advantage is the ability to properly handle bad state, such
 * as due to EMI, etc.
 * It is also a lot simpler than others - a static state table and less
 * than 10 lines of logic.
 */

#define R_START     0x0
#define R_CW_FINAL  0x1
#define R_CW_BEGIN  0x2
#define R_CW_NEXT   0x3
#define R_CCW_BEGIN 0x4
#define R_CCW_FINAL 0x5
#define R_CCW_NEXT  0x6

/*
 * The below state table has, for each state (row), the new state
 * to set based on the next encoder output. From left to right in,
 * the table, the encoder outputs are 00, 01, 10, 11, and the value
 * in that position is the new state to set.
 */

const unsigned char ttable[7][4] = 
{
	// R_START
	{R_START,    R_CW_BEGIN,  R_CCW_BEGIN, R_START},
	// R_CW_FINAL
	{R_CW_NEXT,  R_START,     R_CW_FINAL,  R_START | DIR_CW},
	// R_CW_BEGIN
	{R_CW_NEXT,  R_CW_BEGIN,  R_START,     R_START},
	// R_CW_NEXT
	{R_CW_NEXT,  R_CW_BEGIN,  R_CW_FINAL,  R_START},
	// R_CCW_BEGIN
	{R_CCW_NEXT, R_START,     R_CCW_BEGIN, R_START},
	// R_CCW_FINAL
	{R_CCW_NEXT, R_CCW_FINAL, R_START,     R_START | DIR_CCW},
	// R_CCW_NEXT
	{R_CCW_NEXT, R_CCW_FINAL, R_CCW_BEGIN, R_START},
};

/**
	Constructor. Each argument is the pin number for each encoder contact.
 */
 
SwitchEncoder::SwitchEncoder(char CHA, char CHB, char SWI) 
{
	// Assign variables.
	_cha = CHA;
	_chb = CHB;
	_swi = SWI;

	// Initialise state.
	state = R_START;
  switchValue = 0;

  p_fctButtonPressed  = NULL;
  p_fctButtonReleased = NULL;
  p_fctClockwiseTurn = NULL;
  p_fctCounterClockwiseTurn = NULL;

  lastA = 0;
  lastB = 0;
}

void SwitchEncoder::init()
{
  // Set pins to input.
  pinMode(_cha, INPUT);
  pinMode(_chb, INPUT);
  pinMode(_swi, INPUT);

  switchValue = getValue();
}

boolean SwitchEncoder::getValue()
{
  return digitalRead(_swi);
}

/**
	Reads the shaft's rotation
	Returns the way it rotated (right/left <=> clockwise/counter-clockwise) and increments a counter
*/

void SwitchEncoder::check() 
{
  if((lastA != digitalRead(_cha))||(lastB != digitalRead(_chb)))
  {
	  // Grab state of input pins.
	  unsigned char pinstate = (digitalRead(_cha) << 1) | digitalRead(_chb);
	  // Determine new state from the pins and state table.
	  state = ttable[state & 0xf][pinstate];
	
	  // Increment/Decrement according to rotation sense and return corresponding value
	  if ((state & 0x30) == DIR_CW)
	  {
      if(p_fctCounterClockwiseTurn != NULL)
      {
        (*p_fctCounterClockwiseTurn)();
      }
	  }
	  else if ((state & 0x30) == DIR_CCW)
	  {
      if(p_fctClockwiseTurn != NULL)
      {
        (*p_fctClockwiseTurn)();
      }
	  }
    lastA = digitalRead(_cha);
    lastB = digitalRead(_chb);
  }

  if(switchValue != getValue())
  {
    if(getValue() == false)
    {
      if(p_fctButtonPressed != NULL)
      {
        (*p_fctButtonPressed)();
      }
    }
    else
    {
      if(p_fctButtonReleased != NULL)
      { 
        (*p_fctButtonReleased)();
      }
    }
    switchValue = getValue();
  }
}

  void SwitchEncoder::attachFunctionOnButtonPressedEdge(void (*p_func)())
  {
    p_fctButtonPressed = p_func;
  }
  
  void SwitchEncoder::attachFunctionOnButtonReleasedEdge(void (*p_func)())
  {
    p_fctButtonReleased = p_func;
  }
  
  void SwitchEncoder::attachFunctionOnClockwiseTurn(void (*p_func)())
  {
    p_fctClockwiseTurn = p_func;
  }

  void SwitchEncoder::attachFunctionOnCounterClockwiseTurn(void (*p_func)())
  {
    p_fctCounterClockwiseTurn = p_func;
  }
