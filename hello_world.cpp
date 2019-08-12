#include "raven_cpp_client.h"

int main()
{
    try
    {
        throw ravendb::client::exceptions::RavenException("Test");
    }
    catch (const std::runtime_error& e)
    {
        std::string msg = e.what();
        std::cout << msg << "1 - OK" << std::endl;
    }
    auto name = ravendb::client::impl::utils::GetCppClassName::get_simple_class_name(
            typeid(ravendb::client::exceptions::AllTopologyNodesDownException));

    std::cout << (name == "AllTopologyNodesDownException" ? "Test2 - OK" : "Something wrong...") << std::endl;
}

