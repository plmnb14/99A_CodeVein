#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CPlayerFontUI;
class CHazeUI final : public CUI
{
private:
	explicit CHazeUI(_Device pDevice);
	explicit CHazeUI(const CHazeUI& rhs);
	virtual ~CHazeUI() = default;

public:
	_ulong Get_Haze_Cnt() { return m_uHazeCnt; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_uint iIndex);
	HRESULT SetUp_Default();

public:
	// 인자 만큼 헤이즈 누적
	void Accumulate_Haze(_ulong uAccHazeCnt) { m_uHazeCnt += uAccHazeCnt; }

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	// 헤이즈 개수 표시하는 폰트
	CPlayerFontUI*			m_pHazeCntFont = nullptr;
	_ulong					m_uHazeCnt = 0;

public:
	static CHazeUI*			Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END
