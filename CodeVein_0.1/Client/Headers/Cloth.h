#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CCloth abstract : public CGameObject
{
protected:
	explicit CCloth(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CCloth(const CCloth& rhs);
	virtual ~CCloth() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() PURE;
	virtual HRESULT Ready_GameObject(void* pArg) PURE;
	virtual _int Update_GameObject(_double TimeDelta) PURE;
	virtual _int Late_Update_GameObject(_double TimeDelta) PURE;
	virtual HRESULT Render_GameObject() PURE;
	virtual HRESULT Render_GameObject_SetPass(CShader* pShader, _int iPass, _bool _bIsForMotionBlur = false) PURE ;

public:
	void Set_Wind();

protected:
	void Set_Static_Vertex_W(void* pArg, vector<physx::PxVec4>& vertices);
	void Set_Dynamic_Vertex_W(void* pArg, vector<physx::PxVec4>& vertices);

protected:
	physx::PxClothMeshDesc CreateMesh(physx::PxReal scale, physx::PxQuat rot, physx::PxVec3 offset,
		vector<physx::PxVec4>& vertices, vector<physx::PxU16>& indices);



protected:
	physx::PxCloth*		m_pCloth = nullptr;

	LPD3DXMESH			m_pMesh = nullptr;

	// ¹Ù¶÷
	_float				m_fWindStrength = 20.0f;
	_v3					m_vWindDir = { 1.f, 0.f, 0.f };

protected:
	virtual CGameObject* Clone_GameObject(void* pArg) PURE;
	virtual void Free();
};

END