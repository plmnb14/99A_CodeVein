#pragma once

#include "Client_Defines.h"
#include "Management.h"
#include "UI.h"

BEGIN(Client)

class CLockOn_UI final : public CUI
{
private:
	explicit CLockOn_UI(_Device Graphic_Device);
	explicit CLockOn_UI(const CLockOn_UI& rhs);
	virtual ~CLockOn_UI() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual	HRESULT	LateInit_GameObject();
	virtual HRESULT Render_GameObject();

public:
	void		Set_Bonmatrix(_mat* _Bonmatrix) { m_pTargetBonemat = _Bonmatrix; }
	void		Set_TargetWorldmatrix(_mat* _matWorld) { m_pTargetWorldmat = _matWorld; }

public:
	void		Caculate_World();
	_mat		Caculate_Billboard();
	_v3			Caculate_Pos(_mat _matrix);

public:
	void		Reset_TargetMatrix();

private:
	_mat*					m_pTargetBonemat = nullptr;
	_mat*					m_pTargetWorldmat = nullptr;

private:
	_mat					m_matWorld;

private:
	CTransform*				m_pTransform = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	CTexture*				m_pTexture = nullptr;
	CShader*				m_pShader = nullptr;
	CBuffer_RcTex*			m_pBuffer = nullptr;

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_uint TextureIndex);

public:
	static CLockOn_UI*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END