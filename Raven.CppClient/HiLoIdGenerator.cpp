#include "stdafx.h"
#include "HiLoIdGenerator.h"
#include "NextHiLoCommand.h"
#include "RequestExecutor.h"
#include "HiLoReturnCommand.h"


namespace ravendb::client::documents::identity
{
	void HiLoIdGenerator::get_next_range()
	{
		auto hilo_command = commands::NextHiLoCommand(_tag, _last_batch_size, _last_range_date,
			_identity_parts_separator, _range->maximum);

		auto re = _store->get_request_executor(_db_name);
		re->execute(hilo_command);

		prefix = hilo_command.get_result()->prefix;
		server_tag = hilo_command.get_result()->server_tag;
		_last_range_date = hilo_command.get_result()->last_range_at;
		_last_batch_size = hilo_command.get_result()->last_size;
		_range.reset(new RangeValue(hilo_command.get_result()->low, hilo_command.get_result()->high));
	}

	std::string HiLoIdGenerator::get_document_id_from_id(int64_t next_id) const
	{
		std::ostringstream res{};
		res << prefix << next_id << '-' << server_tag;
		return res.str();
	}

	std::shared_ptr<HiLoIdGenerator::RangeValue> HiLoIdGenerator::get_range() const
	{
		return _range;
	}

	void HiLoIdGenerator::set_range(RangeValue* range)
	{
		_range.reset(range);
	}

	HiLoIdGenerator::HiLoIdGenerator(std::string tag, std::shared_ptr<IDocumentStore> store, std::string db_name,
		std::string identity_parts_separator)
		: _store(store)
		, _tag(std::move(tag))
		, _db_name(std::move(db_name))
	    , _identity_parts_separator(std::move(identity_parts_separator))
		, _range(std::make_shared<RangeValue>(1, 0))
	{}

	std::string HiLoIdGenerator::generate_document_id()
	{
		return get_document_id_from_id(next_id());
	}

	int64_t HiLoIdGenerator::next_id()
	{
		while (true)
		{
			std::shared_ptr<RangeValue> range = _range;

			auto id = range->current.fetch_add(1) + 1;
			if (id <= range->maximum)
			{
				return id;
			}

			{
				auto lock_guard = std::lock_guard(_generator_lock);

				id = _range->current.load();
				if (id <= _range->maximum)
				{
					return id;
				}

				get_next_range();
			}
		}
	}

	void HiLoIdGenerator::return_unused_range() const
	{
		auto return_command = commands::HiLoReturnCommand(_tag, _range->current, _range->maximum);

		auto re = _store->get_request_executor(_db_name);
		re->execute(return_command);
	}
}
