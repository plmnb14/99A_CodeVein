#include "stdafx.h"
#include "..\Headers\Cloth.h"
#include "..\Headers\ClothDynamic.h"
#include "..\Headers\ClothStatic.h"

using namespace physx;

CCloth::CCloth(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CCloth::CCloth(const CCloth & rhs)
	: CGameObject(rhs)
{
}

void CCloth::Set_Wind()
{
	// 자연스러운 바람.sample
	PxReal strength = 5.0f;
	PxVec3 offset(PxReal(CALC::Random_Num_Double(-1, 1)), 1.0f + PxReal(CALC::Random_Num_Double(-1, 1)), PxReal(CALC::Random_Num_Double(-1, 1)));
	PxVec3 windAcceleration = strength * offset;
	m_pCloth->setExternalAcceleration(windAcceleration);

	//PxVec3 windAcceleration = *(PxVec3*)&(m_fWindStrength * m_vWindDir);
	//m_pCloth->setExternalAcceleration(windAcceleration);
}

void CCloth::Set_Static_Vertex_W(void * pArg, vector<PxVec4>& vertices)
{
	CClothStatic::INFO Temp = *(CClothStatic::INFO*)pArg;

	switch (Temp.eName)
	{
	case CClothStatic::Hair_Long:
		/*
		Attribld		0
		VertexStart	0
		VertexCount	4241
		*/

		// 정확한 수치는 구해야함
		for (PxU32 i = 0; i < 4241; i++)
		{
			if (vertices[i].y < 1.5f)
				vertices[i].w = 0.2f;
		}

		break;
	case CClothStatic::Hair_Ponytail:
		/*
		Attribld		0
		VertexStart	0
		VertexCount	9821
		*/

		// 정확한 수치는 구해야함
		// Long Hair
		for (PxU32 i = 0; i < 9821; i++)
		{
			if (vertices[i].y < 1.55f &&
				vertices[i].z < -0.05f)
				vertices[i].w = 0.1f;
		}

		break;
	}

}

void CCloth::Set_Dynamic_Vertex_W(void * pArg, vector<PxVec4>& vertices)
{
	CClothDynamic::INFO Temp = *(CClothDynamic::INFO*)pArg;

	switch (Temp.eName)
	{
	case CClothDynamic::Drape:
		/*
		Attribld		0
		VertexStart	0
		VertexCount	12723

		Attribld		1
		VertexStart	12723
		VertexCount	2483

		Attribld		2
		VertexStart	15206
		VertexCount	2483
		*/

		// 1, 2번 서브셋
		for (PxU32 i = 12723; i < 17689; i++)
		{
			if (vertices[i].y < 1.2f)
				vertices[i].w = 0.2f;
		}

		break;
	case CClothDynamic::Gauntlet:
		/*
		Attribld		0
		VertexStart	0
		VertexCount	11533

		Attribld		1
		VertexStart	11533
		VertexCount	1404

		Attribld		2
		VertexStart	12937
		VertexCount	1404
		*/

		// Gauntlet
		// 1,2 번 서브셋
		for (PxU32 i = 11533; i < 14341; i++)
		{
			if (vertices[i].y < 1.3f)
				vertices[i].w = 1.f;
		}

		break;
	case CClothDynamic::Muffler:
		/*
		Attribld		0
		VertexStart	0
		VertexCount	12570

		Attribld		1
		VertexStart	12570
		VertexCount	912

		Attribld		2
		VertexStart	13482
		VertexCount	912
		*/

		// Cloth_muffler
		// 0번 서브셋
		for (PxU32 i = 0; i < 12570; i++)
		{
			if (vertices[i].y < 1.25f)
				vertices[i].w = 0.2f;
		}

		break;
	case CClothDynamic::LongCoat:
		/*
		Attribld		0
		VertexStart	0
		VertexCount	11855

		Attribld		1 옷 끝자락 천
		VertexStart	11855
		VertexCount	2454

		Attribld		2 벨트
		VertexStart	14309
		VertexCount	1384

		Attribld		3 옷 끝자락 천
		VertexStart	15693
		VertexCount	2454

		Attribld		4 벨트
		VertexStart	18147
		VertexCount	1384
		*/

		//  1번 서브셋
		for (PxU32 i = 11855; i < 14309; i++)
		{
			if (vertices[i].y < 0.7f)
				vertices[i].w = 0.1f;
		}

		// 3번 서브셋
		for (PxU32 i = 15693; i < 18147; i++)
		{
			if (vertices[i].y < 0.7f)
				vertices[i].w = 0.1f;
		}


		break;
	default:
		break;
	}
}

PxClothMeshDesc CCloth::CreateMesh(PxReal scale, PxQuat rot, PxVec3 offset, vector<PxVec4>& vertices, vector<PxU16>& indices)
{
	if (nullptr == m_pMesh)
	{
		MSG_BOX("CCloth : n_pMesh is Nullptr");
		return PxClothMeshDesc();
	}

	_ulong numVertices = m_pMesh->GetNumVertices();
	_ulong numTriangles = m_pMesh->GetNumFaces();

	vertices.resize(numVertices);
	indices.resize(numTriangles * 3);

	vector<PxVec3> verticesTemp;
	verticesTemp.resize(numVertices);

	////////////////////////////////////////////////////
	// 버텍스 포지션 정보를 vec3 에서 vec4로 바꿔서 저장
	//IDirect3DVertexBuffer9* pVB = nullptr;
	_byte*		pVertices = nullptr;

	//m_pMesh->GetVertexBuffer(&pVB);
	//pVB->Lock(0, 0, (void**)&pVertices, 0);
	m_pMesh->LockVertexBuffer(0, (void**)&pVertices);

	_ulong dwStride = m_pMesh->GetNumBytesPerVertex();

	for (_ulong i = 0; i < numVertices; ++i)
	{
		PxVec3 vTemp = *(PxVec3*)(pVertices + (i * dwStride));
		verticesTemp[i] = vTemp;
	}

	PxVec3 *vSrc = (PxVec3*)&(*verticesTemp.begin());
	PxVec4 *vDest = &(*vertices.begin());
	for (_ulong i = 0; i < numVertices; i++, vDest++, vSrc++)
	{
		*vDest = PxVec4(scale * rot.rotate(*vSrc) + offset, 0.f);
	}

	//pVB->Unlock();
	m_pMesh->UnlockVertexBuffer();

	/////////////////////////////////////////////////////
	// 인덱스 정보 저장
	_byte *	pIndex = nullptr;

	m_pMesh->LockIndexBuffer(0, (void**)&pIndex);
	//pMesh->GetIndexBuffer(&m_pIB);
	//m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	for (_ulong i = 0; i < numTriangles * 3; ++i)
	{
		indices[i] = *((PxU16*)pIndex + i);
	}

	m_pMesh->UnlockIndexBuffer();


	PxClothMeshDesc meshDesc;

	// convert vertex array to PxBoundedData (desc.points)
	meshDesc.points.data = &(*vertices.begin());
	meshDesc.points.count = static_cast<PxU32>(numVertices);
	meshDesc.points.stride = sizeof(PxVec4);

	meshDesc.invMasses.data = &vertices.begin()->w;
	meshDesc.invMasses.count = static_cast<PxU32>(numVertices);
	meshDesc.invMasses.stride = sizeof(PxVec4);

	// convert face index array to PxBoundedData (desc.triangles)
	meshDesc.triangles.data = &(*indices.begin());
	meshDesc.triangles.count = static_cast<PxU32>(numTriangles);
	meshDesc.triangles.stride = sizeof(PxU16) * 3; // <- stride per triangle
	//meshDesc.triangles.stride = sizeof(PxU32) * 3; // <- stride per triangle

	meshDesc.flags = PxMeshFlag::e16_BIT_INDICES;

	return meshDesc;
}

void CCloth::Free()
{
	CGameObject::Free();
}
