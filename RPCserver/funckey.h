#pragma once
#include <string>

/*���ݷ���ֵ���͡������������ͺͺ���������Ψһ�ĺ�����ֵ*/

std::string generate_funckey(int ret_type, int para_type, std::string name)
{
	std::string res = std::to_string(ret_type) + "*" + std::to_string(para_type) + "*" + name;
	return move(res);
}