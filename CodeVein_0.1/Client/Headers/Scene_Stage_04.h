#pragma once

#include "Client_Defines.h"
#include "Scene.h"
#include "Loading.h"

#include "Management.h"

BEGIN(Client)

class CScene_Stage_04 final : public CScene
{
private:
	explicit CScene_Stage_04(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CScene_Stage_04() = default;

public:
	virtual HRESULT Ready_Scene();
	virtual _int Update_Scene(_double TimeDelta);
	virtual HRESULT Render_Scene();

public:
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Environment(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Colleague(const _tchar* pLayerTag);

private:
	CNavMesh*		m_pNavMesh = nullptr;

private:
	_float m_fMapFogDelay = 10.f;

private:
	HRESULT Ready_LightDesc();
	void	Create_Fog(_double TimeDelta);

public:
	static CScene_Stage_04* Create(LPDIRECT3DDEVICE9 pGraphic_Device, _bool _bLoadStatic);
	virtual void Free();
};

END