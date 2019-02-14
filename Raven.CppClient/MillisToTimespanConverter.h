#pragma once
#include <chrono>

namespace ravendb::client::impl::utils
{
	// serialization in C# TimeSpan format : d.hh:mm:ss.sssssss or hh:mm:ss.sssssss
	class MillisToTimeSpanConverter
	{
	private:
		const std::string _timespan_as_string;
	public:
		MillisToTimeSpanConverter(const std::chrono::milliseconds& msec);

		operator const std::string&() const;

		operator const char*() const;
	};
}