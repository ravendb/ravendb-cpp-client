#pragma once

#include "ravendb.h"


namespace raven {
	template<typename TResult>
	class RavenCommand {
	public:
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

		inline void set_response_not_found(CURL* curl) { }

		inline Topology get_result() override {
			return _topology;
		}
	};

	class GetDocumentsCommand : RavenCommand<GetDocumentsResult> {
		std::optional<int> _start, _page_size;
		std::vector<std::string> _ids, _includes;
		std::string _exclude, _starts_after, _starts_with, _matches;
		bool _metadata_only;

		GetDocumentsResult _result;

	public:
		GetDocumentsCommand(int start, int page_size) :
			_start(start), _page_size(page_size) {
		}

		GetDocumentsCommand(std::string id) : GetDocumentsCommand({ id }, {}, false) {

		}

		GetDocumentsCommand(std::vector<std::string> ids, std::vector<std::string> includes, bool metadata_only) :
			_ids(ids), _includes(includes), _metadata_only(metadata_only) {
		}

		GetDocumentsCommand(
			std::string starts_with,
			std::string starts_after,
			std::string matches,
			std::string exclude,
			int start,
			int page_size,
			bool metadata_only) :
			_starts_with(starts_with), _starts_after(starts_after),
			_matches(matches), _exclude(exclude), _start(start), _page_size(page_size),
			_metadata_only(metadata_only)
		{

		}

		std::string create_request(ServerNode& node, CURL* curl) override;
		void set_response(CURL* curl, const nlohmann::json& response) override;

		inline void set_response_not_found(CURL* curl) { }

		inline GetDocumentsResult get_result() override {
			return _result;
		}
	};
}