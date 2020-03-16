#pragma once

// ���� �Ҵ�Ǿ� �����ϴ� �� ��ü�� �����Ѵ�.
// �ֿ� �Լ��� ȣ���� �� �ֵ��� �Ѵ�.
#include "Scene.h"

BEGIN(Engine)

class CScene_Manager final : public CBase
{
	DECLARE_SINGLETON(CScene_Manager)
private:
	explicit CScene_Manager();
	virtual ~CScene_Manager() = default;
public:
	HRESULT SetUp_CurrentScene(CScene* pScene);
	_int Update_Scene(_double TimeDelta);
	HRESULT Render_Scene();
private:
	CScene*		m_pCurrentScene = nullptr;
public:
	virtual void Free();
};

END