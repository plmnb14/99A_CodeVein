#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Management.h"

class CItem : public CUI
{
public:
	enum ITEM_TYPE
	{
		ITEM_NONE,
		REGEN_POWER, // 재생력
		ITEM_2,
		ITEM_3,
		ITEM_END
	};
	typedef struct tagItemInfo
	{
		_int iHP; //체력
		_int iSP; //스태미너
		ITEM_TYPE eType;
	}ITEM_INFO;
private:
	explicit CItem(_Device pGraphic_Device);
	explicit CItem(const CItem& rhs);
	virtual ~CItem() = default;

//public:
//	_uint Get_Index() { return m_iIndex; }
//	_uint Get_ItemCnt() { return m_iItemCount; }

public:
	ITEM_INFO& Get_Info() { return m_tInfo; }
	ITEM_TYPE Get_Type() { return m_tInfo.eType; }

public:
	void Set_Info(ITEM_INFO tInfo) { m_tInfo = tInfo; }
	void Set_Type(ITEM_TYPE eType) { m_tInfo.eType = eType; }

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
	
	ITEM_INFO				m_tInfo;

public:
	static CItem*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

