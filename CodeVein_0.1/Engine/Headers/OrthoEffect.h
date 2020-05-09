#pragma once

#include "RenderObject.h"
#include "ActiveObject.h"
#include "Management.h"
#include "Effect.h"

BEGIN(Engine)

class ENGINE_DLL COrthoEffect : public CEffect
{

protected:
	explicit COrthoEffect(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit COrthoEffect(const COrthoEffect& rhs);
	virtual ~COrthoEffect() = default;

public:
	INSTANCEDATA Get_InstanceData();

public:
	void Set_UV_Speed(_float fX, _float fY);
	void Set_Mask(const _tchar* _Name, _int _iMaskIdx);
public:
	HRESULT SetUp_ConstantTable_Instance(CShader* pShader);

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
	CTexture*				m_pMaskTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

protected:
	CManagement*			m_pManagement = nullptr;

private:
	_mat m_matWorld;
	_mat m_matView;
	_mat m_matProj;

	_float	m_fUV_Value_X = 0.f;
	_float	m_fUV_Value_Y = 0.f;
	_float	m_fUV_Speed_X = 0.f;
	_float	m_fUV_Speed_Y = 0.f;

	_int	m_iMaskIdx = 0;

protected:
	void Check_Frame(_double TimeDelta);
	void Check_Move(_double TimeDelta);
	void Check_LifeTime(_double TimeDelta);
	void Check_Alpha(_double TimeDelta);
	void Check_Color(_double TimeDelta);
	void Check_CreateDelay(_double TimeDelta);

	void Setup_Ortho();

protected:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(CShader* pShader);
	void Change_EffectTexture(const _tchar* _Name);
	void Change_GradientTexture(const _tchar* _Name);
	void Change_ColorTexture(const _tchar* _Name);
	void Change_MaskTexture(const _tchar* _Name);

public:
	static COrthoEffect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	static COrthoEffect* Create(LPDIRECT3DDEVICE9 pGraphic_Device, EFFECT_INFO* pInfo);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END