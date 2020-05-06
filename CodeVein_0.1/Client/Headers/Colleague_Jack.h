#pragma once

#include "Client_Defines.h"
#include "Management.h"
#include "UI.h"

BEGIN(Client)

class CColleague_Jack final : public CUI
{
private:
	explicit CColleague_Jack(_Device Graphic_Device);
	explicit CColleague_Jack(const CColleague_Jack& rhs);
	virtual ~CColleague_Jack() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual	HRESULT	LateInit_GameObject();
	virtual HRESULT Render_GameObject();

public:
	void		Set_TotalHP(const _float fTotalHP) { m_fTotalHP = fTotalHP; }
	void		Set_Target(CGameObject* Target) { m_pTarget = Target; }

private:
	HRESULT		Add_Component();
	HRESULT		SetUp_ConstantTable(_uint iIndex);
	void		SetUp_State(_double TimeDelta);
	void		SetUp_RenderIndex();
	void		SetUp_Default();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	_float		m_fSpeed = 0.f;

	_float		m_fAlpha = 0.f;
	_float		m_fPercentage = 0.f;
	_float		m_fWhite_Percentage = 0.f;

	_float		m_fColleagueHp = 0.f;
	_float		m_fTotalHP = 0.f;
	_float		m_fOldHP = 0.f;

	_uint		m_iRenderCheck_Index = 0;


public:
	static CColleague_Jack*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END