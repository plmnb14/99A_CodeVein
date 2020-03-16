#pragma once

// Ư�� ��ü�� �θ�μ� �ش� ��ü�� ���� ����(��ü�� �ּҸ� ������ ������)�� ī�����ϳ�.

#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	explicit CBase();
	virtual ~CBase() = default;
public:
	unsigned long AddRef(); // ���۷���ī��Ʈ�� ������Ű��.
	unsigned long Release(); // ���۷���ī��Ʈ�� �����ҽ�Ű�� or �����Ѵ�.
private:
	unsigned long m_dwRefCnt = 0;
public:
	virtual void Free() = 0;
};

END