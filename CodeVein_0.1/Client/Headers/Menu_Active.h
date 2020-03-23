#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Management.h"

#include "ActiveSkill_UI.h"

BEGIN(Client)

class CMenu_Active : public CUI
{
private:
	explicit CMenu_Active(_Device pGraphic_Device);
	explicit CMenu_Active(const CMenu_Active& rhs);
	virtual ~CMenu_Active() = default;

public:
	_bool Get_WindowState() { return m_bIsOpenWindow; }

public:
	void Set_WindowState(_bool bState) { m_bIsOpenWindow = bState; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	void	SetUp_WindowPos();

public:
	HRESULT SetUp_ActiveSkill(_uint iActiveIndex);

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

	_bool					m_bIsOpenWindow = false;

	vector<CActiveSkill_UI*> m_vecActiveSkill;

public:
	static CMenu_Active*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END