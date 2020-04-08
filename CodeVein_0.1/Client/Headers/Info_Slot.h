#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Expendables.h"

BEGIN(Client)
class CNumberUI;
class CInfo_Slot final : public CUI
{
private:
	explicit CInfo_Slot(_Device pDevice);
	explicit CInfo_Slot(const CInfo_Slot& rhs);
	virtual ~CInfo_Slot() = default;

public:
	void Set_SlotInfo(CExpendables::EXPEND_TYPE eType) { m_eInfo = eType; }
	void Set_Number(_uint iNumber);

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CExpendables::EXPEND_TYPE m_eInfo = CExpendables::EXPEND_END;
	CNumberUI*				m_pNumberUI = nullptr;
	_uint					m_iNumber = 0;

public:
	static CInfo_Slot*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END
