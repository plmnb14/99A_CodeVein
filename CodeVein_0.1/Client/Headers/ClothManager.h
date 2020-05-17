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
		Hair_01, Hair_02, Hair_03/*긴머리*/, Hair_04, Hair_05, Hair_06, Hair_07, Static_End 
	};

	enum Cloth_Dynamic 
	{
		None,
		Drape_01,
		Gauntlet_01, Gauntlet_03,
		LongCoat_01, LongCoat_02, LongCoat_03,
		Muffler_01, Muffler_02,
		Dynamic_End
	};

private:
	explicit CClothManager();
	virtual ~CClothManager() = default;

public:
	HRESULT Ready_ClothManager();

	HRESULT Update_Cloth_Static(Cloth_Static eTag, _bool bCanCol);
	HRESULT Update_Cloth_Dynamic(Cloth_Dynamic eTag, _bool bCanCol);

public:
	physx::PxCloth* Get_Cloth_Static(Cloth_Static eStatic);
	physx::PxCloth* Get_Cloth_Dynamic(Cloth_Dynamic eDynamic);

	void Set_Sleep_AllStatic();
	void Set_Sleep_AllDynamic();

public:
	_bool Is_Valid_Static(Cloth_Static eStatic) { return m_bValid_CLoth_Static[eStatic]; }
	_bool Is_Valid_Dynamic(Cloth_Dynamic eDynamic) { return m_bValid_CLoth_Dynamic[eDynamic]; }

	void Clear_Collider(physx::PxCloth* pCloth);

private:
	HRESULT Ready_Cloth_Static();
	HRESULT Ready_Cloth_Dynamic();
	HRESULT Ready_matPlayerBone();
	HRESULT Ready_ColRadius();
	void Set_Cloth_Default(physx::PxCloth* pCloth);

	void Set_Wind(physx::PxCloth* pCloth, _v3 vWindDir);

private:
	physx::PxClothMeshDesc CreateMesh(LPD3DXMESH pMesh, physx::PxReal scale, physx::PxQuat rot, physx::PxVec3 offset,
		vector<physx::PxVec4>& vertices, vector<physx::PxU16>& indices);

	physx::PxCloth* CreateQuadifier(physx::PxClothMeshDesc& meshDesc);

	void Update_Hair_ColPos(physx::PxCloth* pCloth);
	void Update_Cloth_ColPos(physx::PxCloth* pCloth);

private:
	void Update_CurRadius();

	void Reset_CurRadius();

private:
	// 스테틱 메쉬
	void Init_Hair01(physx::PxClothMeshDesc& pClothMeshDesc, vector<physx::PxVec4>& vertices, CMesh_Static* pStaticMeshCom);
	void Init_Hair02(physx::PxClothMeshDesc& pClothMeshDesc, vector<physx::PxVec4>& vertices, CMesh_Static* pStaticMeshCom);
	void Init_Hair03(physx::PxClothMeshDesc& pClothMeshDesc, vector<physx::PxVec4>& vertices, CMesh_Static* pStaticMeshCom);
	void Init_Hair04(physx::PxClothMeshDesc& pClothMeshDesc, vector<physx::PxVec4>& vertices, CMesh_Static* pStaticMeshCom);
	void Init_Hair05(physx::PxClothMeshDesc& pClothMeshDesc, vector<physx::PxVec4>& vertices, CMesh_Static* pStaticMeshCom);
	void Init_Hair06(physx::PxClothMeshDesc& pClothMeshDesc, vector<physx::PxVec4>& vertices, CMesh_Static* pStaticMeshCom);
	void Init_Hair07(physx::PxClothMeshDesc& pClothMeshDesc, vector<physx::PxVec4>& vertices, CMesh_Static* pStaticMeshCom);

	// 다이나믹 메쉬
	void Init_Drape_01(physx::PxClothMeshDesc& pClothMeshDesc, vector<physx::PxVec4>& vertices);
	void Init_Gauntlet_01(physx::PxClothMeshDesc& pClothMeshDesc, vector<physx::PxVec4>& vertices);
	void Init_Gauntlet_03(physx::PxClothMeshDesc& pClothMeshDesc, vector<physx::PxVec4>& vertices);
	
	void Init_LongCoat_01(physx::PxClothMeshDesc& pClothMeshDesc, vector<physx::PxVec4>& vertices);
	void Init_LongCoat_02(physx::PxClothMeshDesc& pClothMeshDesc, vector<physx::PxVec4>& vertices);
	void Init_LongCoat_03(physx::PxClothMeshDesc& pClothMeshDesc, vector<physx::PxVec4>& vertices);
	
	void Init_Muffler_01(physx::PxClothMeshDesc& pClothMeshDesc, vector<physx::PxVec4>& vertices);
	void Init_Muffler_02(physx::PxClothMeshDesc& pClothMeshDesc, vector<physx::PxVec4>& vertices);

private:
	physx::PxCloth*			m_pCloth_Static[Static_End] = { 0, };
	physx::PxCloth*			m_pCloth_Dynamic[Dynamic_End] = { 0, };

	_bool					m_bValid_CLoth_Static[Static_End] = { false, };
	_bool					m_bValid_CLoth_Dynamic[Dynamic_End] = { false, };


private:
	D3DXFRAME_DERIVED*		m_pPlayerFrame[PlayerBone_End] = { 0, };
	_float					m_fCurRadius[PlayerBone_End] = { 0, };
	_float					m_fMaxRadius[PlayerBone_End] = { 0, };

public:
	virtual void Free();
};

END