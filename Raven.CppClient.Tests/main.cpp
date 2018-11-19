#include "pch.h"

class MyEnv : public ::testing::Environment
{
public:
	~MyEnv() override = default;

	// Override this to define how to set up the environment.
	void SetUp() override
	{
		
	}

	// Override this to define how to tear down the environment.
	//void TearDown() override {}
};

class GlobalEnvRegister final
{
	::testing::Environment* _env = nullptr;
public:
	GlobalEnvRegister()
		:_env(new MyEnv)
	{
		::testing::AddGlobalTestEnvironment(_env);
	}

	GlobalEnvRegister(const GlobalEnvRegister& other) = delete;
	GlobalEnvRegister(GlobalEnvRegister&& other) noexcept = delete;
	GlobalEnvRegister& operator=(const GlobalEnvRegister& other) = delete;
	GlobalEnvRegister& operator=(GlobalEnvRegister&& other) noexcept = delete;
};

namespace
{
	const GlobalEnvRegister GLOBAL_ENV_REGISTER;
}

//int main(int argc, char* argv[])
//{
//	::testing::InitGoogleTest(&argc, argv);
//
//	RUN_ALL_TESTS();
//}
