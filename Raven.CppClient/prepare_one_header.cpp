#include <experimental/filesystem>
#include <fstream>

constexpr char ONE_HEADER_NAME[] = "raven_cpp_client.h";

int main()
{
    std::ofstream one_header(ONE_HEADER_NAME, std::ios_base::out| std::ios_base::trunc);

    if(!one_header)
    {
        throw std::runtime_error(ONE_HEADER_NAME + std::string(" can't be opened for writing"));
    }

    auto path = std::experimental::filesystem::current_path();

    one_header << "#pragma once\n\n";

    for(const auto& entry : std::experimental::filesystem::directory_iterator(path))
    {
        if(auto name = entry.path().filename().string();
            name.length() > 2 && name.substr(name.length()-2) == ".h" && name != ONE_HEADER_NAME)
        {
            one_header << "#include " << name << "\"\n";
        }
    }
}

