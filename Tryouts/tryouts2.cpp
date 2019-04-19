//
#include "pch.h"

#include <iostream>

#include "User.h"

#include "GetDocumentsCommand.h"
#include "PutDocumentCommand.h"
#include "RequestExecutor.h"
#include "GetDatabaseRecordOperation.h"
#include "CreateDatabaseOperation.h"

#include "GetDatabaseNamesOperation.h"
#include "DeleteDatabasesOperation.h"
#include "DeleteDocumentCommand.h"
#include "CreateSampleDataOperation.h"
#include "PutIndexesOperation.h"
#include "GetIndexOperation.h"
#include "GetIndexesOperation.h"
#include "QueryCommand.h"
#include "DateTimeOffset.h"
#include "IndexHasChangedOperation.h"
#include "DeleteIndexOperation.h"
#include "DisableIndexOperation.h"
#include "EnableIndexOperation.h"
#include "ResetIndexOperation.h"
#include "StartIndexOperation.h"
#include "StopIndexOperation.h"
#include "StopIndexingOperation.h"
#include "StartIndexingOperationh.h"
#include "GetIndexNamesOperation.h"
#include "GetIndexErrorsOperation.h"
#include "SetIndexesLockOperation.h"
#include "GetIndexesStatisticsOperation.h"
#include "GetIndexingStatusOperation.h"
#include "GetTermsOperation.h"
#include "SetIndexesPriorityOperation.h"
#include "CertificateDetails.h"
#include <fstream>
#include "DeleteByQueryOperation.h"
#include "PatchOperation.h"
#include "PatchByQueryOperation.h"
#include "GetOperationStateOperation.h"
#include "DocumentSession.h"
#include "MetadataAsDictionary.h"
#include "DocumentStore.h"
#include "SessionOptions.h"
#include "Employee.h"
#include "LazySessionOperations.h"
#include "AdvancedSessionOperations.h"

namespace
{
	std::shared_ptr<ravendb::client::impl::CertificateDetails> get_cert()
	{
		static std::shared_ptr<ravendb::client::impl::CertificateDetails> cert_det{};

		if (!cert_det)
		{
			cert_det = std::make_shared<ravendb::client::impl::CertificateDetails>();
			//std::ifstream cert_file(R"(C:\work\PowerUser\PowerUser.crt)");
			//std::ifstream key_file(R"(C:\work\PowerUser\PowerUser.key)");
			std::ifstream cert_file(R"(c:\work\alexander-pc\alexander-pc.crt)");
			std::ifstream key_file(R"(C:\work\alexander-pc\alexander-pc.key)");
			{
				std::stringstream stream;
				stream << cert_file.rdbuf();
				cert_det->certificate = stream.str();
			}
			{
				std::stringstream stream;
				key_file >> stream.rdbuf();
				cert_det->key = stream.str();
			}
			cert_det->ca_path = "../ca-bundle.crt";
			//cert_det->key_password = "PowerUser";
		}
		return cert_det;
	}

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

	class FakeStore : public IDocumentStore
	{};

	struct MyQueryResult
	{
		std::string full_name{};
		std::string name{};
	};

	inline void to_json(nlohmann::json& j, const MyQueryResult& qr)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "FullName", qr.full_name);
		set_val_to_json(j, "JustName", qr.name);
	}

	inline void from_json(const nlohmann::json& j, MyQueryResult& qr)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "FullName", qr.full_name);
		get_val_from_json(j, "JustName", qr.name);
	}
}

int main()
{
	using namespace ravendb::client;

	//User user{ "Alexander","Timoshenko" };
	//nlohmann::json j = user;
	//std::string id{ "newID123" };
	//auto _executor = ravendb::client::http::RequestExecutor::create({ "http://127.0.0.1:8080" }, "TEST__DocumentCommandsTests", {},
	//	{ debug_curl_init, nullptr });
	//{
	//	documents::commands::PutDocumentCommand cmd(id, {}, j);
	//	_executor->execute(cmd);
	//}
	//{
	//	documents::commands::DeleteDocumentCommand cmd(id,"dssdf");
	//	_executor->execute(cmd);
	//}

	//auto exec = http::RequestExecutor::create({ "http://127.0.0.1:8080" }, {}, {}, set_for_fiddler);
	//serverwide::DatabaseRecord rec;
	//rec.database_name = "Test";
	//rec.topology.members.push_back("A");
	//serverwide::operations::CreateDatabaseCommand cmd(rec,1);
	//auto res1 = exec->execute(cmd);

	//for (auto& it : res1.topology.promotables_status)
	//{
	//	std::cout << static_cast<uint8_t>(it.second)<<" ";
	//}

	//auto test_suite_executor = http::RequestExecutor::create({ "http://127.0.0.1:8080" }, "Test", {}, set_for_fiddler);
	//tests::infrastructure::CreateSampleDataCommand cmd2;
	//test_suite_executor->execute(cmd2);


	//auto test_suite_executor = http::RequestExecutor::create({ "http://127.0.0.1:8080" }, "Test", {}, set_for_fiddler);
	//std::string id1(500,'a'), id2 = id1+"aaa", id3 = id2+"AAA";

	//documents::commands::GetDocumentsCommand cmd({id1,id2,id3}, {}, true);
	//test_suite_executor->execute(cmd);

	//serverwide::operations::GetDatabaseRecordCommand cmd2("test");

	//auto res2 = exec->execute(cmd2);

	//auto test_suite_executor = http::RequestExecutor::create({ "http://127.0.0.1:8080" }, "Northwind", {}, set_for_fiddler);

	//auto test_suite_executor = http::RequestExecutor::create({ "http://127.0.0.1:8082" }, "Northwind", {}, set_for_fiddler);

	//auto test_suite_executor = http::RequestExecutor::create
	//({ "https://a.sashatim125.development.run:8085" }, "Northwind", *get_cert(), set_for_verbose);


	//auto test_suite_executor = http::RequestExecutor::create
	//({ "https://alexander-pc:8086" }, "Northwind", *get_cert(), set_for_verbose);

	//documents::commands::GetDocumentsCommand cmd(
	//	std::vector<std::string>{ "employees/1-A", "orders/830-A" }, { "Freight","ReportsTo"}, false);
	//documents::commands::GetDocumentsCommand cmd(0, 20);
	//std::vector<std::string> ids;
	//for(auto i=1;i<=100;++i)
	//{
	//	ids.push_back("employees/" + std::to_string(i) + "-A");
	//}
	//documents::commands::GetDocumentsCommand cmd(ids, {},false);
	//documents::commands::GetDocumentsCommand cmd("employees/", "", "1-A*", "", 0, 16, false);
	//documents::commands::GetDocumentsCommand cmd(
	//		std::vector<std::string>{ "employees/1-A", "employees/3-A", "employees/3-A", "orders/830-A"}, {"Employee","ReportsTo"}, false);
	//test_suite_executor->execute(cmd);

	//documents::indexes::IndexDefinition index{};
	//index.name = "MyIndex";
	//index.maps = 
	//{ 
	//	R"(
	//	from order in docs.Orders 
	//		select new 
	//		{ 
	//			order.Employee, 
	//			order.Company
	//		})"
	//};
	//auto cmd = documents::operations::indexes::PutIndexesOperation({index}).get_command({});
	//auto res = test_suite_executor->execute(*cmd);
	//std::cout << res[0].index_name << std::endl;

	//std::getchar();

	//auto cmd2 = documents::operations::indexes::GetIndexOperation("Orders/ByCompany").get_command({});
	//auto res2 = test_suite_executor->execute(*cmd2);
	//std::cout << res2.name << std::endl;

	//std::getchar();

	//auto cmd3 = documents::operations::indexes::GetIndexesOperation(0,100).get_command({});
	//auto res3 = test_suite_executor->execute(*cmd3);
	//std::cout << res3[1].name << std::endl;

	//std::getchar();

	//auto cmd4 = documents::operations::indexes::IndexHasChangedOperation(index).get_command({});
	//auto res4 = test_suite_executor->execute(*cmd4);
	//std::cout << std::boolalpha << res4 << std::endl;

	//std::getchar();

	//auto cmd5 = documents::operations::indexes::DeleteIndexOperation("MyIndex").get_command({});
	//auto res5 = test_suite_executor->execute(*cmd5);

	//serverwide::operations::DeleteDatabasesOperation op6("something", true, {}, std::chrono::hours(30));
	//auto cmd6 = op6.get_command({});
	//auto res6 = test_suite_executor->execute(*cmd6);

	//auto cmd7 = documents::operations::indexes::EnableIndexOperation("People/Search").get_command({});
	//auto res4 = test_suite_executor->execute(*cmd7);

	//auto cmd8 = documents::operations::indexes::ResetIndexOperation("People/Search").get_command({});
	//auto res8 = test_suite_executor->execute(*cmd8);

	//auto cmd9 = documents::operations::indexes::StopIndexOperation("MyIndex").get_command({});
	//auto res9 = test_suite_executor->execute(*cmd9);

	//auto cmd10 = documents::operations::indexes::StartIndexOperation("MyIndex").get_command({});
	//auto res10 = test_suite_executor->execute(*cmd10);

	//auto cmd11 = documents::operations::indexes::StopIndexingOperation().get_command({});
	//auto res11 = test_suite_executor->execute(*cmd11);

	//auto cmd12 = documents::operations::indexes::StartIndexingOperation().get_command({});
	//auto res12 = test_suite_executor->execute(*cmd12);

	//auto cmd14 = documents::operations::indexes::GetIndexNamesOperation(0, 100).get_command({});
	//auto res14 = test_suite_executor->execute(*cmd14);
	//for(const auto& name : res14)
	//{
	//	std::cout << name << "\n";
	//}

	//auto cmd15 = documents::operations::indexes::GetIndexErrorsOperation({ "MyIndex" }).get_command({});
	//auto res15 = test_suite_executor->execute(*cmd15);

	//documents::operations::indexes::set_indexes_lock_op::Parameters params{};
	//params.index_names = { "MyIndex" };// , "Orders/ByCompany" };
	//params.mode = IndexLockMode::UNLOCK;
	//auto cmd16 = ravendb::client::documents::operations::indexes::SetIndexesLockOperation(params)		
	//	.get_command({});
	//auto res16 = test_suite_executor->execute(*cmd16);

	//auto cmd17 = ravendb::client::documents::operations::indexes::GetIndexesStatisticsOperation().get_command({});
	//auto res17 = test_suite_executor->execute(*cmd17);

	//auto cmd18 = ravendb::client::documents::operations::indexes::GetIndexingStatusOperation().get_command({});
	//auto res18 = test_suite_executor->execute(*cmd18);

	//auto cmd19 = ravendb::client::documents::operations::indexes::GetTermsOperation("MyIndex","Company")
	//	.get_command({});
	//auto res19 = test_suite_executor->execute(*cmd19);

	//auto cmd20 = ravendb::client::documents::operations::indexes::SetIndexesPriorityOperation("MyIndex", IndexPriority::UNSET)
	//	.get_command({});
	//auto res20 = test_suite_executor->execute(*cmd20);

	std::string query = R"(

	from Employees
	where StartsWith(FirstName,$prefix)

	)";

	//std::string update_query = R"(

	//from Employees as empl
	//where StartsWith(empl.FirstName,$prefix)
	//update 
	//{
	//	empl.FirstName += " The Great"
	//}

	//)";

	std::string update_query = R"(from Employees update {throw 5})";

	documents::queries::IndexQuery index_query(query);
	index_query.query_parameters.emplace("prefix", "A");
	index_query.explain_scores = true;
	index_query.show_timings = true;
	index_query.wait_for_non_stale_results = true;
	index_query.wait_for_non_stale_results_timeout = std::chrono::seconds(5);

	documents::queries::IndexQuery update_index_query(update_query);
	update_index_query.query_parameters.emplace("prefix", "A");
	update_index_query.explain_scores = true;
	update_index_query.show_timings = true;
	update_index_query.wait_for_non_stale_results = true;
	update_index_query.wait_for_non_stale_results_timeout = std::chrono::seconds(5);

	//documents::queries::QueryOperationOptions query_op_options(10 , true, std::chrono::seconds(5), true);
	//auto op = documents::operations::DeleteByQueryOperation(indexQuery, query_op_options);
	//HttpCache cache;
	//auto res = test_suite_executor->execute(op.get_command({}, {}, cache));

	//auto cmd = documents::commands::QueryCommand({}, indexQuery, false , false);
	//auto res = test_suite_executor->execute(cmd);
	//for (auto& result : res.results)
	//{
	//	std::cout << result.at("FirstName").get<std::string>() << " " << result.at("LastName").get<std::string>() << " " <<
	//		result.at("Birthday").get<impl::DateTimeOffset>() << "\n";
	//}
	//std::cout << "last query time " << res.last_query_time << "\n";
	//nlohmann::json j = res.last_query_time;
	//std::cout << "last query time " << j << "\n";

	//documents::queries::MyQueryResult r = res.create_snapshot();

	//auto patch = documents::operations::PatchRequest(R"(this.FirstName = $NiceName)");
	//patch.values = { { "NiceName", "Nancy" } };
	//documents::operations::PatchOperation op("employees/1-A", {}, patch);
	//HttpCache cache;
	//auto&& res = test_suite_executor->execute(op.get_command(FakeStore(), {}, cache));

	//auto options = documents::queries::QueryOperationOptions({},true,std::chrono::seconds(5),true);
	//documents::operations::PatchByQueryOperation op(update_index_query, options);
	//HttpCache cache;
	//auto&& res = test_suite_executor->execute(op.get_command(FakeStore(), {}, cache));


	//std::this_thread::sleep_for(std::chrono::seconds(1));
	//auto op2 = documents::operations::GetOperationStateOperation(res.operation_id);
	//auto&& res1 = test_suite_executor->execute(op2.get_command({}));


	//std::getchar();

	std::shared_ptr<http::RequestExecutor> re = http::RequestExecutor::create({ "http://127.0.0.1:8081" }, "Test",
		DocumentConventions::default_conventions(), {}, set_for_fiddler);
	const auto session_options = documents::session::SessionOptions
	{ "Test",false,true, re,documents::session::TransactionMode::SINGLE_NODE };

	auto store = documents::DocumentStore::create("http://127.0.0.1:8081", "Test");
	store->initialize();

	documents::session::DocumentSession session1 = store->open_session(session_options);
	session1.advanced().set_max_number_of_requests_per_session(1024);

	std::string query1 = R"(
	from Users
	where StartsWith(LastName, $prefix)
	)";

	std::string query2 = R"(
	from Users as u 
		select
		{
			FullName : u.FirstName+" "+u.LastName
		}
	)";

	//auto res = session1.advanced().raw_query(query1)
	//	->add_parameter("$prefix", "T")
	//	->to_list<tests::infrastructure::entities::User>();

	//auto count = session1.advanced().raw_query(query1)
	//	->add_parameter("$prefix", "T")
	//	->count();

	//auto res2 = session1.advanced().raw_query(query2)->to_list<MyQueryResult>();

	//auto res3 = session1.advanced().raw_query(query2)->take(1)->to_list<MyQueryResult>();

	//auto res4 = session1.advanced().raw_query(query2)->skip(1)->to_list<MyQueryResult>();

	////std::cout << res[0]->last_name << std::endl;

	//for(const auto& full_name : res2)
	//{
	//	std::cout << full_name->full_name << std::endl;
	//}

	//auto user = session1.load<tests::infrastructure::entities::User>("users/4");

	//session1.advanced().patch<tests::infrastructure::entities::User>("users/4", "AddressId", "Israel");
	//session1.save_changes();

	//auto user = session1.advanced().lazily().load<tests::infrastructure::entities::User>("users/7");

	//auto print_name = [](std::shared_ptr<tests::infrastructure::entities::User> u)
	//{
	//	std::cout << (u ? u->name  : "\nno such user\n\n") << std::endl;
	//};
	//print_name(user);

	//auto user3 = session1.load<tests::infrastructure::entities::User>("users/4");
	//print_name(user3);

	//auto user2 = session1.advanced().lazily().load<tests::infrastructure::entities::User>("users/4");
	//print_name(user2);
	//print_name(user2);

	std::vector<std::string> ids = { "users/1", "users/2", "users/4" };

	//auto users4 = session1.advanced().lazily().load<tests::infrastructure::entities::User>(ids.begin(), ids.end());

	//auto res = users4.get_value();

	auto store2 = documents::DocumentStore::create("http://127.0.0.1:8080", "Northwind");
	store2->set_before_perform(set_for_fiddler);
	store2->initialize();

	auto session2 = store2->open_session();
	auto emp = session2.advanced().lazily().include("ReportsTo").load<tests::infrastructure::entities::Employee>("employees/1-A");
	auto ids2 = std::vector<std::string>{ "employees/1-A" };
	//auto emp = session2.advanced().lazily().include("ReportsTo").load<tests::infrastructure::entities::Employee>
	//	(ids2.begin(), ids2.end());

	auto res = session2.advanced().eagerly().execute_all_pending_lazy_operations();

	std::cout << emp.get_value()->first_name << std::endl;
	auto is_loaded = session2.advanced().is_loaded("employees/2-A");

	std::getchar();

	//std::vector<std::string> ids{"users/1", "users/2", "no_such_user"};
	//auto users = session1.load<tests::infrastructure::entities::User>(ids.begin(), ids.end());

	//auto users2 = session1.load<tests::infrastructure::entities::User>(ids.begin(), ids.end());

	//auto new_user = std::make_shared<tests::infrastructure::entities::User>();
	//new_user->id = "users/2";
	//new_user->name = "Alexander";
	//new_user->last_name = "Timoshenko";
	//new_user->age = 38;

	//session1.store(new_user, new_user->id);
	//auto res1 = session1.load<tests::infrastructure::entities::User>("users/2");
	//std::cout << "Got user  = " << res1->name << " " << res1->last_name << std::endl;

	//session1.save_changes();

	//auto num_of_requests = session1.advanced().get_number_of_requests();

	//session1.save_changes();//no changes were made

	//auto num_of_requests2 = session1.advanced().get_number_of_requests();

	//new_user->name = "Oleksandr";

	//session1.save_changes();//changes were made

	//auto num_of_requests3 = session1.advanced().get_number_of_requests();

	//auto metadata = session1.advanced().get_metadata_for(new_user);
	//auto m = json::MetadataAsDictionary::create(
	//	R"({
	//	"Key1":{
	//		"Key1_1" : "Val1_1",
	//		"Key1_2" : "Val1_2"
	//	},
	//	"Key2" : "Val2",
	//	"Key3" : ["a", "b", "c"]
	//	})"_json);
	//metadata->insert("Dict", m);

	//session1.save_changes();//changes were made

	//auto j = std::make_shared<nlohmann::json>(R"({
	//	"name" : "Alexander",
	//	"@metadata" : {
	//		"@collection" : "Users",
	//		"@id" : "users/1"
	//	}
	//})"_json);

	//session1.store(j, (*j)["/@metadata/@id"_json_pointer].get<std::string>());
	//session1.save_changes();

	//documents::session::DocumentSession session2(session_options);
	//session2.max_number_of_requests_per_session = 1024;

	//auto j2 = session2.load<nlohmann::json>("users/1");

	//(*j2)["/@metadata/@collection"_json_pointer] = "Superusers";

	//session2.save_changes();


	//documents::session::DocumentSession session2(session_options);
	//session2.max_number_of_requests_per_session = 1024;


	//session2.delete_document<tests::User>(new_user->id);

	//session2.save_changes();

	//documents::session::DocumentSession session2(session_options);
	//session2.max_number_of_requests_per_session = 1024;
	//auto res1 = session2.load<tests::User>("users/1");

	//std::cout << "Got user  = " << res1->name << " " << res1->last_name << std::endl;

	//auto res2 = session2.load<tests::User>("users/1");

	//std::cout << "Got user  = " << res2->name << " " << res2->last_name << std::endl;

	//res1->last_name = "Timoshenko";
	//auto res3 = session2.load<tests::User>("users/1");

	//std::cout << "Got user  = " << res3->name << " " << res3->last_name << std::endl;


}


