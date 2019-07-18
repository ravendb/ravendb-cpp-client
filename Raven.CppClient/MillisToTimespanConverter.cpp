#include "stdafx.h"
#include "MillisToTimespanConverter.h"
#include <iomanip>

namespace ravendb::client::impl::utils
{
	MillisToTimeSpanConverter::MillisToTimeSpanConverter(const std::chrono::milliseconds & msec)
		: _timespan_as_string([&]()
	{
		using namespace std::chrono;

		std::ostringstream time_dur;
		auto h = duration_cast<hours>(msec).count();
		auto m = duration_cast<minutes>(msec%hours(1)).count();
		auto s = duration_cast<seconds>(msec%minutes(1)).count();
		auto ms = (msec%seconds(1)).count();

		//num of days and hours remained
		auto d = h / 24;
		h %= 24;

		if (d > 0)
		{
			time_dur << d << ".";
		}

		time_dur << std::setfill('0') <<
			std::setw(2) << h << ":" <<
			std::setw(2) << m << ":" <<
			std::setw(2) << s << "." <<
			std::setw(3) << ms <<
			std::setw(4) << 0;

		return time_dur.str();
	}())
	{}

	MillisToTimeSpanConverter::operator const char*() const
	{
		return _timespan_as_string.c_str();
	}

	std::string MillisToTimeSpanConverter::as_string(const std::chrono::milliseconds& msec)
	{
		return MillisToTimeSpanConverter(msec);
	}

	MillisToTimeSpanConverter::operator const std::string&() const
	{
		return _timespan_as_string;
	}
}
