#pragma once
#include "IDocumentStore.h"
#include "DateTimeOffset.h"

namespace ravendb::client::documents::identity
{
	// Generate hilo numbers against a RavenDB document

	class HiLoIdGenerator
	{
	protected:
		struct RangeValue
		{
			const int64_t minimum;
			const int64_t maximum;
			std::atomic_int64_t current;

			RangeValue(int64_t min_param, int64_t max_param)
				: minimum(min_param)
				, maximum(max_param)
				, current(min_param - 1)
			{}
		};

	private:
		std::mutex _generator_lock{};

		const std::shared_ptr<IDocumentStore> _store;
		const std::string _tag;
		int64_t _last_batch_size{};
		impl::DateTimeOffset _last_range_date{};
		const std::string _db_name;
		const std::string _identity_parts_separator;

		//IMPORTANT : from https://en.cppreference.com/w/cpp/memory/shared_ptr
		//All member functions(including copy constructor and copy assignment) can be called by multiple threads
		//on different instances of shared_ptr without additional synchronization even (sic!)
		//if these instances are copies and share ownership of the same object.
		std::shared_ptr<RangeValue> _range;

		void get_next_range();

	protected:
		std::string prefix{};
		std::string server_tag{};

		virtual std::string get_document_id_from_id(int64_t next_id) const;

		std::shared_ptr<RangeValue> get_range() const;
		void set_range(RangeValue* range);

	public:
		virtual ~HiLoIdGenerator() = default;

		HiLoIdGenerator(std::string tag, std::shared_ptr<IDocumentStore> store, std::string db_name,
		                std::string identity_parts_separator);

		std::string generate_document_id();

		int64_t next_id();

		void return_unused_range() const;
	};
}
