#pragma once

#include "Client_Defines.h"
#include "Scene.h"
#include "Loading.h"

#include "Management.h"

BEGIN(Client)

class CScene_Stage_Base final : public CScene
{
private:
	explicit CScene_Stage_Base(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CScene_Stage_Base() = default;

public:
	virtual HRESULT Ready_Scene();
	virtual _int Update_Scene(_double TimeDelta);
	virtual HRESULT Render_Scene();
	HRESULT Ready_Layer_Custom(const _tchar* pLayerTag);

public:
	virtual HRESULT Ready_Player();

public:	
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag); 

private:
	CNavMesh*			m_pNavMesh = nullptr;

private:
	CLoading*			m_pLoading = nullptr;
	_float				m_fMapFogDelay = 10.f;
	_bool				m_bLoadingCheck = false;
private:
	HRESULT Ready_LightDesc();

private:
	void	Create_Fog(_double TimeDelta);

public:
	static CScene_Stage_Base* Create(LPDIRECT3DDEVICE9 pGraphic_Device , _bool _bLoadStatic);
	virtual void Free();
};

END