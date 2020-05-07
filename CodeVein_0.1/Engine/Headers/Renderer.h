#pragma once

// 화면에 그려져야하는 객체들을 그리는 순서대로 모아서 보관한 객체.


#include "Component.h"
#include "GameObject.h"
#include "Target_Manager.h"
#include "Light_Manager.h"
#include "Shader.h"
#include "Texture.h"

BEGIN(Engine)
class ENGINE_DLL CRenderer final : public CComponent
{
private:
	explicit CRenderer(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CRenderer() = default;
public:
	virtual HRESULT Ready_Component_Prototype();
	virtual HRESULT Ready_Component(void* pArg);
public:
	HRESULT Add_RenderList(RENDERID eGroup, CGameObject* pGameObject);
	HRESULT Draw_RenderList();

public:
	void Set_ShadowLightPos(_v3 _vPos) { m_vShadowLightPos = _vPos; }

public:
	void DOF_On(_bool bOn);
	void Mono_On(_bool bOn);
	void Fog_On(_bool bOn);

private:
	list<CGameObject*>			m_RenderList[RENDER_END];
	typedef list<CGameObject*>	RENDERLIST;

private:
	_v3							m_vShadowLightPos = V3_NULL;

private:
	_bool						m_bOnRenderTarget = false;
	_mat						m_matLastWVP;

	INSTANCEDATA*				m_pInstanceData = nullptr;
	_int						m_iInstanceCnt = 0;

	_bool						m_bMono = false;
	_float						m_fToneGradient = 1.f;
	_int						m_iToneIdx = 5;

	_bool						m_bDOF = false;
	_float						m_fFocus = 0.f;
	_float						m_fRange = 0.f;

	_bool						m_bFog = false;
	_float						m_fFogDestiny = 0.f;

private:
	CTarget_Manager*			m_pTarget_Manager = nullptr;
	CLight_Manager*				m_pLight_Manager = nullptr;
private:
	CShader*					m_pShader_LightAcc = nullptr;
	CShader*					m_pShader_Blend = nullptr;
	CShader*					m_pShader_Shadow = nullptr;
	CShader*					m_pShader_Effect = nullptr;
	CShader*					m_pShader_Blur = nullptr;
	CShader*					m_pShader_SSAO = nullptr;

	CBuffer_ViewPort*			m_pViewPortBuffer = nullptr;
	CTexture*					m_pSSAOTexture = nullptr;
	CTexture*					m_pGradingTexture = nullptr;
	CTexture*					m_pGradingTextureTest = nullptr;
	
public:
	//====================
	// 임시 타겟 변수
	//====================
	_v3 m_vLookAtPos = {};
	virtual void Set_LookAtPos(_v3 _vLookAt) { m_vLookAtPos = _vLookAt; }
	//====================

private:
	HRESULT Render_Priority();
	HRESULT Render_NonAlpha();
	HRESULT Render_ShadowMap();
	HRESULT Render_Shadow();
	HRESULT Render_MotionBlurTarget();
	HRESULT Render_Distortion();
	HRESULT Render_Alpha();
	HRESULT Render_Effect();
	HRESULT Render_Instance();
	HRESULT Render_UI();
	HRESULT Render_3dUI();
	HRESULT Render_SSAO();

private:
	HRESULT Render_LightAcc();
	HRESULT Render_Blend();
	HRESULT Render_BrightPass();
	HRESULT Render_Blur();
	HRESULT Render_MotionBlurObj();
	HRESULT Render_MotionBlur();
	HRESULT Render_ToneMapping();
	HRESULT Render_BlurDOF();
	HRESULT Render_After();

private:
	void Calc_CSM();
	void Calc_FrustumBoundSphere(_float fNear, _float fFar, _v3& vBoundCenter, _float& fBoundRadius);

public:
	static CRenderer* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone_Component(void* pArg);
	virtual void Free();
};

END


