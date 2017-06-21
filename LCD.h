/*
 * LCD.h
 *
 *  Created on: Jun 19, 2017
 *      Author: tim
 */

#ifndef LCD_H_
#define LCD_H_
#include <array>
#include "GPIO.h"
#include <chrono>
#include <thread>
/*
 * http://www.ekenrooi.net/lcd/lcd.shtml
 * http://www.ekenrooi.net/lcd/lcd.shtml
 * http://www.ekenrooi.net/lcd/lcd.shtml
 * http://www.ekenrooi.net/lcd/lcd.shtml
 */

template <size_t M, size_t N>
class LCD
{
public:
	static GPIO<> & gpio;
	static constexpr const size_t row_count{M};
	static constexpr const size_t col_count{N};
	using char_t = unsigned char;
	LCD();
private:
	void load_data(char_t bits)
	{
		gpio[data_pins_[0]] = (bits >> 0) & 0x01;
		gpio[data_pins_[1]] = (bits >> 1) & 0x01;
		gpio[data_pins_[2]] = (bits >> 2) & 0x01;
		gpio[data_pins_[3]] = (bits >> 3) & 0x01;
		gpio[data_pins_[4]] = (bits >> 4) & 0x01;
		gpio[data_pins_[5]] = (bits >> 5) & 0x01;
		gpio[data_pins_[6]] = (bits >> 6) & 0x01;
		gpio[data_pins_[7]] = (bits >> 7) & 0x01;
	}

	bool get_rw() const
	{
		return rw_;
	}
	bool is_write_mode()
	{
		return not get_rw();
	}
	bool is_read_mode()
	{
		return get_rw();
	}

	bool get_reg() const
	{
		return reg_;
	}

	bool get_enable() const
	{
		return enable_;
	}

	void set_rw(bool val)
	{
		rw_ = val;
		if(val)
		{
			gpio[data_pins_[0]].set_INPUT();
			gpio[data_pins_[1]].set_INPUT();
			gpio[data_pins_[2]].set_INPUT();
			gpio[data_pins_[3]].set_INPUT();
			gpio[data_pins_[4]].set_INPUT();
			gpio[data_pins_[5]].set_INPUT();
			gpio[data_pins_[6]].set_INPUT();
			gpio[data_pins_[7]].set_INPUT();
		}
		else
		{
			gpio[data_pins_[0]].set_OUTPUT();
			gpio[data_pins_[1]].set_OUTPUT();
			gpio[data_pins_[2]].set_OUTPUT();
			gpio[data_pins_[3]].set_OUTPUT();
			gpio[data_pins_[4]].set_OUTPUT();
			gpio[data_pins_[5]].set_OUTPUT();
			gpio[data_pins_[6]].set_OUTPUT();
			gpio[data_pins_[7]].set_OUTPUT();
		}
		gpio[rw_select_] = val;
	}

	void read_mode()
	{
		set_rw(true);
	}

	void write_mode()
	{
		set_rw(false);
	}


	void set_enable(bool value)
	{
		en_ = value;
		gpio[enable_] = value;
	}

	void set_register(bool value)
	{
		reg_ = value;
		gpio[reg_select_] = value;
	}

	void select_register(bool value)
	{
		set_register(value);
	}

	void instruction_mode()
	{
		set_register(false);
	}

	void data_mode()
	{
		set_register(true);
	}

	void disable()
	{
		set_enable(false);
	}

	void enable()
	{
		set_enable(true);
	}

	/*
	 * More complex operations.
	 */

	void exec()
	{
		enable();
		std::this_thread::sleep_for(std::chrono::nanoseconds(500));
		disable();
	}

	bool is_busy()
	{
		disable();
		read_mode();
		set_register(false);
		enable();
		return gpio[data_pins_[7]];
	}

	void block_while_busy()
	{
		while(is_busy())
		{
			asm("");
		}
	}

	void putc(char_t chr)
	{
		block_while_busy();
		data_mode();
		write_mode();
		load_data(chr);
		exec();
	}

	void puts(const std::string & msg)
	{
		auto iter = msg.begin();
		for(auto r = row_; r < row_count; ++r)
		{
			for(auto c = col_; c < col_count; ++c)
			{
				if(iter == msg.end())
				{
					return;
				}
				else
				{
					this->putc(*iter);
				}
			}
			next_row();
		}
	}

	void display_message(const std::string & msg)
	{
		home();
	}

	char_t getc()
	{
		block_while_busy();
		data_mode();
		read_mode();
		return get_data();
	}




	char_t get_data() const
	{
		char_t ret{0x00};
		ret |= (char_t(bool(gpio[data_pins_[0]])) << 0);
		ret |= (char_t(bool(gpio[data_pins_[1]])) << 1);
		ret |= (char_t(bool(gpio[data_pins_[2]])) << 2);
		ret |= (char_t(bool(gpio[data_pins_[3]])) << 3);
		ret |= (char_t(bool(gpio[data_pins_[4]])) << 4);
		ret |= (char_t(bool(gpio[data_pins_[5]])) << 5);
		ret |= (char_t(bool(gpio[data_pins_[6]])) << 6);
		ret |= (char_t(bool(gpio[data_pins_[7]])) << 7);
		return ret;
	}

	size_t get_row() const
	{
		return row_;
	}

	size_t get_col() const
	{
		return col_;
	}

	std::pair<size_t, size_t> get_position() const
	{
		return {get_row(), get_col()};
	}

	void to_row_begin(size_t row)
	{
		if(row >= row_count)
		{
			throw std::out_of_range("[LCD::set_row()] Attempt to set cursor to unreachable row.");
		}
		size_t col_cpy = col_;
		instruction(line_code((row_ = row)));
	}

	void set_row(size_t row)
	{
		to_row_begin(row);
		set_col(col_);
	}

	void set_col(size_t col)
	{
		if(col_ >= col_count)
		{
			throw std::out_of_range("[LCD::set_col()] Attempt to set cursor to unreachable column.");
		}
		while(col_ < col)
		{
			next_col();
		}
		while(col_ > col)
		{
			prev_col();
		}
	}

	void get_position(size_t row, size_t col) const
	{
		return {get_col(), get_row()};
	}

	/******************
	 *  Instructions  *
	 ******************
	 */
	void pre_instruction()
	{
		block_while_busy();
		write_mode();
		instruction_mode();
	}

	void instruction(char_t instr)
	{
		pre_instruction();
		load_data(instr);
		exec();
	}

	void clear_display()
	{
		instruction(0x01);
	}

	void home()
	{
		instruction(0x02);
		row_ = 0;
		col_ = 0;
	}
	void prev_col()
	{
		instruction(0x04);
		--col_;
	}
	void next_col()
	{
		instruction(0x06);
		++col_;
	}
	void display_shift_left()
	{
		instruction(0x05);
	}
	void display_shift_right()
	{
		instruction(0x07);
	}
	void cursor_on()
	{
		instruction(0x0E);
	}
	void display_off()
	{
		instruction(0x08);
	}
	void cursor_off()
	{
		instruction(0x0C);
	}



	void next_row()
	{
		instruction(line_code(++row_));
	}

	void prev_row()
	{
		instruction(line_code(--row_));
	}



	static char_t line_code(size_t line_number)
	{
		static constexpr const std::array<char_t, 4> line_map{0x80, 0xC0, 0x90, 0xD0};
		return line_map[line_number];
	}

	void show(const std::string & msg)
	{
		clear_display();
		home();
		puts(msg);
	}

	// DB0-DB7
	const std::array<size_t, 8> data_pins_;
	const size_t reg_select_;
	const size_t rw_select_;
	const size_t enable_;
	bool en_;
	bool rw_;
	bool reg_;
	size_t row_;
	size_t col_;
};

template <size_t M, size_t N>
GPIO<> & LCD<M, N>::gpio{GPIO<>::gpio()};
#endif /* LCD_H_ */
