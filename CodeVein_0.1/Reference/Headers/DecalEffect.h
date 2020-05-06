#pragma once

#include "RenderObject.h"
#include "ActeiveObject.h"
#include "Management.h"
#include "Effect.h"

BEGIN(Engine)

class ENGINE_DLL CDecalEffect : public CEffect
{

protected:
	explicit CDecalEffect(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CDecalEffect(const CDecalEffect& rhs);
	virtual ~CDecalEffect() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual HRESULT LateInit_GameObject();
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_SetShader(CShader* pShader);

protected:
	virtual void Setup_Info();

protected:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CTexture*				m_pGradientTextureCom = nullptr;
	CTexture*				m_pColorTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_CubeTex*		m_pBufferCom = nullptr;

protected:
	CManagement*			m_pManagement = nullptr;

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
	HRESULT SetUp_ConstantTable(CShader* pShader);
	void Change_EffectTexture(const _tchar* _Name);
	void Change_GradientTexture(const _tchar* _Name);
	void Change_ColorTexture(const _tchar* _Name);

private:
	_float	m_fDissolveStartTime = 6.f;

public:
	static CDecalEffect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	static CDecalEffect* Create(LPDIRECT3DDEVICE9 pGraphic_Device, EFFECT_INFO* pInfo);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END