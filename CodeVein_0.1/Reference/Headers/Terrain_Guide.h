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
	virtual HRESULT Ready_GameObject_Prototype(); // ������ü ���� ��, ȣ��� �Լ�.
	virtual HRESULT Ready_GameObject(void* pArg); // �ΰ��� ��ü ���� ��, ȣ��� �Լ�.
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
	static CTerrain_Guide* Create(_Device pGraphic_Device); // ������ü ����.
	virtual CGameObject* Clone_GameObject(void* pArg); // ���簴ü ����.
	virtual void Free();
};

END