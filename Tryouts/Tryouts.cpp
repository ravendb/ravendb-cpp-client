
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

//	auto store = documents::DocumentStore::create();
//	store->set_urls({ "http://127.0.0.1:8080" });
	//store->set_before_perform(set_for_fiddler);
//	store->set_database("Test");
	//store->initialize();

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


    auto cert = impl::CertificateDetails{};
    cert.certificate = R"(-----BEGIN CERTIFICATE-----
MIIFBTCCAu2gAwIBAgIVAK/d72tgF9CskZhVnszKzRriEVc+MA0GCSqGSIb3DQEB
DQUAMCwxKjAoBgNVBAMMISouc2FzaGF0aW0xMjV0ZXN0LmRldmVsb3BtZW50LnJ1
bjAeFw0xOTA2MDUwMDAwMDBaFw0yNDA2MDUwMDAwMDBaMA8xDTALBgNVBAMMBFVz
ZXIwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIKAoICAQCrmxboAsvN24rk+yW/
LatlUHjpFQjPE9nbI0II3Z7/tVIn5336DC4PWBos7aa60wbBYLSlDriNT8pnLY20
mW7GMN5pHb/17/MKbx6ufpbAG2q0QIfotnRzIkR7zG93TkcQ773Qeb9XE2OFM7yR
uleW595ZtCouFw6A93el2lgQ0dpWXEoVU2xYveW5zEOIvlZcmbr3dnakynussjKZ
4hvAcsWAKiA/doJns5R16o+L7ZCdJmFxlr4nddraCGJkY+d5rRW7ngSpmu2IA1qJ
FJfsO3KjRywgHdv7RvLze4wOX+uvpF0Vn/OO6hIfl9KLbKZLJh/uPULmvDCklRdn
CNUCk04ciAdEqx2Yqs94RrVdX4XkqKzVWEJObT5Uw+xxXgUscwbLauSX00qDTXk7
q8HRpV9848KU4jovQqbDjYHlLcLb5S5w6fHV0K3eFTQVX5WNB2sn4CuZTHSv3HM4
zO7Iti/ks94QAyzk26STrwZBBarY0M/TLrTbwI1Y+7CbeGr3kFXqkHGlkjV4UKHO
bdejUCHcqPWxGl4GYF7SFet+kThgV+xH1wJDfhaDkeNmNtUT0uLBGvlcF4uGX/aF
jaQVk3r6kF/4ksj36qrdg5lrYKQsG484as/dRkISD3BDMnu6WlUfHGZpAAko+lKe
hHDBxh2WGyHyQ6Vu5oyHNePi1QIDAQABozswOTAfBgNVHSMEGDAWgBT18jzh6yYN
CbxewjrshXMsmKQ/FTAWBgNVHSUBAf8EDDAKBggrBgEFBQcDAjANBgkqhkiG9w0B
AQ0FAAOCAgEAW9YFOY+nemSGdhsi8cfh+gVtXKUxdQKWQRo1irj5DqQmN/2Q5qtm
YRmXsmLuL9K03ByyZLEEqhEfdZsLf2FanM6RCEu3TD1ygXeg0JTAGIEFy/Psd7E0
rarzxeH9rL8PEQwEOWQnJDkA4yuklZrFQ5nwJX4WKSZ0cdtrUu24QmDOOgfwe7zi
MAnsebsoPtxCSu36UAQcmmmYrFhUG4IrLJjwh8V17dw4WhPX/aO127I9VM82UrpN
WWAYzp5lkgvX+RVzyVUaDi36hATkCrzrq5ZsB3RS5YADthQOuBWMNQFg3BTMDaJD
e4Jv7LVydyhyLueKCXovuKLaDgWYOsqSlGaImphI5aK09mTlWHHS+/C7CtILj0+Q
DOt4OvKfB836Pm9MCymo9UuS8ea+upFvUlTW/RG+epJ5yrRTVaJDvqY2EUbfXunp
x/Oykors/0aG5p9ib5dF3HIMqXoVzRkrgMo+/nHj8zbpNeKk4n4rIsBotlHMnW0N
al1EfRyecrjpVHcZaROT/Kuf1veG+e2DSRuvM/MldnIaFwkBpZDTM/5YTDkdApRv
nuCzQGCBD7IGNHdGC/Zpw+DpAzqjGJZtengEV5yqpMaicbI0pydjuYXLTvWZa5R1
NupwWmsCSMwLVroWgpHnw+Y17B2NgMwrna/Jf9MqEdBLqXFt9Vl1Ggw=
-----END CERTIFICATE-----)";

    cert.key = R"(-----BEGIN RSA PRIVATE KEY-----
Proc-Type: 4,ENCRYPTED
DEK-Info: DES-EDE3-CBC,e6a99ed06880c32d

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
-----END RSA PRIVATE KEY-----)";

    cert.key_password = "User";

    auto store = documents::DocumentStore::create();
    store->set_urls({ "https://a.sashatim125test.development.run:8085" });
    store->set_certificate_details(cert);
    store->get_conventions()->set_disable_topology_updates(true);
    store->initialize();

    auto database_record = serverwide::DatabaseRecord();
    database_record.database_name = "TEST";

    store->maintenance()->server()->send(serverwide::operations::CreateDatabaseOperation(database_record));

}