#pragma once


#include "Client_Defines.h"
#include "UI.h"
#include "Management.h"

class CItemSlot : public CUI
{
private:
	explicit CItemSlot(_Device pGraphic_Device);
	explicit CItemSlot(const CItemSlot& rhs);
	virtual ~CItemSlot() = default;

public:
	_uint Get_Index() { return m_iIndex; }
	_uint Get_MaxIndex() { return m_iMaxIndex; }
	_uint Get_ItemCount() { return m_iItemCnt; }

public:
	void Set_Index(_uint iIndex);

public:
	virtual HRESULT			Ready_GameObject_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(_double TimeDelta);
	virtual _int			Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT			Render_GameObject();

private:
	HRESULT					Add_Component();
	HRESULT					SetUp_ConstantTable();
	void					Click_ItemSlot();

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

	_uint					m_iIndex = 0;
	_uint					m_iMaxIndex = 8;

	_uint					m_iTexNum = 0;
	_uint					m_iItemCnt = 0;

public:
	static CItemSlot*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};