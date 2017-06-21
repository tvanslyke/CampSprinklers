/*
 * LCDArray.h
 *
 *  Created on: Jun 20, 2017
 *      Author: tim
 */

#ifndef LCDARRAY_H_
#define LCDARRAY_H_

#include "LCD.h"
template <size_t R, size_t C>
class LCD_Array
{
public:
	static constexpr const size_t row_count = R;
	static constexpr const size_t col_count = R;
	LCD_Array();

};

#endif /* LCDARRAY_H_ */
