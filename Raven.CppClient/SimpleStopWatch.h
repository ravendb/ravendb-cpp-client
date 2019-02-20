#pragma once

namespace ravendb::client::impl
{
	class SimpleStopWatch
	{
	private:
		std::chrono::steady_clock::time_point _start{};

	public:
		SimpleStopWatch();

		void restart();

		std::chrono::milliseconds millis_elapsed() const;
	};

	inline SimpleStopWatch::SimpleStopWatch()
		: _start(std::chrono::steady_clock::now())
	{}

	inline void SimpleStopWatch::restart()
	{
		_start = std::chrono::steady_clock::now();
	}

	inline std::chrono::milliseconds SimpleStopWatch::millis_elapsed() const
	{
		auto now = std::chrono::steady_clock::now();

		return std::chrono::duration_cast<std::chrono::milliseconds>(now - _start);
	}
}
