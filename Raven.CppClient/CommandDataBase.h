#pragma once
#include "CommandType.h"

namespace ravendb::client::documents::session
{
	class InMemoryDocumentSessionOperations;
}

namespace ravendb::client::documents::commands::batches
{
	class CommandDataBase
	{
	private:
		std::string id{};
		std::string name{};
		std::string change_vector{};
		CommandType type = CommandType::UNSET;

	protected:
		CommandDataBase() = default;

		CommandDataBase(std::string id_, std::string name_, std::string change_vector_, CommandType type_)
			: id(std::move(id_))
			, name(std::move(name_))
			, change_vector(std::move(change_vector_))
			, type(type_)
		{}
	public:
		virtual ~CommandDataBase() = 0;

		const std::string& get_id() const
		{
			return id;
		}
		const std::string& get_name() const
		{
			return name;
		}
		const std::string& get_change_vector() const
		{
			return change_vector;
		}
		CommandType get_type() const
		{
			return type;
		}

		virtual nlohmann::json serialize() const = 0;

		virtual void on_before_save_changes(std::shared_ptr<session::InMemoryDocumentSessionOperations> session) = 0;
	};

	inline CommandDataBase::~CommandDataBase() = default;

	
}
