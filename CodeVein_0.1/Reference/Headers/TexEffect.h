#pragma once

#include "RenderObject.h"
#include "ActeiveObject.h"
#include "Management.h"
#include "Effect.h"

BEGIN(Engine)

class ENGINE_DLL CTexEffect : public CEffect
{

protected:
	explicit CTexEffect(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CTexEffect(const CTexEffect& rhs);
	virtual ~CTexEffect() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual HRESULT LateInit_GameObject();
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	HRESULT Render_GameObject_HWInstance();

protected:
	virtual void Setup_Info();

protected:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CTexture*				m_pGradientTextureCom = nullptr;
	CTexture*				m_pColorTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CMesh_Static*			m_pDecalCube = nullptr;

protected:
	CManagement*			m_pManagement = nullptr;

private:

protected:
	void Check_Frame(_double TimeDelta);
	void Check_Move(_double TimeDelta);
	void Check_LifeTime(_double TimeDelta);
	void Check_Alpha(_double TimeDelta);
	void Check_Color(_double TimeDelta);
	void Check_CreateDelay(_double TimeDelta);

	void Setup_Billboard();

protected:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	void Change_EffectTexture(const _tchar* _Name);
	void Change_GradientTexture(const _tchar* _Name);
	void Change_ColorTexture(const _tchar* _Name);

public:
	static CTexEffect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	static CTexEffect* Create(LPDIRECT3DDEVICE9 pGraphic_Device, EFFECT_INFO* pInfo);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END