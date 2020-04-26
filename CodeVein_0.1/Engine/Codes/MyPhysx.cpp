#include "..\Headers\MyPhysx.h"

#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL;	}
#define PVD_HOST "127.0.0.1"	//Set this to the IP address of the system running the PhysX Visual Debugger that you want to connect to.

IMPLEMENT_SINGLETON(CMyPhysx)

using namespace physx;

CMyPhysx::CMyPhysx()
{

}

HRESULT CMyPhysx::Ready_MyPhysx()
{
	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);

	if (nullptr == m_pFoundation)
	{
		MSG_BOX("CMyPhysx : Failed to Create Foundation");
		return E_FAIL;
	}

	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), true);

	if (nullptr == m_pPhysics)
	{
		MSG_BOX("CMyPhysx : Failed to Create m_Physics");
		return E_FAIL;
	}

	// ±âº» ¾À »ý¼º
	PxSceneDesc sceneDesc(m_pPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	m_pDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = m_pDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	m_pScene = m_pPhysics->createScene(sceneDesc);

	//m_Material = m_Physics->createMaterial(0.5f, 0.5f, 0.6f);


	return S_OK;
}

int CMyPhysx::Update_MyPhysx(_double TimeDelta)
{
	m_pScene->simulate(_float(TimeDelta));
	m_pScene->fetchResults(true);

	return 0;
}

void CMyPhysx::Clear_Scene()
{
	m_pScene->flushSimulation();
}

void CMyPhysx::Free()
{
	PX_RELEASE(m_pScene);
	//PX_RELEASE(m_Material);
	PX_RELEASE(m_pDispatcher);
	PX_RELEASE(m_pPhysics);

	PX_RELEASE(m_pFoundation);
}
