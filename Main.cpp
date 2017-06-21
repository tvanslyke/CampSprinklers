/*
 * Main.cpp
 *
 *  Created on: Jun 19, 2017
 *      Author: tim
 */

#include "PeriodicEvent.h"
#include "GPIO.h"
#include "LCD.h"
#include <ctime>
#include <iostream>
#include <iomanip>


int main()
{
	auto event = [&]()
	{
		std::time_t t = std::time(nullptr);
		std::cout << std::put_time(std::localtime(&t), "%c %Z") << std::endl;
	};

	auto pe = make_periodic_event(event, std::chrono::seconds(1));
	pe.run_continuously();

	return 0;
}
