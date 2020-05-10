#include "stdafx.h"
#include "..\Headers\ClothDynamic.h"

using namespace physx;

CClothDynamic::CClothDynamic(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CCloth(pGraphic_Device)
{
}

CClothDynamic::CClothDynamic(const CClothDynamic & rhs)
	: CCloth(rhs)
{
}

HRESULT CClothDynamic::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CClothDynamic::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	PxScene& scene = *g_pPhysx->Get_Scene();
	PxPhysics& physics = *g_pPhysx->Get_Physics();

	PxSceneWriteLock scopedLock(scene);

	// create the cloth cape mesh from file
	vector<PxVec4> vertices;
	vector<PxU16> primitives;
	PxClothMeshDesc meshDesc = CreateMesh(1.f, PxQuat(PxIdentity), PxVec3(0, 0, 0), vertices, primitives);

	// �������� �ϴ��� �ǹ̸� �𸣰���.
	//for (PxU32 i = 0; i < meshDesc.invMasses.count; i++)
	//{
	//	((float*)meshDesc.invMasses.data)[i * 4] = 0.5f;
	//}

	if (!meshDesc.isValid())
		MSG_BOX(" PxCloth : Failed to Craete Mesh");

	////////////////////////////////////////
	// �޽��� ��鸱 �� ����.
	Set_Dynamic_Vertex_W(pArg, vertices);
	/////////////////////////////////////////

	PxClothMeshQuadifier quadifier(meshDesc);

	// Create Fabric
	PxClothFabric* clothFabric = PxClothFabricCreate(physics, quadifier.getDescriptor(), PxVec3(0, -1, 0));
	PX_ASSERT(quadifier.getDescriptor().points.stride == sizeof(PxVec4));

	// create the cloth
	const PxClothParticle* particles = (const PxClothParticle*)quadifier.getDescriptor().points.data;
	m_pCloth = physics.createCloth(PxTransform(PxVec3(0.f, 0.f, 0.f), PxQuat(PxIdentity)), *clothFabric, particles, PxClothFlags());
	PX_ASSERT(m_pCloth);

	scene.addActor(*m_pCloth);



	CClothDynamic::INFO Temp = *(CClothDynamic::INFO*)pArg;

	switch (Temp.eName)
	{
	case CClothDynamic::Drape:
		Init_Darpe();
		break;
	case CClothDynamic::Gauntlet:
		Init_Gauntlet();
		break;
	case CClothDynamic::LongCoat:
		Init_LongCoat();
		break;
	case CClothDynamic::Muffler:
		Init_Muffler();
		break;
	}

	

	m_pTransformCom->Set_Pos(_v3(0.f, 0.f, 0.f));
	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));

	return S_OK;
}

_int CClothDynamic::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	PxSceneWriteLock scopedLock(*g_pPhysx->Get_Scene());

	Set_Wind();

	//_v3 vPlayerPos = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL))->Get_Pos();

	//PxTransform rootPose = PxTransform(PxVec3(vPlayerPos.x, vPlayerPos.y, vPlayerPos.z));;
	//
	//// physx �ʿ� ��ǥ ����.
	//m_pCloth->setTargetPose(rootPose);

	//// d3d���� ��  physx���� ���� ��ǥ.
	//m_pTransformCom->Set_Pos(_v3(m_pCloth->getGlobalPose().p.x, m_pCloth->getGlobalPose().p.y, m_pCloth->getGlobalPose().p.z));


	return NO_EVENT;
}

_int CClothDynamic::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	if (nullptr == m_pRendererCom)
		return E_FAIL;

	//=============================================================================================
	// �׸��ڶ� ��Ǻ��� �������� �ȿ� ������ �ȱ׸�
	//=============================================================================================
	if (m_pOptimizationCom->Check_InFrustumforObject(&m_pTransformCom->Get_Pos(), 2.f))
	{
		if (!m_bDissolve)
		{
			if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
				return E_FAIL;
		}

		else
		{
			if (FAILED(m_pRendererCom->Add_RenderList(RENDER_ALPHA, this)))
				return E_FAIL;
		}

		if (FAILED(m_pRendererCom->Add_RenderList(RENDER_MOTIONBLURTARGET, this)))
			return E_FAIL;
		if (FAILED(m_pRendererCom->Add_RenderList(RENDER_SHADOWTARGET, this)))
			return E_FAIL;
	}
	//=============================================================================================

	return NO_EVENT;
}

HRESULT CClothDynamic::Render_GameObject()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pMeshCom)
		return E_FAIL;

	m_pMeshCom->Play_Animation(g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60")); // * alpha

	if (m_pOptimizationCom->Check_InFrustumforObject(&m_pTransformCom->Get_Pos(), 2.f))
	{
		// ���ؽ� ��ü
		PxSceneWriteLock scopedLock(*g_pPhysx->Get_Scene());

		PxClothFabric* pFabric = m_pCloth->getFabric();
		PxClothParticleData* pData = m_pCloth->lockParticleData();

		_ulong dwStride = m_pMesh->GetNumBytesPerVertex();

		_byte* pVertices = nullptr;

		m_pMesh->LockVertexBuffer(0, (void**)&pVertices);

		for (_ulong i = 0; i < _ulong(pFabric->getNbParticles()); ++i)
		{
			*(_v3*)(pVertices + (i * dwStride)) = *(_v3*)(pData->particles + i);
		}

		m_pMesh->UnlockVertexBuffer();
		/////////////////////////////////////////////////


		if (FAILED(SetUp_ConstantTable()))
			return E_FAIL;

		m_pShaderCom->Begin_Shader();

		_uint iNumMeshContainer = _uint(m_pMeshCom->Get_NumMeshContainer());

		for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
		{
			_uint iNumSubSet = (_uint)m_pMeshCom->Get_NumMaterials(i);

			m_pMeshCom->Update_SkinnedMesh(i);

			for (_uint j = 0; j < iNumSubSet; ++j)
			{
				m_iPass = m_pMeshCom->Get_MaterialPass(i, j);

				if (m_bDissolve)
					m_iPass = 3;

				m_pShaderCom->Begin_Pass(m_iPass);

				m_pShaderCom->Set_DynamicTexture_Auto(m_pMeshCom, i, j);

				m_pShaderCom->Commit_Changes();

				m_pMeshCom->Render_Mesh(i, j);

				m_pShaderCom->End_Pass();
			}
		}

		m_pShaderCom->End_Shader();
	}

	return S_OK;
}

HRESULT CClothDynamic::Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur)
{
	if (nullptr == pShader ||
		nullptr == m_pMeshCom)
		return E_FAIL;

	//============================================================================================
	// ���� ����
	//============================================================================================

	_mat	ViewMatrix = CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW);
	_mat	ProjMatrix = CManagement::Get_Instance()->Get_Transform(D3DTS_PROJECTION);
	_mat	WorldMatrix = m_pTransformCom->Get_WorldMat();

	if (FAILED(pShader->Set_Value("g_matWorld", &WorldMatrix, sizeof(_mat))))
		return E_FAIL;

	//============================================================================================
	// ��� �� ���
	//============================================================================================
	if (_bIsForMotionBlur)
	{
		if (FAILED(pShader->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(pShader->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(pShader->Set_Value("g_matLastWVP", &m_matLastWVP, sizeof(_mat))))
			return E_FAIL;

		m_matLastWVP = WorldMatrix * ViewMatrix * ProjMatrix;

		_bool bMotionBlur = true;
		if (FAILED(pShader->Set_Bool("g_bMotionBlur", bMotionBlur)))
			return E_FAIL;
		_bool bDecalTarget = false;
		if (FAILED(pShader->Set_Bool("g_bDecalTarget", bDecalTarget)))
			return E_FAIL;
		_float fBloomPower = 0.f;
		if (FAILED(pShader->Set_Value("g_fBloomPower", &fBloomPower, sizeof(_float))))
			return E_FAIL;
	}

	//============================================================================================
	// ��Ÿ ���
	//============================================================================================
	else
	{
		_mat matWVP = WorldMatrix * ViewMatrix * ProjMatrix;

		if (FAILED(pShader->Set_Value("g_matWVP", &matWVP, sizeof(_mat))))
			return E_FAIL;
	}

	//============================================================================================
	// ���̴� ����
	//============================================================================================

	_uint iNumMeshContainer = _uint(m_pMeshCom->Get_NumMeshContainer());

	for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
	{
		_uint iNumSubSet = (_uint)m_pMeshCom->Get_NumMaterials(i);

		for (_uint j = 0; j < iNumSubSet; ++j)
		{
			_int tmpPass = m_pMeshCom->Get_MaterialPass(i, j);

			pShader->Begin_Pass(iPass);
			pShader->Commit_Changes();

			pShader->Commit_Changes();

			m_pMeshCom->Render_Mesh(i, j);

			pShader->End_Pass();
		}
	}

	//============================================================================================


	return NOERROR;
}

void CClothDynamic::Init_Darpe()
{
	// set solver settings
	m_pCloth->setSolverFrequency(240);

	// damp global particle velocity to 90% every 0.1 seconds
	m_pCloth->setDampingCoefficient(PxVec3(0.2f)); // damp local particle velocity
	m_pCloth->setLinearDragCoefficient(PxVec3(0.2f)); // transfer frame velocity
	m_pCloth->setAngularDragCoefficient(PxVec3(0.2f)); // transfer frame rotation

	// reduce impact of frame acceleration
	// x, z: cloth swings out less when walking in a circle
	// y: cloth responds less to jump acceleration
	m_pCloth->setLinearInertiaScale(PxVec3(0.8f, 0.6f, 0.8f));

	// leave impact of frame torque at default
	//m_pCloth->setAngularInertiaScale(PxVec3(1.0f));
	m_pCloth->setAngularInertiaScale(PxVec3(0.3f));

	// reduce centrifugal force of rotating frame
	m_pCloth->setCentrifugalInertiaScale(PxVec3(0.3f));

	m_pCloth->setInertiaScale(0.5f);

	const bool useCustomConfig = true;

	// custom fiber configuration
	if (useCustomConfig)
	{
		PxClothStretchConfig stretchConfig;
		stretchConfig.stiffness = 1.0f;

		m_pCloth->setStretchConfig(PxClothFabricPhaseType::eVERTICAL, PxClothStretchConfig(0.8f));
		m_pCloth->setStretchConfig(PxClothFabricPhaseType::eHORIZONTAL, PxClothStretchConfig(0.6f));
		m_pCloth->setStretchConfig(PxClothFabricPhaseType::eSHEARING, PxClothStretchConfig(0.5f));
		m_pCloth->setStretchConfig(PxClothFabricPhaseType::eBENDING, PxClothStretchConfig(0.5f));
		m_pCloth->setTetherConfig(PxClothTetherConfig(1.0f));
	}
}

void CClothDynamic::Init_Gauntlet()
{
	// set solver settings
	m_pCloth->setSolverFrequency(240);

	// damp global particle velocity to 90% every 0.1 seconds
	m_pCloth->setDampingCoefficient(PxVec3(0.2f)); // damp local particle velocity
	m_pCloth->setLinearDragCoefficient(PxVec3(0.2f)); // transfer frame velocity
	m_pCloth->setAngularDragCoefficient(PxVec3(0.2f)); // transfer frame rotation

													   // reduce impact of frame acceleration
													   // x, z: cloth swings out less when walking in a circle
													   // y: cloth responds less to jump acceleration
	m_pCloth->setLinearInertiaScale(PxVec3(0.8f, 0.6f, 0.8f));

	// leave impact of frame torque at default
	//m_pCloth->setAngularInertiaScale(PxVec3(1.0f));
	m_pCloth->setAngularInertiaScale(PxVec3(0.3f));

	// reduce centrifugal force of rotating frame
	m_pCloth->setCentrifugalInertiaScale(PxVec3(0.3f));

	m_pCloth->setInertiaScale(0.5f);

	const bool useCustomConfig = true;

	// custom fiber configuration
	if (useCustomConfig)
	{
		PxClothStretchConfig stretchConfig;
		stretchConfig.stiffness = 1.0f;

		m_pCloth->setStretchConfig(PxClothFabricPhaseType::eVERTICAL, PxClothStretchConfig(0.8f));
		m_pCloth->setStretchConfig(PxClothFabricPhaseType::eHORIZONTAL, PxClothStretchConfig(0.6f));
		m_pCloth->setStretchConfig(PxClothFabricPhaseType::eSHEARING, PxClothStretchConfig(0.5f));
		m_pCloth->setStretchConfig(PxClothFabricPhaseType::eBENDING, PxClothStretchConfig(0.5f));
		m_pCloth->setTetherConfig(PxClothTetherConfig(1.0f));
	}
}

void CClothDynamic::Init_Muffler()
{
	// set solver settings
	m_pCloth->setSolverFrequency(240);

	// damp global particle velocity to 90% every 0.1 seconds
	m_pCloth->setDampingCoefficient(PxVec3(0.2f)); // damp local particle velocity
	m_pCloth->setLinearDragCoefficient(PxVec3(0.2f)); // transfer frame velocity
	m_pCloth->setAngularDragCoefficient(PxVec3(0.2f)); // transfer frame rotation

													   // reduce impact of frame acceleration
													   // x, z: cloth swings out less when walking in a circle
													   // y: cloth responds less to jump acceleration
	m_pCloth->setLinearInertiaScale(PxVec3(0.8f, 0.6f, 0.8f));

	// leave impact of frame torque at default
	//m_pCloth->setAngularInertiaScale(PxVec3(1.0f));
	m_pCloth->setAngularInertiaScale(PxVec3(0.3f));

	// reduce centrifugal force of rotating frame
	m_pCloth->setCentrifugalInertiaScale(PxVec3(0.3f));

	m_pCloth->setInertiaScale(0.5f);

	const bool useCustomConfig = true;

	// custom fiber configuration
	if (useCustomConfig)
	{
		PxClothStretchConfig stretchConfig;
		stretchConfig.stiffness = 1.0f;

		m_pCloth->setStretchConfig(PxClothFabricPhaseType::eVERTICAL, PxClothStretchConfig(0.8f));
		m_pCloth->setStretchConfig(PxClothFabricPhaseType::eHORIZONTAL, PxClothStretchConfig(0.6f));
		m_pCloth->setStretchConfig(PxClothFabricPhaseType::eSHEARING, PxClothStretchConfig(0.5f));
		m_pCloth->setStretchConfig(PxClothFabricPhaseType::eBENDING, PxClothStretchConfig(0.5f));
		m_pCloth->setTetherConfig(PxClothTetherConfig(1.0f));
	}
}

void CClothDynamic::Init_LongCoat()
{
	// set solver settings
	m_pCloth->setSolverFrequency(240);

	// damp global particle velocity to 90% every 0.1 seconds
	m_pCloth->setDampingCoefficient(PxVec3(0.2f)); // damp local particle velocity
	m_pCloth->setLinearDragCoefficient(PxVec3(0.2f)); // transfer frame velocity
	m_pCloth->setAngularDragCoefficient(PxVec3(0.2f)); // transfer frame rotation

													   // reduce impact of frame acceleration
													   // x, z: cloth swings out less when walking in a circle
													   // y: cloth responds less to jump acceleration
	m_pCloth->setLinearInertiaScale(PxVec3(0.8f, 0.6f, 0.8f));

	// leave impact of frame torque at default
	//m_pCloth->setAngularInertiaScale(PxVec3(1.0f));
	m_pCloth->setAngularInertiaScale(PxVec3(0.3f));

	// reduce centrifugal force of rotating frame
	m_pCloth->setCentrifugalInertiaScale(PxVec3(0.3f));

	m_pCloth->setInertiaScale(0.5f);

	const bool useCustomConfig = true;

	// custom fiber configuration
	if (useCustomConfig)
	{
		PxClothStretchConfig stretchConfig;
		stretchConfig.stiffness = 1.0f;

		m_pCloth->setStretchConfig(PxClothFabricPhaseType::eVERTICAL, PxClothStretchConfig(0.8f));
		m_pCloth->setStretchConfig(PxClothFabricPhaseType::eHORIZONTAL, PxClothStretchConfig(0.6f));
		m_pCloth->setStretchConfig(PxClothFabricPhaseType::eSHEARING, PxClothStretchConfig(0.5f));
		m_pCloth->setStretchConfig(PxClothFabricPhaseType::eBENDING, PxClothStretchConfig(0.5f));
		m_pCloth->setTetherConfig(PxClothTetherConfig(1.0f));
	}
}

HRESULT CClothDynamic::Add_Component(void * pArg)
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	
	_tchar name[STR_128] = { 0, };
	
	if (nullptr == pArg)
		lstrcpy(name, L"Mesh_Cloth_Drape");
	else
	{
		CClothDynamic::INFO Temp = *(CClothDynamic::INFO*)pArg;

		switch (Temp.eName)
		{
		case ClothDynamic_Name::Drape:
			lstrcpy(name, L"Mesh_Cloth_Drape");
			break;
		case ClothDynamic_Name::Gauntlet:
			lstrcpy(name, L"Mesh_Cloth_Gauntlet");
			break;
		case ClothDynamic_Name::LongCoat:
			lstrcpy(name, L"Mesh_Cloth_LongCoat");
			break;
		case ClothDynamic_Name::Muffler:
			lstrcpy(name, L"Mesh_Cloth_Muffler");
			break;
		}

	}


	// enum�� ���� �޽���ü �ؾ���.
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, name, L"Com_Mesh", (CComponent**)&m_pMeshCom)))
		return E_FAIL;

	m_pMesh = m_pMeshCom->Get_vecContainer()[0]->pOriginalMesh;

	// for.Com_Optimaization
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Optimization", L"Com_Optimization", (CComponent**)&m_pOptimizationCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CClothDynamic::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	_mat ViewMatrix = g_pManagement->Get_Transform(D3DTS_VIEW);
	_mat ProjMatrix = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	if (FAILED(m_pShaderCom->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(g_pDissolveTexture->SetUp_OnShader("g_FXTexture", m_pShaderCom)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fFxAlpha", &m_fFXAlpha, sizeof(_float))))
		return E_FAIL;

	//=============================================================================================
	// ���̴� �������� ��ġ �Է�
	//=============================================================================================
	_float	fEmissivePower = 3.f;	// �̹̽ú� : ���� ����, ��ü �߱��� ������.
	_float	fSpecularPower = 5.f;	// ��Ż�Ͻ� : ���� ����, ���ݻ簡 ������.
	_float	fRoughnessPower = 0.5f;	// �����Ͻ� : ���� ����, �� ����� ������(���� ������).
	_float	fRimLightPower = 0.f;	// ��		: ���� ���� ���� ����(������Ʈ�� ������ �о����� , �������� ������).
	_float	fMinSpecular = 1.f;	// �ּ� ��	: ���� ���� ���� ����(������Ʈ�� ������ �о����� , �������� ������).

	if (FAILED(m_pShaderCom->Set_Value("g_fEmissivePower", &fEmissivePower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fSpecularPower", &fSpecularPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fRoughnessPower", &fRoughnessPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fRimAlpha", &fRimLightPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fMinSpecular", &fMinSpecular, sizeof(_float))))
		return E_FAIL;
	//=============================================================================================


	return S_OK;
}

CClothDynamic * CClothDynamic::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CClothDynamic* pInstance = new CClothDynamic(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CClothDynamic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CClothDynamic::Clone_GameObject(void * pArg)
{
	CClothDynamic* pInstance = new CClothDynamic(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CClothDynamic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CClothDynamic::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pOptimizationCom);

	CCloth::Free();
}
