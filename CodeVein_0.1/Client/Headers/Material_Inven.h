#pragma once

#include "Client_Defines.h"
#include "UI.h"

#include "Material_Slot.h"

BEGIN(Client)
class CExplainMaterialUI;
class CMaterial_Inven final : public CUI
{
private:
	explicit CMaterial_Inven(_Device pDevice);
	explicit CMaterial_Inven(const CMaterial_Inven& rhs);
	virtual ~CMaterial_Inven() = default;

public:
	vector<CMaterial_Slot*>* Get_VecMaterialSlot() { return &m_vecMaterialSlot; }

public:
	virtual HRESULT			Ready_GameObject_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(_double TimeDelta);
	virtual _int			Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT			Render_GameObject();

private:
	HRESULT					Add_Component();
	HRESULT					SetUp_ConstantTable();
	void					Load_Materials(CMaterial* pMaterial, _uint iIndex = 0);
	void					Click_Inven(); // 활성화일 경우에만 적용됨
	void					SetUp_Default();
	void					Add_Slot();
	void					SetUp_SlotPos();
	void					SetUp_SubUI_Active(_bool bIsActive);

public:
	void Add_Material(CMaterial::MATERIAL_TYPE eType);
	void Add_MultiMaterial(CMaterial::MATERIAL_TYPE eType, _uint iCnt);
	void Sell_Material(_uint iDelete);
	void Sell_Item(CMaterial_Slot* pSlot);

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	vector<CMaterial*>		m_vecMaterial;
	vector<CMaterial_Slot*>	m_vecMaterialSlot;
	vector<UI_DESC*>		m_vecUI_DESC;
	CExplainMaterialUI*		m_pExplainUI = nullptr;
	_bool					m_bIsSubActive = false;

public:
	static CMaterial_Inven*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END
