#pragma once

#include "ravendb.h"


namespace raven {

	template<typename TResult>
	class RavenCommand {
	
	protected:
		TResult _result;
		
	public:
		virtual ~RavenCommand() = 0 {};

		virtual std::string create_request(ServerNode& node, CURL* curl) = 0;
		virtual void set_response(CURL* curl, const nlohmann::json& response) = 0;
		virtual void set_response_not_found(CURL* curl) = 0;

		virtual TResult get_result() = 0;
	};

	class GetDatabaseTopologyCommand : RavenCommand<Topology> {

		Topology _topology;

	public:
		std::string create_request(ServerNode& node, CURL* curl) override;
		void set_response(CURL* curl, const nlohmann::json& response) override;

		inline void set_response_not_found(CURL* curl) override { }

		inline Topology get_result() override
		{
			return _topology;
		}
	};

	class GetDocumentsCommand : public RavenCommand<GetDocumentsResult> {
		
		//std::string _id;

		std::vector<std::string> _ids;
		std::vector<std::string> _includes;

		bool _metadataOnly = false;

		std::string _startWith;
		std::string _matches;
		std::optional<int32_t> _start;
		std::optional<int32_t> _pageSize;
		std::string _exclude;
		std::string _startAfter;
		
/*		std::optional<int> _start, _page_size;
		const std::vector<std::string> _ids, _includes;
		std::string _exclude, _starts_after, _starts_with, _matches;
		bool _metadata_only;

		GetDocumentsResult _result;
*/
	public:
		GetDocumentsCommand(int start, int pageSize)
		: _start(start)
		, _pageSize(pageSize)
		{}

		explicit GetDocumentsCommand(std::string id) 
		: GetDocumentsCommand(std::vector<std::string>{ std::move(id) })
		{}

		GetDocumentsCommand(std::vector<std::string> ids, std::vector<std::string> includes = {}, bool _metadataOnly = false)
		: _ids()
		, _includes()
		, _metadataOnly(_metadataOnly)
		{
			if(ids.empty())
			{
				throw std::invalid_argument("Please supply at least one id");
			}
			_ids = std::move(ids);
			_includes = std::move(includes);
		}

		GetDocumentsCommand
			( std::string startWith
			, std::string startAfter
			, std::string matches
			, std::string exclude
			, int start
			, int pageSize
			, bool metadataOnly)
			: _startWith(startWith)
			, _startAfter(startAfter)
			, _matches(matches)
			, _exclude(exclude)
			, _start(start)
			, _pageSize(pageSize)
			,_metadataOnly(metadataOnly)
		{}

		std::string create_request(ServerNode& node, CURL* curl) override;

		void set_response(CURL* curl, const nlohmann::json& response) override;

		inline void set_response_not_found(CURL* curl) override { }

		inline GetDocumentsResult get_result() override {
			return _result;
		}
	};
}