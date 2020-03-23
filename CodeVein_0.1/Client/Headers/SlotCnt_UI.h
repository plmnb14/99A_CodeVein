#pragma once

#include "Client_Defines.h"
#include "Management.h"
#include "UI.h"

BEGIN(Client)

class CSlotCnt_UI final : public CUI
{
private:
	explicit CSlotCnt_UI(_Device pGraphic_Device);
	explicit CSlotCnt_UI(const CSlotCnt_UI& rhs);
	virtual ~CSlotCnt_UI() = default;

public:
	void Set_Count(_uint iItemCnt) { m_iItemCnt = iItemCnt; }

public:
	virtual HRESULT			Ready_GameObject_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(_double TimeDelta);
	virtual _int			Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT			Render_GameObject();

private:
	HRESULT					Add_Component();
	HRESULT					SetUp_ConstantTable();

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

	_uint					m_iItemCnt = 0;

public:
	static CSlotCnt_UI*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END