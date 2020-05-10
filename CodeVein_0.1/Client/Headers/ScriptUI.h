#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CScriptUI : public CUI
{
public:
	// 말하는 대상
	enum Script_Talker
	{
		Talker_Player,
		Talker_Jack,
		Talker_End
	};
	// 말하는 대사 (Texture - 인덱스 설정시 사용)
	enum Script_Number
	{
		Jack_Player_Test1,
		Jack_Player_Test2,
		Script_End
	};
private:
	explicit CScriptUI(_Device pDevice);
	explicit CScriptUI(const CScriptUI& rhs);
	virtual ~CScriptUI() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_uint iIndex);
	void	SetUp_Default();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	_float					m_fLifeTime = 0.f;

public:
	static CScriptUI*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END
