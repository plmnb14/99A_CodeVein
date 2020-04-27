#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CBloodSkillSlot;
class CBloodSkillCursor;
class CSkillReleaseUI final : public CUI
{
private:
	explicit CSkillReleaseUI(_Device pDevice);
	explicit CSkillReleaseUI(const CSkillReleaseUI& rhs);
	virtual ~CSkillReleaseUI() = default;

public:
	void Set_Type(BloodCode_ID eType) { m_eBloodCodeType = eType; }
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

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	vector<CBloodSkillSlot*> m_vecSkillSlot;
	BloodCode_ID			m_eBloodCodeType;
	_v3						m_vSlotPosition;
	CBloodSkillCursor*		m_pBloodSkillCursor = nullptr;

public:
	static CSkillReleaseUI*		Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END
