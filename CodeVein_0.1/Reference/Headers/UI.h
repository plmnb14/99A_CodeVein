#pragma once

#include "Management.h"
#include "GameObject.h"
#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CUI : public CGameObject
{
public:
	typedef struct tagUI_DESC
	{
		_float					fPosX, fPosY;
		_float					fSizeX, fSizeY;
		_uint					iIndex = 0;
	}UI_DESC;

protected:
	explicit CUI(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CUI(const CUI& rhs);
	virtual ~CUI() = default;

public:
	_v2 Get_UI_Pos() { return _v2(m_fPosX, m_fPosY); }
	_v2 Get_UI_Size() { return _v2(m_fSizeX, m_fSizeY); }
	_uint Get_UI_Index() { return m_iIndex; }
	_bool Get_Active() { return m_bIsActive; }

public:
	void Set_UI_Pos(_float fPosX, _float fPosY);
	void Set_UI_Size(_float fSizeX, _float fSizeY);
	void Set_UI_Index(_uint iIndex) { m_iIndex = iIndex; }
	void Set_ViewZ(_float fViewZ) { m_fViewZ = fViewZ; }
	void Set_Active(_bool bIsActive) { m_bIsActive = bIsActive; }

public:
	virtual HRESULT Ready_GameObject_Prototype(); // 원복객체 생성 시, 호출될 함수.
	virtual HRESULT Ready_GameObject(void* pArg); // 인게임 객체 생성 시, 호출될 함수.
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

protected:
	_mat					m_matProj;
	_mat					m_matWorld;
	_mat					m_matView;
	_mat					m_matOldView, m_matOldProj;

	_float					m_fPosX = 0.f, m_fPosY = 0.f;
	_float					m_fSizeX = 0.f, m_fSizeY = 0.f;
	_uint					m_iIndex = 0;
	
	UI_DESC*				m_pUIDesc = nullptr;
	_bool					m_bIsActive = false;

public:
	virtual CGameObject*	Clone_GameObject(void* pArg) = 0; // 복사객체 생성.
	virtual void			Free();
};

END