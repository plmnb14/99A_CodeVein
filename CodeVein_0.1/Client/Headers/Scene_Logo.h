#pragma once

#include "Client_Defines.h"
#include "Scene.h"
#include "Loading.h"

#include "OrthoEffect.h"

BEGIN(Client)
class CLogoBtn;
class CLoadingScreen;
class CScene_Logo final : public CScene
{
public:
	enum DEBUG_SceneChange
	{
		Stage_Base, Stage_Training, Stage_01, Stage_02, Stage_03, Stage_04, Stage_End
	};

private:
	explicit CScene_Logo(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CScene_Logo() = default;

public:
	virtual HRESULT Ready_Scene();
	virtual _int Update_Scene(_double TimeDelta);
	virtual HRESULT Render_Scene();

public:
	HRESULT Ready_Essential_Prototype_GameObject();
	HRESULT Ready_Layer_Logo(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Custom(const _tchar* pLayerTag);

private:
	virtual void Update_DebugStage_Console();

private:
	Engine::EFFECT_INFO * Read_EffectData(const _tchar * szPath);

private:
	CLoading*			m_pLoading = nullptr;
	CLogoBtn*			m_pLogoBtn = nullptr;
	CLoadingScreen*		m_pLoadingScreen = nullptr;
	_bool				m_bIsChangeStage = false;
	COrthoEffect*		m_pGlitterEffect_0 = nullptr;
	COrthoEffect*		m_pGlitterEffect_1 = nullptr;
	COrthoEffect*		m_pTitleBG = nullptr;

private:
	DEBUG_SceneChange   m_eSceneChange = Stage_Training;

private:
	virtual void Logo_KeyInput();

public:
	static CScene_Logo* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free();
};

END