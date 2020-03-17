#pragma once

#include "Engine_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Engine)

class ENGINE_DLL CTerrain_Guide : public CGameObject
{
protected:
	explicit CTerrain_Guide(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CTerrain_Guide(const CTerrain_Guide& rhs);
	virtual ~CTerrain_Guide() = default;
public:
	virtual HRESULT Ready_GameObject_Prototype(); // 원복객체 생성 시, 호출될 함수.
	virtual HRESULT Ready_GameObject(void* pArg); // 인게임 객체 생성 시, 호출될 함수.
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_Terrain*		m_pBufferCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CTerrain_Guide* Create(_Device pGraphic_Device); // 원형객체 생성.
	virtual CGameObject* Clone_GameObject(void* pArg); // 복사객체 생성.
	virtual void Free();
};

END