/*
 * GPIO.h
 *
 *  Created on: Jun 19, 2017
 *      Author: tim
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "Pin.h"
#include <utility>
template <size_t N = 28>
class GPIO {
public:
	using pin_t = Pin;
	static constexpr const size_t pin_count{N};
	static GPIO & gpio()
	{
		static GPIO gpio_singleton{};
		return gpio_singleton;
	}

	const pin_t & operator[](size_t pin) const
	{
		return pins_[pin];
	}

	pin_t & operator[](size_t pin)
	{
		return pins_[pin];
	}

	const pin_t & get_pin(size_t pin) const
	{
		return pins_[pin];
	}

	pin_t & get_pin(size_t pin)
	{
		return pins_[pin];
	}

	GPIO(const GPIO &) = delete;

private:

	template <size_t ... I>
	static std::array<pin_t, pin_count> pins_init(std::index_sequence<I...>)
	{
		return {pin_t(I) ... };
	}

	GPIO():
		pins_{pins_init(std::make_index_sequence<pin_count>{})}
	{
		wiringPiSetup();
	}

	std::mutex gpio_mutex_;
	std::array<pin_t, pin_count> pins_;
};

#endif /* GPIO_H_ */
