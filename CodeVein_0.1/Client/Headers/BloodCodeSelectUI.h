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
	BloodCode_ID  Get_CodeID();

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_uint iIndex);
	void	SetUp_Default();
	void	Click_BloodCodeSlot();

public:
	void Release_BloodCode(BloodCode_ID eBloodCodeID);

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CCollider*				m_pCollider = nullptr;

private:
	BloodCode_ID			m_eID = BloodCode_End;
	vector<CBloodCodeSlot*>	m_vecBloodCodeSlot;
	_float					m_fCross = 100.f;

public:
	static CBloodCodeSelectUI*		Create(_Device pGraphic_Device);
	virtual CGameObject*			Clone_GameObject(void* pArg);
	virtual void					Free();
};

END