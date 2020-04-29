#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CBloodCodeSlot;
class CBloodCodeSelectUI final : public CUI
{
private:
	explicit CBloodCodeSelectUI(_Device pDevice);
	explicit CBloodCodeSelectUI(const CBloodCodeSelectUI& rhs);
	virtual ~CBloodCodeSelectUI() = default;

public:
	BloodCode_ID  Get_Type();

public:
	void Set_SlotPos(_v3 vPosition) { m_vSlotPosition = vPosition; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_uint iIndex);
	void	SetUp_Default();
	void	Click_BloodCodeSlot();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CCollider*				m_pCollider = nullptr;

private:
	BloodCode_ID			m_eType = BloodCode_End;
	_bool					m_bIsSelect = false;
	vector<CBloodCodeSlot*>	m_vecBloodCodeSlot;
	_uint					m_iSelectIndex = 0;
	_v3						m_vSlotPosition;
	_float					m_fCross = 100.f;

public:
	static CBloodCodeSelectUI*		Create(_Device pGraphic_Device);
	virtual CGameObject*			Clone_GameObject(void* pArg);
	virtual void					Free();
};

END