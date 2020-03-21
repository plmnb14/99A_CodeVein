#pragma once

#include "GameObject.h"
#include "Management.h"

BEGIN(Engine)

class ENGINE_DLL CMeshEffect final : public CGameObject
{
public:

protected:
	explicit CMeshEffect(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CMeshEffect(const CMeshEffect& rhs);
	virtual ~CMeshEffect() = default;

public:
	EFFECT_INFO* Get_Info() { return m_pInfo; }

public:
	virtual HRESULT Ready_GameObject_Prototype(); // 원복객체 생성 시, 호출될 함수.
	virtual HRESULT Ready_GameObject(void* pArg); // 인게임 객체 생성 시, 호출될 함수.
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual	HRESULT LateInit_GameObject();
	virtual HRESULT Render_GameObject();
private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CMesh_Static*			m_pMeshCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

private:
	EFFECT_INFO*			m_pInfo = nullptr;

	_float					m_fAlpha = 1.f;
	_float					m_fLifeTime = 0.f;
	_float					m_fMoveSpeed = 0.f;
	_float					m_fLinearMoveSpeed = 0.f;
	_float					m_fLinearMovePercent = 0.f;
	_float					m_fRotSpeed = 0.f;
	_float					m_fAlphaSpeed = 0.f;
	_float					m_fCreateDelay = 0.f;
	_v3						m_vLerpPos = { 1.f, 1.f, 1.f };
	_v3						m_vLerpScale = { 1.f, 1.f, 1.f };
	_v3						m_vDir = { 1.f, 1.f, 1.f };
	_v3						m_vRot = { 0.f, 0.f, 0.f };
	_v3						m_vFollowPos = { 1.f, 1.f, 1.f };
	_v4						m_vColor = { 1.f, 1.f, 1.f, 1.f };

	_bool					m_bFadeOutStart = false;
	_bool					m_bClone = false;

private:
	void Check_Move(_double TimeDelta);
	void Check_LifeTime(_double TimeDelta);
	void Check_Alpha(_double TimeDelta);
	void Check_Color(_double TimeDelta);
	void Check_CreateDelay(_double TimeDelta);

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	void Change_Texture(const _tchar* _Name);
	void Change_Mesh(const _tchar* _Name);

public:
	static CMeshEffect* Create(LPDIRECT3DDEVICE9 pGraphic_Device); // 원형객체 생성.
	static CMeshEffect* Create(LPDIRECT3DDEVICE9 pGraphic_Device, EFFECT_INFO* pInfo);

	virtual CGameObject* Clone_GameObject(void* pArg); // 복사객체 생성.
	virtual void Free();
};

END