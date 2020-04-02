#pragma once
#include "Client_Defines.h"
#include "UI.h"
#include "Management.h"

#include "Material.h"
#include "Select_UI.h"

BEGIN(Client)
class CMaterial_Slot final : public CUI
{
private:
	explicit CMaterial_Slot(_Device pDevice);
	explicit CMaterial_Slot(const CMaterial_Slot& rhs);
	virtual ~CMaterial_Slot() = default;

public:
	vector<CMaterial*> Get_Expendables() { return m_vecMaterial; }
	_bool Get_Select() { return m_bIsSelect; }
	void Set_Select(_bool bIsSelect) { m_bIsSelect = bIsSelect; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT			Add_Component();
	HRESULT			SetUp_ConstantTable();

public:
	void Input_Item(CMaterial* pMaterial);
	void Input_Items(vector<CMaterial*> vecMaterial) { m_vecMaterial = vecMaterial; }
	_bool Pt_InRect();
	_uint Get_Size();
	_uint Get_Type();
	void Delete_Item();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

	CSelect_UI*				m_pSelectUI = nullptr;
	_bool					m_bIsSelect = false;
	vector<CMaterial*>		m_vecMaterial;

public:
	static CMaterial_Slot*		Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};

END

