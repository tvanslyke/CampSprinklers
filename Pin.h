/*
 * Pin.h
 *
 *  Created on: Jun 19, 2017
 *      Author: tim
 */

#ifndef PIN_H_
#define PIN_H_

#include <mutex>
#include <string>
#include <stdexcept>
#include "/home/tim/WiringPi/wiringPi/wiringPi/wiringPi.h"


class Pin
{

public:
	using bitfield_t = unsigned short;
	using mutex_t = std::mutex;
	using lock_t = std::lock_guard<mutex_t>;
	using pin_t = unsigned char;
	using mode_t = unsigned short;


	static std::string mode_name(short mode)
	{
		switch(mode)
		{
		case INPUT:
			return "INPUT";
			break;
		case OUTPUT:
			return "OUTPUT";
			break;
		case PWM_OUTPUT:
			return "PWM_OUTPUT";
			break;
		case GPIO_CLOCK:
			return "GPIO_CLOCK";
			break;
		default:
			return "UNKOWN";
			break;
		}
	}

	Pin(pin_t pin):
		pin_{pin}
	{
		set_PUD_DOWN();
		set_INPUT();
	}

	void validate_mode(mode_t needed_mode) const
	{
		if(not (get_mode() == needed_mode))
		{
			auto needed_mode_name = mode_name(needed_mode);
			auto current_mode_name = mode_name(get_mode());
			throw std::runtime_error(("Attempted operation on pin " + std::to_string(pin_) + " that requires " + needed_mode_name + " mode "
					"but the current mode is " + current_mode_name + " mode.").c_str());
		}
	}


	pin_t number() const
	{
		return pin_;
	}

	mode_t get_mode() const
	{
		return get_bits(14, 0b1100000000000000);
	}
	mode_t get_pud() const
	{
		return get_bits(12, 0b0011000000000000);
	}
	void set_PUD_UP()
	{
		pullUpDnControl(pin_, PUD_UP);
		set_bits(PUD_UP, 12, 0b0011000000000000);
	}
	void set_PUD_DOWN()
	{
		pullUpDnControl(pin_, PUD_DOWN);
		set_bits(PUD_DOWN, 12, 0b0011000000000000);
	}
	void set_PUD_OFF()
	{
		pullUpDnControl(pin_, PUD_OFF);
		set_bits(PUD_OFF, 12, 0b0011000000000000);
	}
	void set_INPUT()
	{
		pinMode(pin_, INPUT);
		set_bits(INPUT, 14, 0b1100000000000000);
	}
	void set_OUTPUT()
	{
		pinMode(pin_, OUTPUT);
		set_bits(OUTPUT, 14, 0b1100000000000000);
	}
	void set_PWM_OUTPUT()
	{
		pinMode(pin_, PWM_OUTPUT);
		set_bits(PWM_OUTPUT, 14, 0b1100000000000000);
	}
	void set_GPIO_CLOCK()
	{
		pinMode(pin_, GPIO_CLOCK);
		set_bits(GPIO_CLOCK, 14, 0b1100000000000000);
	}

	void write(unsigned short value)
	{
		lock_t lock(pin_mutex_);
		if(get_mode() == OUTPUT)
		{
			set_bits(value, 0, 0b0000111111111111);
			digitalWrite(pin_, value);
		}
		else if(get_mode() == PWM_OUTPUT)
		{
			set_bits(value, 0, 0b0000111111111111);
			pwmWrite(pin_, value);
		}
		else
		{
			validate_mode(OUTPUT);
		}
	}

	unsigned short read() const
	{
		lock_t lock(pin_mutex_);
		switch(get_mode())
		{
		case INPUT:
			return digitalRead(pin_);
			break;
		case OUTPUT:
		case PWM_OUTPUT:
			return get_bits(0, 0b0000111111111111);
			break;
		default:
			return -1;
			break;
		}
	}

	Pin & operator=(unsigned short value)
	{
		write(value);
		return *this;
	}
	operator int() const
	{
		return read();
	}
private:
	mutable mutex_t pin_mutex_;
	const pin_t pin_;
	bitfield_t bitfield_;

	void set_bits(unsigned short value, size_t shift, unsigned short mask)
	{
		lock_t lock(pin_mutex_);
		bitfield_ &= (~mask);
		bitfield_ |= (mask & value);
	}
	unsigned short get_bits(size_t shift, unsigned short mask) const
	{
		lock_t lock(pin_mutex_);
		return ((bitfield_ & mask) >> shift) ;
	}
};

#endif /* PIN_H_ */
