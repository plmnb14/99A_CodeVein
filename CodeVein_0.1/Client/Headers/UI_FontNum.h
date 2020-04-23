#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CUI_FontNum final : public CUI
{
public:
	typedef enum FontType
	{
		Billboard_UI , Orthgrahpuic_UI
	}F_TYPE;

private:
	explicit CUI_FontNum(_Device pDevice);
	explicit CUI_FontNum(const CUI_FontNum& rhs);
	virtual ~CUI_FontNum() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	virtual void Set_Type(F_TYPE _eType) { m_eType = _eType; } 

public:
	virtual void Update_Number();

private:
	CTransform*				m_pTransform = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	CTexture*				m_pTexture = nullptr;
	CShader*				m_pShader = nullptr;
	CBuffer_RcTex*			m_pBuffer = nullptr;

private:
	F_TYPE					m_eType = Billboard_UI;

private:
	_float					m_fValue = 0.f;

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

private:
	_int	Calc_Interval(_float _fValue);
	_int	Calc_LoopCnt(_float _fValue);
	_int	Calc_NumTexIdx(_float _fValue , _int _iLoopCnt);

private:
	HRESULT Update_Matrix();

public:
	static CUI_FontNum*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END