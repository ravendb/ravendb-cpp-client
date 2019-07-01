#pragma once

namespace ravendb::client::documents::commands::batches
{
	class PutAttachmentCommandHelper
	{
	public:
		PutAttachmentCommandHelper() = delete;
		~PutAttachmentCommandHelper() = delete;

		static inline void throw_stream_was_already_used()
		{
			throw std::runtime_error("It is forbidden to re-use the same InputStream for more than one attachment."
				" Use a unique InputStream per put attachment command.");
		}
	};
}
