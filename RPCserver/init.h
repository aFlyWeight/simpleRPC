#pragma once
#include "funckey.h"
#include "funcmap.h"

//��ʼ��ʱ
//���ذ��������ͺͺ���

#define REG_PARA(name) \
	paramap::getInstance().bind(#name)
#define REG_FUNC(paratype, rettype, name) \
	 bind_func(#paratype, #rettype, #name)

void bind_func(string stra, string strb, string strc)
{
	int a = paramap::getInstance().getindexbytype(stra);
	int b = paramap::getInstance().getindexbytype(strb);  
	funcmap::getInstance().bind(generate_funckey(b, a, strc));
}

void init()
{
	//������
	REG_PARA(VD);
	REG_PARA(A);
	REG_PARA(B);
	REG_PARA(C);
	REG_PARA(D);
	//����funcCall���������˳��󶨺���
	REG_FUNC(A,B,m_sum);
	REG_FUNC(A, B, m_sub);
	REG_FUNC(A, B, m_multi);
	REG_FUNC(A, B, m_dvd);
	REG_FUNC(A, B, m_xor);
	REG_FUNC(C, B, all_sum);
	REG_FUNC(VD, D, printcat);
}