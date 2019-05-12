#pragma once
#include <ctime>
#include <ostream>
#include "json.hpp"
#include "utils.h"

namespace ravendb::client::impl
{
	using RawDateTimeOffset = std::tuple<std::tm, long, std::time_t>;

	class DateTimeOffset
	{
	private:
		//WARNING tm_wday and tm_yday fields are NOT filled in the deserialization ctor

		std::tm _date_time{ 0,0,0,1,0,-1899,0,0,0 };//with seconds precision
		long _nsec{ 0 };          //the actual precision is 100nanoseconds
		std::time_t _offset{ 0 }; //in seconds, the actual precision is minutes

		static void throw_invalid_format(const std::string& input_str);

	public:
		RawDateTimeOffset get_date_time_offset() const;

		DateTimeOffset() = default;

		//WARNING : this ctor may not check for every possible argument errors
		//WARNING tm_wday and tm_yday fields are NOT filled in the deserialization ctor
		//str should be in ISO8061 format : YYYY-MM-DDThh:mm:ss.sssssss(Z) or YYYY-MM-DDThh:mm:ss.sssssss±hh:mm
		explicit DateTimeOffset(const std::string& str);

		//output in ISO8061 format : YYYY-MM-DDThh:mm:ss.sssssss(Z) or YYYY-MM-DDThh:mm:ss.sssssss±hh:mm
		std::string to_string(bool add_Z_if_zero_offset = false) const;

		friend std::ostream& operator<<(std::ostream& os, const DateTimeOffset& dt);

		friend bool operator<(const DateTimeOffset& lhs, const DateTimeOffset& rhs);

		friend bool operator<=(const DateTimeOffset& lhs, const DateTimeOffset& rhs);

		friend bool operator>(const DateTimeOffset& lhs, const DateTimeOffset& rhs);

		friend bool operator>=(const DateTimeOffset& lhs, const DateTimeOffset& rhs);

		friend bool operator==(const DateTimeOffset& lhs, const DateTimeOffset& rhs);

		friend bool operator!=(const DateTimeOffset& lhs, const DateTimeOffset& rhs);
	};

	void from_json(const nlohmann::json& j, DateTimeOffset& dt);

	void to_json(nlohmann::json& j, const DateTimeOffset& dt);

}
