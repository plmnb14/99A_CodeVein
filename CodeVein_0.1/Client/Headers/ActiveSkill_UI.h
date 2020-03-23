#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Management.h"

BEGIN(Client)

class CActiveSkill_UI final : public CUI
{
private:
	explicit CActiveSkill_UI(_Device pGraphic_Device);
	explicit CActiveSkill_UI(const CActiveSkill_UI& rhs);
	virtual ~CActiveSkill_UI() = default;

public:
	_uint Get_Skill_Index() { return m_iIndex; }

public:
	void Set_Skill_Index(_uint iIndex) { m_iIndex = iIndex; }

public:
	void Set_OpenUI(_bool bOpen) { m_bIsOpen = bOpen; }

public:
	virtual HRESULT			Ready_GameObject_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(_double TimeDelta);
	virtual _int			Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT			Render_GameObject();

private:
	HRESULT					Add_Component();
	HRESULT					SetUp_ConstantTable();
	void					Update_UIPos();

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

	_bool					m_bIsOpen = true;

	_v2						m_vOldPos;

	_uint					m_iIndex = 0;

public:
	static CActiveSkill_UI*		Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};

END