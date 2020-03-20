#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Management.h"

class CItemIcon : public CUI
{
private:
	explicit CItemIcon(_Device pGraphic_Device);
	explicit CItemIcon(const CItemIcon& rhs);
	virtual ~CItemIcon() = default;

public:
	_uint Get_Index() { return m_iIndex; }
	_uint Get_ItemCnt() { return m_iItemCount; }

public:
	void Set_Index(_uint iIndex) { m_iIndex = iIndex; }
	void Set_ItemCnt(_uint iItemCnt) { m_iItemCount = iItemCnt; }

public:
	virtual HRESULT			Ready_GameObject_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(_double TimeDelta);
	virtual _int			Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT			Render_GameObject();

private:
	HRESULT					Add_Component();
	HRESULT					SetUp_ConstantTable();

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	
	_uint					m_iIndex = 0;
	_uint					m_iItemCount = 0;

public:
	static CItemIcon*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

