#pragma once
#include <exception>

namespace ravendb::client::exceptions
{
	class RavenException : public std::runtime_error
	{
	private:
		bool reached_leader{};

	public:
		~RavenException() = default;

		RavenException();

		explicit RavenException(const std::string& message);

		bool is_reached_leader() const;

		void set_reached_leader(bool reached_leader);

		static RavenException generic(const std::string& error, const std::string& json);
	};
}
