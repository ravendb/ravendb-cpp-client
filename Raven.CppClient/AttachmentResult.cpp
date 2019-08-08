#include "stdafx.h"
#include "AttachmentResult.h"

namespace ravendb::client::documents::operations::attachments
{
	AttachmentResult::AttachmentResult(const impl::CurlResponse& response, AttachmentDetails details)
		: _details(std::move(details))
	{
		_data_stream << response.output.rdbuf();
	}

	std::istream& AttachmentResult::get_data()
	{
		return _data_stream;
	}

	const AttachmentDetails& AttachmentResult::get_details() const
	{
		return _details;
	}
}
