#pragma once
#include <stdio.h>
#include <time.h>
#include <ostream>
#include <iomanip>

namespace ravendb::client::impl
{
	class DateTimeOffset
	{
	private:
		std::tm date_time{ 0 };		//with seconds precision
		long nsec{ 0 };				//the actual precision is 100nanoseconds
		std::time_t offset{ 0 };	//in seconds, the actual precision is minutes

	public:
		using DateTimeOffsetTuple = std::tuple<std::tm, long, std::time_t>;

		DateTimeOffsetTuple get_date_time_offset() const
		{
			return { date_time, nsec, offset };
		}

		DateTimeOffset() = default;

		//WARNING : this ctor may not check for every possible argument errors
		//str should be in ISO8061 format : YYYY-MM-DDThh:mm:ss.sssssss(Z) or YYYY-MM-DDThh:mm:ss.sssssss±hh:mm
		explicit DateTimeOffset(const std::string& str)
		{
			constexpr size_t DATE_TIME_LENGTH_1 = 27;
			constexpr size_t DATE_TIME_LENGTH_2 = 28;
			constexpr size_t DATE_TIME_LENGTH_3 = 33;

			if (str.length() < DATE_TIME_LENGTH_1)
			{
				throw_invalid_format(str);
			}

			const char* input_str = str.c_str();

			//expecting ISO8061 format : YYYY-MM-DDThh:mm:ss.sssssss(Z) or YYYY-MM-DDThh:mm:ss.sssssss±hh:mm
			int res = sscanf_s(input_str, "%04d-%02d-%02dT%02d:%02d:%02d.%07d",
				&date_time.tm_year, &date_time.tm_mon, &date_time.tm_mday,
				&date_time.tm_hour, &date_time.tm_min, &date_time.tm_sec, &nsec);

			date_time.tm_mon -= 1;
			nsec *= 100;

			if (constexpr long MAX_NSEC = 999999999;
				res < 7 ||
				date_time.tm_mon < 0  || date_time.tm_mon > 11  ||
				date_time.tm_mday < 1 || date_time.tm_mday > 31 ||
				date_time.tm_hour < 0 || date_time.tm_hour > 23 ||
				date_time.tm_min < 0  || date_time.tm_min > 59  ||
				date_time.tm_sec < 0  || date_time.tm_sec > 59  ||
				nsec < 0 || nsec > MAX_NSEC )
			{
				throw_invalid_format(str);
			}

			date_time.tm_year -= 1900;
			if (-1 == mktime(&date_time))
			{
				throw_invalid_format(str);
			}

			input_str += DATE_TIME_LENGTH_1;
			int offset_sign = 0;

			switch (str.length())
			{
				case DATE_TIME_LENGTH_1:
					break;
				case DATE_TIME_LENGTH_2:
					if(*input_str != 'Z')
					{
						throw_invalid_format(str);
					}
					break;
				case DATE_TIME_LENGTH_3:
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
						// offset is in format +(-)hh:mm
						res = sscanf_s(input_str, "%02d:%02d", &hour, &min);
						if (
							res < 2 ||
							hour < 0 || hour > 23 ||
							min < 0 || min > 59)
						{
							throw_invalid_format(str);
						}
						//storing offset in seconds
						this->offset = offset_sign * (3600 * hour + 60 * min);
					}
				default:
					throw_invalid_format(str);
			}
		}

		//output in ISO8061 format : YYYY-MM-DDThh:mm:ss.sssssss(Z) or YYYY-MM-DDThh:mm:ss.sssssss±hh:mm
		std::string to_string(bool add_Z_if_zero_offset = false) const
		{
			std::ostringstream result{};

			result << std::setfill('0') <<
				std::setw(4) << date_time.tm_year + 1900 << "-" <<
				std::setw(2) << date_time.tm_mon + 1 << "-" <<
				std::setw(2) << date_time.tm_mday << "T" <<
				std::setw(2) << date_time.tm_hour << ":" <<
				std::setw(2) << date_time.tm_min << ":" <<
				std::setw(2) << date_time.tm_sec << "." <<
				std::setw(7) << nsec / 100;

			std::time_t offset_copy = this->offset;
			if (0 == offset_copy)
			{
				if (add_Z_if_zero_offset)
				{
					result << "Z";
				}
			}else
			{
				if(offset_copy < 0)
				{
					result << "-";
					offset_copy *= -1;
				}else
				{
					result << "+";
				}
				result << std::setfill('0') <<
					std::setw(2) << offset_copy / 3600 << ":" <<
					std::setw(2) << (offset_copy % 3600) / 60;
			}
			return result.str();
		}

		friend std::ostream& operator<<(std::ostream& os, const DateTimeOffset& dt)
		{
			return os << dt.to_string() ;
		}

	private:
		static void throw_invalid_format(const std::string& input_str)
		{
			throw std::invalid_argument("invalid time format : " + input_str);
		}
	};

	inline void from_json(const nlohmann::json& j, DateTimeOffset& dt)
	{
		dt = DateTimeOffset(j.get<std::string>());
	}

	inline void to_json(nlohmann::json& j, const DateTimeOffset& dt)
	{
		j = dt.to_string();
	}

}
