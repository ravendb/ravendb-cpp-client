
#include "pch.h"
#include <curl/curl.h>
#include <thread>
#include <DatabaseRecord.h>
#include <CreateDatabaseOperation.h>
#include "DocumentStore.h"
#include "DocumentSession.h"
#include "User.h"
#include "GetDatabaseTopologyCommand.h"
#include "EntityIdHelperUtil.h"
#include "GetNextOperationIdCommand.h"
#include "TasksExecutor.h"
#include "GetDatabaseNamesOperation.h"
#include "RavenException.h"
#include "ConnectionDetailsHolder.h"
#include "MaintenanceOperationExecutor.h"
#include "PutAttachmentOperation.h"
#include "CompareStringsEqualIgnoreCase.h"
#include "HeadAttachmentCommand.h"
#include "DeleteAttachmentOperation.h"
#include "PutDocumentCommand.h"
#include "GetAttachmentOperation.h"

namespace
{
	//using fiddler + verbose
	void set_for_fiddler(CURL* curl)
	{
		curl_easy_setopt(curl, CURLOPT_PROXY, "127.0.0.1:8888");
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	}

	void set_for_verbose(CURL* curl)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	}
}

//static void work()
//{
//	static std::atomic_int32_t session_id{ 0 };
//
//	auto conventions = ravendb::client::documents::conventions::DocumentConventions::create();
//	conventions->set_read_balance_behavior(ravendb::client::http::ReadBalanceBehavior::ROUND_ROBIN);
//	conventions->freeze();
//	auto executor = ravendb::client::http::RequestExecutor::create({ "http://127.0.0.1:8080" }, "Test",
//		{}, std::make_shared<ravendb::client::impl::TasksScheduler>(std::make_shared<ravendb::client::impl::TasksExecutor>()),
//		conventions, set_for_fiddler);
//
//	auto session_info = ravendb::client::documents::session::SessionInfo{session_id++};
//
//	for (auto i = 0; i < 100; ++i)
//	{
//		auto cmd = ravendb::client::serverwide::operations::GetDatabaseNamesOperation(0, 20).get_command(conventions);
//		executor->execute(*cmd, session_info);
//		std::this_thread::sleep_for(std::chrono::seconds(10));
//	}
//}

static void thread_work(std::shared_ptr<ravendb::client::documents::DocumentStore> store, int i)
{
	auto cmd = ravendb::client::documents::commands::GetDocumentsCommand(i*10, 10);
	
	for (auto j = 0; j < 500; ++j)
	{
		store->get_request_executor()->execute(cmd);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

int main()
{
	using namespace ravendb::client;

	REGISTER_ID_PROPERTY_FOR(ravendb::client::tests::infrastructure::entities::User, id);

	auto store = documents::DocumentStore::create();
	store->set_urls({ "http://127.0.0.1:8080" });
	store->set_before_perform(set_for_fiddler);
	store->set_database("Test111");
	store->initialize();

	//auto conventions = documents::conventions::DocumentConventions::create();
	//conventions->set_read_balance_behavior(http::ReadBalanceBehavior::NONE);
	//conventions->freeze();
	//auto executor = http::RequestExecutor::create({ "http://127.0.0.1:8080" }, "Test",
	//	{}, std::make_shared<impl::TasksScheduler>(std::make_shared<impl::TasksExecutor>(4)), 
	//	conventions, set_for_verbose);

	//for (auto i = 0; i < 100; ++i)
	//{
	//	auto cmd = serverwide::operations::GetDatabaseNamesOperation(0, 20).get_command(conventions);
	//	executor->execute(*cmd);
	//	std::this_thread::sleep_for(std::chrono::seconds(10));
	//}

	//std::thread threads[3];
	//for(auto& thread : threads)
	//{
	//	thread = std::thread(work);
	//}
	//for (auto& thread : threads)
	//{
	//	thread.join();
	//}

//	store->get_conventions()->set_max_http_cache_size(1024*10);
//	store->initialize();
//
//    {
//        auto session = store->open_session();
//        auto user = std::make_shared<tests::infrastructure::entities::User>();
//        user->name = "Alexander";
//        session.store(user);
//        session.save_changes();
//    }

//	constexpr auto NUM_OF_THREADS = 10;
//	std::vector<std::thread> threads{};
//	threads.reserve(NUM_OF_THREADS);
//	for(auto i = 0; i< NUM_OF_THREADS; ++i)
//	{
//		threads.emplace_back(thread_work, store, i);
//	}
//
//	for(auto& thread : threads)
//	{
//		thread.join();
//	}





	auto file1 = std::make_shared<std::vector<std::byte>>(impl::utils::transform_string_to_bytes_array("Alexander"));

	std::stringstream str{};
	for (auto i = 0; i < 20000; ++i)
	{
		str << "ab Cd$" << 123;
	}

	nlohmann::json j;
	j["str"] = str.str();

	auto cmd = documents::commands::PutDocumentCommand("user2/1", {}, j);
	store->get_request_executor()->execute(cmd);


	auto cmd1 = documents::operations::attachments::PutAttachmentOperation("users/1", "file1.txt",
		str, "ASCII").get_command(
			store, store->get_conventions(), store->get_request_executor()->get_cache());

	store->get_request_executor()->execute(*cmd1);

	//auto command = documents::operations::attachments::PutAttachmentOperation("users/1", "file1.txt",
	//	file1, nullptr, "ASCII").get_command(
	//		store, store->get_conventions(), store->get_request_executor()->get_cache());

	//store->get_request_executor()->execute(*command);

	auto cmd2 = documents::commands::HeadAttachmentCommand("users/1", "file.txt");
	store->get_request_executor()->execute(cmd2);


	auto cmd3 = documents::operations::attachments::GetAttachmentOperation("users/1", "file1.txt",
		documents::attachments::AttachmentType::DOCUMENT).get_command(store,
			store->get_conventions(), store->get_request_executor()->get_cache());
	store->get_request_executor()->execute(*cmd3);

	std::ostringstream res{};

	res << cmd3->get_result()->get_data().rdbuf();

	std::cout << (res.str() == str.str() ? "----OK----" : "There is a problem ...") << std::endl;

	auto cmd4 = documents::operations::attachments::DeleteAttachmentOperation("users/1", "file1.txt")
		.get_command(store, store->get_conventions(), store->get_request_executor()->get_cache());
	store->get_request_executor()->execute(*cmd4);


	std::cout << "Bye" << std::endl;
}