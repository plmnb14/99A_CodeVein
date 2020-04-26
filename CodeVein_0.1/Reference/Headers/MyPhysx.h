#pragma once

#include "Engine_Defines.h"
#include "Base.h"
#include "PxPhysicsAPI.h"

BEGIN(Engine)

class ENGINE_DLL CMyPhysx final : public CBase
{
	DECLARE_SINGLETON(CMyPhysx)

private:
	explicit CMyPhysx();
	virtual ~CMyPhysx() = default;

public:
	HRESULT Ready_MyPhysx();
	int Update_MyPhysx(_double TimeDelta);

public:
	physx::PxPhysics*	Get_Physics() { return m_pPhysics; }
	physx::PxScene*		Get_Scene() { return m_pScene; }

public:
	void Clear_Scene();

private:
	physx::PxFoundation*				m_pFoundation = nullptr;
	physx::PxPhysics*					m_pPhysics = nullptr;
	physx::PxDefaultCpuDispatcher*		m_pDispatcher = nullptr;
	physx::PxScene*						m_pScene = nullptr;

	physx::PxDefaultAllocator			m_Allocator;
	physx::PxDefaultErrorCallback		m_ErrorCallback;

public:
	virtual void Free();
};

END