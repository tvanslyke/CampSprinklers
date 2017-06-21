/*
 * utilities.h
 *
 *  Created on: Jun 19, 2017
 *      Author: tim
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

template <class T>
class NextIntegral
{
public:
	using type = 	typename std::conditional<std::is_same<T, unsigned char>::value, unsigned short,
						typename std::conditional<std::is_same<T, unsigned short>::value, unsigned int,
							typename std::conditional<std::is_same<T, unsigned int>::value, unsigned long,
								typename std::conditional<std::is_same<T, unsigned long>::value, unsigned long long,
									void>::type
								>::type
							>::type
						>::type;
};
template <size_t N, class T = unsigned char>
class uint_least_has_max
{
private:
	size_t maxv =  std::numeric_limits<T>::max();
public:
	std::conditional<
};

constexpr size_t type_select(size_t maxv)
{
	if constexpr(maxv > std::numeric_limits<unsigned char>::max())
	{
		if constexpr(maxv > std::numeric_limits<unsigned short>::max())
		{
			if constexpr(maxv > std::numeric_limits<unsigned int>::max())
			{
				if constexpr(maxv > std::numeric_limits<unsigned long>::max())
				{
					if constexpr(maxv > std::numeric_limits<unsigned long long>::max())
					{

					}
					else
					{

					}
				}
				else
				{

				}
			}
			else
			{

			}
		}
		else
		{

		}
	}
	else
	{

	}
}


#endif /* UTILITIES_H_ */
