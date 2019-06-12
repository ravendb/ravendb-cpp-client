#include "stdafx.h"
#include "DateTimeOffset.h"

#include <iomanip>
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>

namespace ravendb::client::impl
{
	RawDateTimeOffset DateTimeOffset::get_date_time_offset() const
	{
		return { _date_time, _nsec, _offset };
	}

	DateTimeOffset::DateTimeOffset(const std::string & str)
	{
		constexpr size_t DATE_TIME_LENGTH_BASIC = 27;
		constexpr size_t DATE_TIME_LENGTH_UTC = 28;
		constexpr size_t DATE_TIME_LENGTH_WITH_OFFSET = 33;

		const auto length = str.length();

		if (length != DATE_TIME_LENGTH_BASIC &&
			length != DATE_TIME_LENGTH_UTC &&
			length != DATE_TIME_LENGTH_WITH_OFFSET)
		{
			throw_invalid_format(str);
		}

		const char* input_str = str.c_str();

		//expecting ISO8061 format : YYYY-MM-DDThh:mm:ss.sssssss(Z) or YYYY-MM-DDThh:mm:ss.sssssss�hh:mm
		int res =
#ifdef __STDC_LIB_EXT1__
			std::sscanf_s
#else
#ifdef _MSC_VER 
			sscanf_s
#else
            std::sscanf
#endif
#endif
		                (input_str, "%04d-%02d-%02dT%02d:%02d:%02d.%07ld",
			&_date_time.tm_year, &_date_time.tm_mon, &_date_time.tm_mday,
			&_date_time.tm_hour, &_date_time.tm_min, &_date_time.tm_sec, &_nsec);

		_nsec *= 100;
		{
			constexpr decltype(_nsec) MAX_NSEC = 999'999'999l;
			constexpr decltype(_date_time.tm_year) MAX_YEAR = 9'999;//valid max 4 digits year
			if (res < 7 ||
				_date_time.tm_year < 1 || _date_time.tm_year > MAX_YEAR ||
				_date_time.tm_mon < 1 || _date_time.tm_mon > 12 ||
				_date_time.tm_mday < 1 || _date_time.tm_mday > 31 ||
				_date_time.tm_hour < 0 || _date_time.tm_hour > 23 ||
				_date_time.tm_min < 0 || _date_time.tm_min > 59 ||
				_date_time.tm_sec < 0 || _date_time.tm_sec > 59 ||
				_nsec < 0 || _nsec > MAX_NSEC)
			{
				throw_invalid_format(str);
			}
		}

		_date_time.tm_year -= 1900;
		_date_time.tm_mon -= 1;

		input_str += DATE_TIME_LENGTH_BASIC;
		int offset_sign = 0;

		switch (length)
		{
		case DATE_TIME_LENGTH_BASIC:
			break;
		case DATE_TIME_LENGTH_UTC:
			if (*input_str != 'Z')
			{
				throw_invalid_format(str);
			}
			break;
		case DATE_TIME_LENGTH_WITH_OFFSET:
			switch (*input_str)
			{
			case '+':
				offset_sign = +1;
				break;
			case '-':
				offset_sign = -1;
				break;
			default:
				throw_invalid_format(str);
			}
			{
				input_str += 1;
				int hour = 0, min = 0;
				// _offset is in format +(-)hh:mm
				res =
#ifdef __STDC_LIB_EXT1__
					std::sscanf_s
#else
#ifdef _MSC_VER 
					sscanf_s
#else
					std::sscanf
#endif
#endif
                    (input_str, "%02d:%02d", &hour, &min);
				if (
					res < 2 ||
					hour < 0 || hour > 12 ||
					min < 0 || min > 59)
				{
					throw_invalid_format(str);
				}
				//storing _offset in seconds
				this->_offset = offset_sign * (3600 * hour + 60 * min);
			}
		default:
			throw_invalid_format(str);
		}
	}

	std::string DateTimeOffset::to_string(bool add_Z_if_zero_offset) const
	{
		std::ostringstream result{};

		result << std::setfill('0') <<
			std::setw(4) << _date_time.tm_year + 1900 << "-" <<
			std::setw(2) << _date_time.tm_mon + 1 << "-" <<
			std::setw(2) << _date_time.tm_mday << "T" <<
			std::setw(2) << _date_time.tm_hour << ":" <<
			std::setw(2) << _date_time.tm_min << ":" <<
			std::setw(2) << _date_time.tm_sec << "." <<
			std::setw(7) << _nsec / 100;

		std::time_t offset_copy = this->_offset;
		if (0 == offset_copy)
		{
			if (add_Z_if_zero_offset)
			{
				result << "Z";
			}
		}
		else
		{
			if (offset_copy < 0)
			{
				result << "-";
				offset_copy *= -1;
			}
			else
			{
				result << "+";
			}
			result << std::setfill('0') <<
				std::setw(2) << offset_copy / 3600 << ":" <<
				std::setw(2) << (offset_copy % 3600) / 60;
		}
		return result.str();
	}

	void DateTimeOffset::throw_invalid_format(const std::string & input_str)
	{
		throw std::invalid_argument("invalid time format : " + input_str);
	}

	bool operator<(const DateTimeOffset& lhs, const DateTimeOffset& rhs)
	{
		if(lhs._date_time.tm_year > rhs._date_time.tm_year)
		{
			return false;
		}
		if(lhs._date_time.tm_year < rhs._date_time.tm_year)
		{
			return true;
		}
		//same year
		if (lhs._date_time.tm_mon > rhs._date_time.tm_mon)
		{
			return false;
		}
		if (lhs._date_time.tm_mon < rhs._date_time.tm_mon)
		{
			return true;
		}
		//same month
		if (lhs._date_time.tm_mday > rhs._date_time.tm_mday)
		{
			return false;
		}
		if (lhs._date_time.tm_mday < rhs._date_time.tm_mday)
		{
			return true;
		}
		//same day
		if (lhs._date_time.tm_hour > rhs._date_time.tm_hour)
		{
			return false;
		}
		if (lhs._date_time.tm_hour < rhs._date_time.tm_hour)
		{
			return true;
		}
		//same hour
		if (lhs._date_time.tm_min > rhs._date_time.tm_min)
		{
			return false;
		}
		if (lhs._date_time.tm_min < rhs._date_time.tm_min)
		{
			return true;
		}
		//same minute
		if (lhs._date_time.tm_sec > rhs._date_time.tm_sec)
		{
			return false;
		}
		if (lhs._date_time.tm_sec < rhs._date_time.tm_sec)
		{
			return true;
		}
		//same second
		return lhs._nsec < rhs._nsec;
	}

	bool operator<=(const DateTimeOffset& lhs, const DateTimeOffset& rhs)
	{
		return !(rhs < lhs);
	}

	bool operator>(const DateTimeOffset& lhs, const DateTimeOffset& rhs)
	{
		return rhs < lhs;
	}

	bool operator>=(const DateTimeOffset& lhs, const DateTimeOffset& rhs)
	{
		return !(lhs < rhs);
	}

	std::ostream & operator<<(std::ostream & os, const DateTimeOffset & dt)
	{
		return os << dt.to_string();
	}

	bool operator==(const DateTimeOffset& lhs, const DateTimeOffset& rhs)
	{
		return lhs._date_time == rhs._date_time
			&& lhs._nsec == rhs._nsec
			&& lhs._offset == rhs._offset;
	}

	bool operator!=(const DateTimeOffset& lhs, const DateTimeOffset& rhs)
	{
		return !(lhs == rhs);
	}

	void from_json(const nlohmann::json & j, DateTimeOffset & dt)
	{
		dt = DateTimeOffset(j.get<std::string>());
	}

	void to_json(nlohmann::json & j, const DateTimeOffset & dt)
	{
		j = dt.to_string();
	}
}
