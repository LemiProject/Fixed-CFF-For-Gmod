#include "aye.h"


#include <filesystem>
#include <iostream>
#include <vector>
#include <mutex>

#include "file_tools.h"
//#include "../../interfaces.h"
//#include "../../utils/game_utils.h"

std::mutex run_mutex;

std::vector<std::string> codes_to_run;
std::vector<std::string> codes_to_run_safe;

void lua_features::add_code_to_run(const std::string& str)
{
	codes_to_run.clear();
	codes_to_run.push_back(str);

	{
		std::unique_lock l(run_mutex);
		codes_to_run.swap(codes_to_run_safe);
	}
}

void lua_features::run_all_code()
{
	std::unique_lock l(run_mutex);
	is_hack_call = true;

	auto lua = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
	if (lua)
	{
		for (auto i : codes_to_run_safe)
		{
			try
			{
				lua->RunString("ExecIloveYou", "", i.c_str(), true, true);
			}
			catch (...)
			{
			}
		}
	}

	codes_to_run_safe.clear();
	is_hack_call = false;
}

