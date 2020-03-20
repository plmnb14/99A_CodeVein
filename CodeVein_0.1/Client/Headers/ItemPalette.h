#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Management.h"

#include "ItemIcon.h"

class CItemPalette : public CUI
{

private:
	
	explicit CItemPalette(_Device pGraphic_Device);
	explicit CItemPalette(const CItemPalette& rhs);
	virtual ~CItemPalette() = default;

public:
	_uint Get_ItemIndex(_uint iPaletteIndex);

public:
	virtual HRESULT			Ready_GameObject_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(_double TimeDelta);
	virtual _int			Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT			Render_GameObject();

private:
	HRESULT					Add_Component();
	HRESULT					SetUp_ConstantTable();
	void					SetUp_WindowPosition();
	
public:
	void Add_ItemIcon(const _tchar* pPrototypeTag, _uint iSceneID, const _tchar* pLayerTag, _uint iItemIndex);

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

	_bool					m_bIsOpenWindow = false;
	_uint					m_iMaxCnt = 6;
	
	vector<CItemIcon*>		m_vecItemIcon;
	
public:
	static CItemPalette*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();


};

