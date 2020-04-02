#pragma once

#include "Client_Defines.h"
#include "UI.h"

#include "Material_Slot.h"

BEGIN(Client)

class CMaterial_Inven final : public CUI
{
private:
	explicit CMaterial_Inven(_Device pDevice);
	explicit CMaterial_Inven(const CMaterial_Inven& rhs);
	virtual ~CMaterial_Inven() = default;



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

public:
	void Add_Material(CMaterial::MATERIAL_TYPE eType);
	void Sell_Material(_uint iDelete);

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	_bool					m_bIsActive;
	vector<CMaterial*>		m_vecMaterial;
	vector<CMaterial_Slot*>	m_vecMaterialSlot;

public:
	static CMaterial_Inven*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END
