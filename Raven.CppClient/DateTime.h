#pragma once
#include <stdio.h>
#include <time.h>
#include <ostream>
#include <iomanip>

namespace ravendb::client::impl
{
	struct DateTime
	{
		std::tm date_time{ 0 };
		long nsec{ 0 };
		std::time_t time_zone{ 0 };

		DateTime() = default;

		//WARNING : this ctor may not check for every possible argument errors
		explicit DateTime(const std::string& str)
		{
			constexpr size_t MIN_DATE_TIME_LENGTH = 27;

			if(str.length() < MIN_DATE_TIME_LENGTH)
			{
				throw std::invalid_argument("invalid time format");
			}

			int res = sscanf(str.data(), "%04d-%02d-%02dT%02d:%02d:%02d.%07d",
				&date_time.tm_year, &date_time.tm_mon, &date_time.tm_mday,
				&date_time.tm_hour, &date_time.tm_min, &date_time.tm_sec, &nsec);
			if(res < 7)
			{
				throw std::invalid_argument("invalid time format");
			}

			date_time.tm_mon -= 1;
			date_time.tm_year -= 1900;
			date_time.tm_isdst = -1;
			mktime(&date_time);
			nsec *= 100;

			const char* time_zone_str = str.data() + MIN_DATE_TIME_LENGTH;
			int time_zone_sign = 0;
			switch (*time_zone_str)
			{
				case '\0':
					break;
				case 'Z':
					if(*(time_zone_str+1) != '\0')
					{
						throw std::invalid_argument("invalid time format");
					}
					break;
				case '+':
					time_zone_sign = +1;
					break;
				case '-':
					time_zone_sign = -1;
					break;
				default:
					throw std::invalid_argument("invalid time format");
			}

			if(time_zone_sign != 0)
			{
				int hour = 0, min = 0;

				time_zone_str += 1;
				res = sscanf_s(time_zone_str,"%02d:%02d",&hour, &min);
				if (res < 2 || *(time_zone_str + 5) != '\0')
				{
					throw std::invalid_argument("invalid time format");
				}

				this->time_zone = time_zone_sign * (3600 * hour + 60 * min);
			}
		}

		std::string to_string() const
		{
			std::ostringstream result{};

			result << std::setfill('0') <<
				std::setw(4) << date_time.tm_year + 1900 << "-" <<
				std::setw(2) << date_time.tm_mon + 1 << "-" <<
				std::setw(2) << date_time.tm_mday << "T" <<
				std::setw(2) << date_time.tm_hour << ":" <<
				std::setw(2) << date_time.tm_min << ":" <<
				std::setw(2) << date_time.tm_sec << "." <<
				std::setw(7) << this->nsec / 100;

			std::time_t time_zone_copy = this->time_zone;
			if(time_zone_copy != 0)
			{
				if(time_zone_copy < 0)
				{
					result << "-";
					time_zone_copy *= -1;
				}else
				{
					result << "+";
				}
				result << std::setfill('0') <<
					std::setw(2) << time_zone_copy / 3600 << ":" <<
					std::setw(2) << (time_zone_copy % 3600) / 60;
			}
			return result.str();
		}


		friend std::ostream& operator<<(std::ostream& os, const DateTime& dt)
		{
			return os << dt.to_string() ;
		}
	};

	inline void from_json(const nlohmann::json& j, DateTime& dt)
	{
		dt = DateTime(j.get<std::string>());
	}

	inline void to_json(nlohmann::json& j, const DateTime& dt)
	{
		j = dt.to_string();
	}

}
