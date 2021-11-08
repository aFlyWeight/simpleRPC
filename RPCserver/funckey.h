#pragma once
#include <string>

/*根据返回值类型、函数参数类型和函数名生成唯一的函数键值*/

std::string generate_funckey(int ret_type, int para_type, std::string name)
{
	std::string res = std::to_string(ret_type) + "*" + std::to_string(para_type) + "*" + name;
	return move(res);
}