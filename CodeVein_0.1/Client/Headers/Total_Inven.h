#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CInventory_Icon;
class CInventory;
class CWeapon_Slot;
class CArmor_Slot;
class CBloodCode_Icon;
class CInfo_Slot;
class CNumberUI;
class CQuickSlot;
class CSkillIcon;
class CSkillSlot;
class CNoticeUI;
class CTotal_Inven final : public CUI
{
private:
	explicit CTotal_Inven(_Device pDevice);
	explicit CTotal_Inven(const CTotal_Inven& rhs);
	virtual ~CTotal_Inven() = default;

public:
	_bool Get_Detail() { return m_bIsDetail; }
	BloodCode_ID Get_PlayerBloodCode() { return m_ePlayerBloodCode; }
	Skill_ID Get_Registration_Skill(_uint iNum);
	WEAPON_DATA Get_Registration_Weapon(_uint iNum);

public:
	void Set_Detail(_bool bIsDetail) { m_bIsDetail = bIsDetail; }
	void Set_PlayerBloodCodeType(BloodCode_ID eID) { m_ePlayerBloodCode = eID; }
	void Set_Skill_ID(_uint iNum, Skill_ID eSkillID);
	void Set_WeaponParam(_uint iIndex, WPN_PARAM tWpnParam);

public:
	virtual HRESULT			Ready_GameObject_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(_double TimeDelta);
	virtual _int			Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT			Render_GameObject();

private:
	HRESULT					Add_Component();
	HRESULT					SetUp_ConstantTable();
	void					SetUp_Default();
	void					Click_Icon();
	void					SkillSlot_Touch_Sound(_uint i);
	void					SetUp_SubUI_Active(_bool bIsActive);

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	vector<CInventory_Icon*>	m_vecIcon;
	_bool					m_bIsDetail = false;
	CInventory*				m_pInventory = nullptr;
	CWeapon_Slot*			m_pWeapon_Slot[2];
	CArmor_Slot*			m_pArmor_Slot;
	CBloodCode_Icon*		m_pBloodCode = nullptr;
	CInfo_Slot*				m_pQuickSlotInfo[8];
	CNumberUI*				m_pNumberUI[8];
	BloodCode_ID			m_ePlayerBloodCode = BloodCode_End;
	vector<CSkillSlot*>		m_vecSkillIcon;
	CNoticeUI*				m_pNoticeUI = nullptr;
	_bool					m_bIsSubActive = false;

public:
	static CTotal_Inven*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END
