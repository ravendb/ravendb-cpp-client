#include "stdafx.h"
#include "DocumentInfo.h"

namespace ravendb::client::documents::session
{
	DocumentInfo::DocumentInfo(nlohmann::json document_)
		: document(std::move(document_))
		, metadata([&]
	{
		auto it = document.find(constants::documents::metadata::KEY);
		if (it == document.end() || !it->is_object())
		{
			throw std::runtime_error("Document must have a metadata");
		}
		return *it;
	}())
		, id([&]
	{
		auto it = metadata.find(constants::documents::metadata::ID);
		if (it == metadata.end() || !it->is_string())
		{
			throw std::runtime_error("Document must have an id");
		}
		return it->get<std::string>();
	}())
		, change_vector([&]
	{
		auto it = metadata.find(constants::documents::metadata::CHANGE_VECTOR);
		if (it == metadata.end() || !it->is_string())
		{
			throw std::runtime_error("Document " + id + " must have a Change Vector");
		}
		return it->get<std::string>();
	}())
	{}
}
