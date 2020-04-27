#pragma once

#include "Client_Defines.h"
#include "Scene.h"
#include "Loading.h"

#include "Management.h"

BEGIN(Client)

class CScene_Stage_Training final : public CScene
{
private:
	explicit CScene_Stage_Training(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CScene_Stage_Training() = default;

public:
	virtual HRESULT Ready_Scene();
	virtual _int Update_Scene(_double TimeDelta);
	virtual HRESULT Render_Scene();

public:
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Enemies();
	HRESULT Ready_Layer_Environment();

private:
	void Create_Fog(_double TimeDelta);
	void Create_Snow(_double TimeDelta);

private:
	CNavMesh*		m_pNavMesh = nullptr;

private:
	HRESULT Ready_LightDesc();

private:
	_float m_fMapFogDelay = 10.f;
	_float m_fMapWindDustDelay = 1.f;
	_float m_fMapSnowDelay = 1.f;

public:
	static CScene_Stage_Training* Create(LPDIRECT3DDEVICE9 pGraphic_Device, _bool _bLoadStatic);
	virtual void Free();
};

END