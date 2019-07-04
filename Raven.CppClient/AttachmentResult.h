#pragma once
#include "AttachmentDetails.h"
#include "CurlResponse.h"

namespace ravendb::client::documents::operations::attachments
{
	class AttachmentResult
	{
	private:
		const AttachmentDetails _details;
		std::stringstream _data_stream{};
	public:
		AttachmentResult(const impl::CurlResponse& response, AttachmentDetails details);

		std::istream& get_data();
		const AttachmentDetails& get_details() const;
	};
}
