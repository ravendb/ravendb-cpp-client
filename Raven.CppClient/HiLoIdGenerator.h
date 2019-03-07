#pragma once

namespace ravendb::client::documents::identity
{
	class HiLoIdGenerator
	{
	private:
		std::mutex _generator_lock{};


	};
}
