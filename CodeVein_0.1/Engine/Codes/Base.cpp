#include "..\Headers\Base.h"

CBase::CBase()
	: m_dwRefCnt(0)
{
}

unsigned long CBase::AddRef()
{
	return ++m_dwRefCnt;
}

unsigned long CBase::Release()
{
	if (0 == m_dwRefCnt) // 삭제
	{
		Free();

		delete this;		

		return 0;		
	}

	else // 감소
		return m_dwRefCnt--;
}

