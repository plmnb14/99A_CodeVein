#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "Management.h"

BEGIN(Client)

class CClothManager final : CBase
{
	DECLARE_SINGLETON(CClothManager)

public:
	enum Cloth_Static { Hair01, Static_End };
	enum Cloth_Dynamic { Muffler, Dynamic_End };

private:
	explicit CClothManager();
	virtual ~CClothManager() = default;

public:
	HRESULT Ready_ClothManager();

	HRESULT Update_Cloth_Static(Cloth_Static eTag);
	HRESULT Update_Cloth_Dynamic(Cloth_Dynamic eTag);

public:
	physx::PxCloth* Get_Cloth_Static(Cloth_Static eStatic);
	physx::PxCloth* Get_Cloth_Dynamic(Cloth_Dynamic eDynamic);

private:
	physx::PxClothMeshDesc CreateMesh(LPD3DXMESH pMesh, physx::PxReal scale, physx::PxQuat rot, physx::PxVec3 offset,
		vector<physx::PxVec4>& vertices, vector<physx::PxU16>& indices);

	physx::PxCloth* CreateQuadifier(physx::PxClothMeshDesc& meshDesc);

private:
	// 스테틱 메쉬
	void Init_HairLong(physx::PxClothMeshDesc& pClothMeshDesc, vector<physx::PxVec4>& vertices);
	void Init_HairPonyTail();

	// 다이나믹 메쉬
	void Init_Muffler();

private:
	physx::PxCloth*		m_pCloth_Static[Static_End] = { 0, };
	physx::PxCloth*		m_pCloth_Dynamic[Dynamic_End] = { 0, };

public:
	virtual void Free();
};

END