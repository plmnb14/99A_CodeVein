#pragma once

// 현재 할당되어 동작하는 씬 객체를 보관한다.
// 주요 함수를 호출할 수 있도록 한다.
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