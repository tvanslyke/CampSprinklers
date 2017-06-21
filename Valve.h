/*
 * Valve.h
 *
 *  Created on: Jun 19, 2017
 *      Author: tim
 */

#ifndef VALVE_H_
#define VALVE_H_

class Valve
{
public:

	Valve(size_t pin):
		valve_pin_{pin}
	{
		gpio[valve_pin_].set_OUTPUT();
		gpio[valve_pin_] = true;
	}
	~Valve()
	{
		gpio[valve_pin_].set_OUTPUT();
		gpio[valve_pin_] = false;
		gpio[valve_pin_].set_INPUT();
		gpio[valve_pin_].set_PUD_DOWN();
	}
private:
	static GPIO<> & gpio;
	const size_t valve_pin_;
};
GPIO<> Valve::gpio{GPIO<>::gpio()};

#endif /* VALVE_H_ */
