#pragma once

#include "Client_Defines.h"
#include "Management.h"
#include "UI.h"

BEGIN(Client)

class CPickUp_ItemUI;
class CPickUp_ItemUIManager final : public CUI
{

private:
	explicit CPickUp_ItemUIManager(_Device Graphic_Device);
	explicit CPickUp_ItemUIManager(const CPickUp_ItemUIManager& rhs);
	virtual ~CPickUp_ItemUIManager() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual	HRESULT	LateInit_GameObject();
	virtual HRESULT Render_GameObject();

public:
	_uint	Get_Count_GetItem() { return m_iCount_GetItem; }

	vector<CPickUp_ItemUI*>&	Get_vecPickUp_Item() { return m_vecPickUp_Item; }

public:
	void	Set_Count_GetItem(_uint _Count) { m_iCount_GetItem = _Count; }

	void	Set_vecPickUp_Item(CPickUp_ItemUI* _vecPickUP);

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_uint TextureIndex);
	void	SetUp_State(_double TimeDelta);
	void	SetUp_Rendering_ItemTextrue();

private:
	vector<CPickUp_ItemUI*>	m_vecPickUp_Item;

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	_uint	m_iCount_GetItem = 0;

public:
	static	CPickUp_ItemUIManager*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END