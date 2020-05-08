#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Pet.h"
BEGIN(Client)

class CPet_Slot final : public CUI
{
private:
	explicit CPet_Slot(_Device pDevice);
	explicit CPet_Slot(const CPet_Slot& rhs);
	virtual ~CPet_Slot() = default;

public:
	CPet::PET_TYPE Get_PetType() { return m_ePetType; }
	_uint Get_PetLevel() { return m_iPetLevel; }

public:
	void Set_PetType(CPet::PET_TYPE eType) { m_ePetType = eType; }
	void Set_PetLevel(_uint iLevel) { m_iPetLevel = iLevel; }
	void Set_Select(_bool bIsSelect) { m_bIsSelect = bIsSelect; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT					Add_Component();
	HRESULT					SetUp_ConstantTable(_uint iIndex);

public:
	_bool					Pt_InRect();

private:
	CTransform*				m_pTransform = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	CTexture*				m_pTexture = nullptr;
	CShader*				m_pShader = nullptr;
	CBuffer_RcTex*			m_pBuffer = nullptr;

private:
	_bool					m_bIsCollMouse = false;
	_bool					m_bIsSelect = false;
	CPet::PET_TYPE			m_ePetType = CPet::PET_TYPE_END;
	_uint					m_iPetLevel = 1;

public:
	static CPet_Slot*			Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();

	/*CPet::PET_PLUS_TYPE		m_ePlusType = CPet::PET_PLUS_TYPE::PET_PLUS_END;
	CPet::PET_GRADE_TYPE	m_eGradeType = CPet::PET_GRADE_TYPE::PET_GRADE_TYPE_END;
	CPet::PET_TYPE			m_ePetType = CPet::PET_TYPE::PET_TYPE_END;*/
};

END

