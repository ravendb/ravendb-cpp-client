#include "pch.h"
//#include "stdafx.h"
//#include "GetDocumentsCommand.h"
//#include "PutDocumentCommand.h"
//#include "RequestExecutor.h"
//#include "GetDatabaseRecordCommand.h"
//#include "CreateDatabaseCommand.h"
//#include "DeleteDocumentCommand.h"
//#include "ravendb::client.h"
//
//using namespace ravendb::client;
//
//using fiddler + verbose
//CURL* debug_curl_init(void*)
//{
//	auto curl = curl_easy_init();
//	if (curl == nullptr)
//	{
//		std::runtime_error("debug_curl_init failed");
//	}
//	for fiddler
//	curl_easy_setopt(curl, CURLOPT_PROXY, "127.0.0.1:8888");
//
//	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
//	return curl;
//}
//
//
//int main(int argc, char** argv)
//{
////	RavenGlobalInit::make_global_initializations();
//
//	std::unique_ptr<RequestExecutor> rq;
//	try
//	{
//		rq = RequestExecutor::create({ "http://127.0.0.1:8080" }, "Northwind", {}, { debug_curl_init, nullptr });
//	}
//	catch (RavenError &re)
//	{
//		std::cout << re.what() << std::endl;
//		return 1;
//	}
//
//
//	GetDatabaseRecordCommand cmd2("Northwind");
//
//	DatabaseRecordWithEtag result2;
//
//	try
//	{
//		result2 = rq->execute(cmd2);
//	}
//	catch (RavenError &re)
//	{
//		std::cout << re.what() << std::endl;
//		return 2;
//	}
//
//	std::cout << "The db's name and etag are " << result2.database_name << " and " << result2.etag << std::endl;
//	   
//
//	{
//		CreateDatabaseCommand cmd({ "abc" }, 1);
//		DatabasePutResult result3;
//
//		try
//		{
//			result3 = rq->execute(cmd);
//		}
//		catch (RavenError &re)
//		{
//			std::cerr << re.what() << std::endl;
//			return 2;
//		}
//
//		std::cout << "raftCommandIndex=" << result3.raftCommandIndex << " name=" << result3.name << "\n";
//		std::cout << "nodes_added_to:\n";
//
//		std::for_each(result3.nodes_added_to.cbegin(), result3.nodes_added_to.cend(), [](auto node) {std::cout << node << std::endl; });
//
//		rq.reset();
//	}
//
//	std::unique_ptr<RequestExecutor> rq;
//	try
//	{
//		rq = RequestExecutor::create({ "http://127.0.0.1:8080" }, "abc", {}, { debug_curl_init, nullptr });
//	}
//	catch (RavenError &re)
//	{
//		std::cout << re.what() << std::endl;
//		return 1;
//	}
//	
//	{
//		std::string sample_doc = R"(
//		{
//			"last_name": "Timoshenko",
//			"name" : "Alexander",
//			"@metadata" :
//				{
//					"@collection": "Employees"
//				}
//		})";
//
//		nlohmann::json j = nlohmann::json::parse(sample_doc);
//
//		PutDocumentCommand cmd("newID", {}, j);
//
//
//		PutResult result;
//		try
//		{
//			result = rq->execute(cmd);
//		}
//		catch (RavenError &re)
//		{
//			std::cout << re.what() << std::endl;
//			return 3;
//		}
//
//		std::cout << "Result id is :" << result.id << std::endl;
//		std::cout << "Change vector is :" << result.change_vector << std::endl;
//	}
//
//	GetDocumentsCommand cmd(std::vector<std::string>{ "newID" }, {}, false);
//	GetDocumentsResult result;
//	try
//	{
//		result = rq->execute(cmd);
//	}
//	catch (RavenError &re)
//	{
//		std::cout << re.what() << std::endl;
//		return 2;
//	}
//	for (auto& doc : result.results)
//	{
//		if (std::string(doc.type_name()) == "null")
//		{
//			continue;
//		}
//		auto id = doc.at("@metadata").at("@id").get<std::string>();
//		auto name = doc.at("name").get<std::string>();
//		std::cout << "id - " << id << ", name - " << name << std::endl;
//	}
//
//	DeleteDocumentCommand cmd("id1","C:14-cRVqyd3o9Uy+rfjXNwpYDA");
//	try
//	{
//		rq->execute(cmd);
//	}
//	catch (RavenError &re)
//	{
//		std::cout << re.what() << std::endl;
//		return 3;
//	}
//
//
//	return 0;
//}
