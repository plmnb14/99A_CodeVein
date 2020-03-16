#pragma once

// 특정 객체의 부모로서 해당 객체의 참조 갯수(객체의 주소를 보관한 포인터)를 카운팅하낟.

#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	explicit CBase();
	virtual ~CBase() = default;
public:
	unsigned long AddRef(); // 레퍼런스카운트를 증가시키낟.
	unsigned long Release(); // 레퍼런스카운트르 ㄹ감소시키낟 or 삭제한다.
private:
	unsigned long m_dwRefCnt = 0;
public:
	virtual void Free() = 0;
};

END