#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Armor_Slot.h"

BEGIN(Client)

class CArmor_Inven : public CUI
{
private:
	explicit CArmor_Inven(_Device pDevice);
	explicit CArmor_Inven(const CArmor_Inven& rhs);
	virtual ~CArmor_Inven() = default;

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
	void					Click_Inven();
	void					Regist_Armor(CArmor_Slot* pArmorSlot);
	void					UnRegist_Armor(CArmor_Slot* pArmorSlot);

public:
	void					Add_Armor(CArmor::ARMOR_TYPE eType);

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	vector<CArmor_Slot*>	m_vecArmorSlot;
	CArmor::ARMOR_TYPE		m_eRegistArmor = CArmor::ARMOR_END;

public:
	static CArmor_Inven*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END