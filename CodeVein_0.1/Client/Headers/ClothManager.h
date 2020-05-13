#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "Management.h"

BEGIN(Client)

class CClothManager final : CBase
{
	DECLARE_SINGLETON(CClothManager)

private:
	enum PlayerBone
	{
		Neck, Hips,
		LeftArm, LeftForeArm, LeftHand,
		RightArm, RightForeArm, RightHand,
		LeftUpLeg, LeftLeg, LeftFoot,
		RightUpLeg, RightLeg, RightFoot,
		PlayerBone_End
	};

public:
	enum Cloth_Static 
	{ 
		Hair01, Hair02, Hair03, Hair04, Hair05, Hair06, Hair07, Static_End 
	};

	enum Cloth_Dynamic 
	{
		Drape, LongCoat, Gauntlet, Muffler, Dynamic_End
	};

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
	HRESULT Ready_Cloth_Static();
	HRESULT Ready_Cloth_Dynamic();
	HRESULT Ready_matPlayerBone();

private:
	physx::PxClothMeshDesc CreateMesh(LPD3DXMESH pMesh, physx::PxReal scale, physx::PxQuat rot, physx::PxVec3 offset,
		vector<physx::PxVec4>& vertices, vector<physx::PxU16>& indices);

	physx::PxCloth* CreateQuadifier(physx::PxClothMeshDesc& meshDesc);

	void Update_Hair_ColPos(physx::PxCloth* pCloth);

private:
	// 스테틱 메쉬
	void Init_Hair01(physx::PxClothMeshDesc& pClothMeshDesc, vector<physx::PxVec4>& vertices);
	void Init_Hair02(physx::PxClothMeshDesc& pClothMeshDesc, vector<physx::PxVec4>& vertices);
	void Init_Hair03(physx::PxClothMeshDesc& pClothMeshDesc, vector<physx::PxVec4>& vertices);
	void Init_Hair04(physx::PxClothMeshDesc& pClothMeshDesc, vector<physx::PxVec4>& vertices);
	void Init_Hair05(physx::PxClothMeshDesc& pClothMeshDesc, vector<physx::PxVec4>& vertices);
	void Init_Hair06(physx::PxClothMeshDesc& pClothMeshDesc, vector<physx::PxVec4>& vertices);
	void Init_Hair07(physx::PxClothMeshDesc& pClothMeshDesc, vector<physx::PxVec4>& vertices);

	// 다이나믹 메쉬
	void Init_Drape(physx::PxClothMeshDesc& pClothMeshDesc, vector<physx::PxVec4>& vertices);
	void Init_LongCoat(physx::PxClothMeshDesc& pClothMeshDesc, vector<physx::PxVec4>& vertices);
	void Init_Gauntlet(physx::PxClothMeshDesc& pClothMeshDesc, vector<physx::PxVec4>& vertices);
	void Init_Muffler(physx::PxClothMeshDesc& pClothMeshDesc, vector<physx::PxVec4>& vertices);

private:
	physx::PxCloth*			m_pCloth_Static[Static_End] = { 0, };
	physx::PxCloth*			m_pCloth_Dynamic[Dynamic_End] = { 0, };

private:
	D3DXFRAME_DERIVED*		m_pPlayerFrame[PlayerBone_End] = { 0, };

public:
	virtual void Free();
};

END