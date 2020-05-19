#pragma once

#include "Management.h"
#include "UI.h"
#include "Client_Defines.h"


BEGIN(Client)
class CPlayer;
class CCursorEffect;
class CCustomSliderBar final : public CUI
{
public:
	enum SLIDER_TYPE
	{
		TYPE_HAIR_HUE,
		TYPE_HAIR_CONTRAST,
		TYPE_HAIR_BRIGHTNESS,
		TYPE_HAIR_SATURATION,

		TYPE_END
	};
private:
	explicit CCustomSliderBar(_Device pGraphic_Device);
	explicit CCustomSliderBar(const CCustomSliderBar& rhs);
	virtual ~CCustomSliderBar() = default;

public:
	_bool Get_IsColl() { return m_bIsColl; }

public:
	void Set_Type(SLIDER_TYPE eType) { m_eType = eType; }
	void Set_UI_Pos(_float fX, _float fY);

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_uint iIndex);
	HRESULT SetUp_CursorEffect();

	void	Late_Init();
	void	Check_Drag();
	void	Check_Value();

public:
	_bool Coll_Mouse();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CPlayer*				m_pPlayer = nullptr;

	_bool					m_bLateInit = false;
	_bool					m_bIsColl = false;
	_bool					m_bIsSelect = false;

	_bool					m_bDragStart = false;

	_float					m_fBackBarPosX = 0.f;
	_float					m_fBackBarPosY = 0.f;
	_float					m_fControlBarPosX = 0.f;
	_float					m_fControlBarPosY = 0.f;

	_float					m_fMaxLen = 0.f;
	_float					m_fMinLen = 0.f;

	_float					m_fValue = 0.f;
	SLIDER_TYPE				m_eType = TYPE_END;

public:
	static CCustomSliderBar*	Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};

END

