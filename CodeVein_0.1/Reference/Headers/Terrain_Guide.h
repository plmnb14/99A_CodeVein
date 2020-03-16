#pragma once

#include "Engine_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Engine)

class ENGINE_DLL CTerrain_Guide : public CGameObject
{
protected:
	explicit CTerrain_Guide(_Device pGraphic_Device);
	explicit CTerrain_Guide(const CTerrain_Guide& rhs);
	virtual ~CTerrain_Guide() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype(); // ������ü ���� ��, ȣ��� �Լ�.
	virtual HRESULT Ready_GameObject(void* pArg); // �ΰ��� ��ü ���� ��, ȣ��� �Լ�.
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	CTransform*				m_pTransform = nullptr;
	CShader*				m_pShader = nullptr;
	CBuffer_Terrain*		m_pBuffer = nullptr;
	CCollider*				m_pCollider = nullptr;

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
public:
	static CTerrain_Guide* Create(_Device pGraphic_Device); // ������ü ����.
	virtual CGameObject* Clone_GameObject(void* pArg); // ���簴ü ����.
	virtual void Free();
};

END