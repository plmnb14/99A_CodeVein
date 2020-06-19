#pragma once

#include "Client_Item_Enum.h"
#include "UI.h"

BEGIN(Client)

class CPet_Slot final : public CUI
{
private:
	explicit CPet_Slot(_Device pDevice);
	explicit CPet_Slot(const CPet_Slot& rhs);
	virtual ~CPet_Slot() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	_bool Pt_InRect();
	ITEM_NAMETYPE Get_PetType() { return m_ePetName; }
	_bool Get_Select() { return m_bIsSelect; }

	void Set_PetType(ITEM_NAMETYPE ePetName) { m_ePetName = ePetName; }
	void Set_Select(_bool bIsSelect) { m_bIsSelect = bIsSelect; }

private:
	HRESULT					Add_Component();
	HRESULT					SetUp_ConstantTable(_uint iIndex);

public:
	static CPet_Slot*			Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();

private:
	CTransform*				m_pTransform = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	CTexture*				m_pTexture = nullptr;
	CShader*				m_pShader = nullptr;
	CBuffer_RcTex*			m_pBuffer = nullptr;

	_bool					m_bIsCollMouse = false;
	_bool					m_bIsSelect = false;
	ITEM_NAMETYPE			m_ePetName = ITEM_NAMETYPE::NAMETYPE_End;

};

END

