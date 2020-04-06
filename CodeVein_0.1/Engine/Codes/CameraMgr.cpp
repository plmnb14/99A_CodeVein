#include "CameraMgr.h"

IMPLEMENT_SINGLETON(CCameraMgr)

CCameraMgr::CCameraMgr(void)
	: m_bDelay(false), m_bChange(false), m_pMainCamera(nullptr), m_pCamTag(nullptr), m_wContainerSize(0)
{

}

CCameraMgr::~CCameraMgr(void)
{
	//Free();
}

void CCameraMgr::Update()
{
	if (m_pMainCamera != nullptr)
	{
		m_pMainCamera->Update_GameObject();

		if (m_bChange)
			Set_MainCamera(m_sCamClass, m_pCamTag);
	}
}


HRESULT CCameraMgr::Reserve_ContainerSize(const _ushort& wSize)
{
	if (nullptr != m_pmapCamera)
		return E_FAIL;

	m_pmapCamera = new map<const _tchar*, CCamera*>[wSize];

	m_wContainerSize = wSize;

	return S_OK;
}

HRESULT CCameraMgr::Ready_Camera(_Device pGraphicDev,
	const _ushort& _eCameraClass,
	const _tchar* pCameraTag,
	CameraView _eCameraView,
	CameraMode _eCameraMode)
{
	if (nullptr == m_pmapCamera)
	{
		MSG_BOX("Resources Container not Reserved");
		return E_FAIL;
	}

	CCamera* pCamera = Find_Camera(_eCameraClass, pCameraTag);

	if (nullptr != pCamera)
		return E_FAIL;

	switch (_eCameraClass)
	{
	case DYNAMIC_CAM:
	{
		pCamera = CDynamic_Camera::Create(pGraphicDev, _eCameraView, _eCameraMode);
		break;
	}
	}

	//m_pMainCamera = pCamera;
	m_pmapCamera[_eCameraClass].emplace(pCameraTag, pCamera);

	//pCamera = nullptr;

	return S_OK;
}

void CCameraMgr::Set_MainCamera(const _ushort & _eCameraClass, const _tchar * pCameraTag)
{
	if (m_pMainCamera == nullptr)
	{
		m_pMainCamera = Find_Camera(_eCameraClass, pCameraTag);
		//Safe_AddRef(m_pMainCamera);

		m_bChange = false;
		return;
	}

	else if (m_pMainCamera != nullptr)
	{
		CCamera* tmpCam = Find_Camera(_eCameraClass, pCameraTag);

		if (Calc_Distance(m_pMainCamera, tmpCam, &m_vDistance))
		{
			m_pMainCamera = Find_Camera(_eCameraClass, pCameraTag);
			m_bChange = false;
			return;
		}
	}
}

HRESULT CCameraMgr::Set_Target(CGameObject * _Target)
{
	if (m_pMainCamera != nullptr)
	{
		m_pMainCamera->Set_Target(_Target);
	}

	return S_OK;
}

void CCameraMgr::Change_MainCam(const _ushort & _eCameraClass, const _tchar * pCameraTag, _bool _bDelay)
{
	//if (!m_bChange)
	//{
	//	if (m_pCamTag != nullptr)
	//		m_pCamTag = nullptr;
	//
	//	_tchar* tmpChar = new _tchar[128];
	//
	//	lstrcpy(tmpChar, pCameraTag);
	//	m_pCamTag = tmpChar;
	//
	//	m_bDelay = _bDelay;
	//	m_sCamClass = _eCameraClass;
	//	m_bChange = true;
	//}
}

void CCameraMgr::Set_CamView(CameraView _eCamViewType)
{
	m_pMainCamera->Set_CameraViewType(_eCamViewType);
}

void CCameraMgr::Set_CamMode(CameraMode _eCamMode)
{
	m_pMainCamera->Set_CameraMode(_eCamMode);
}

void CCameraMgr::Set_MaxDistance(_float _fMaxdist)
{
	m_pMainCamera->Set_MaxDist(_fMaxdist);
}

void CCameraMgr::Set_MidDistance(_float _fMidDist)
{
	m_pMainCamera->Set_MidDist(_fMidDist);
}

void CCameraMgr::Set_MinDistance(_float _fMinDist)
{
	m_pMainCamera->Set_MaxDist(_fMinDist);
}

void CCameraMgr::Set_Distance(_float _fDist)
{
}

void CCameraMgr::Set_OnAimingTarget(_bool _bOnAim)
{
	m_pMainCamera->Set_OnAimingTarget(_bOnAim);
}

void CCameraMgr::Set_AimingTarget(CGameObject * pAimingTarget)
{
	m_pMainCamera->Set_AimingTarget(pAimingTarget);
}

void CCameraMgr::Set_MouseCtrl(_bool _bMouseCtrl)
{
	m_pMainCamera->Set_MouseControl(_bMouseCtrl);
}

void CCameraMgr::Add_MainPos(_float _fSpeed, _v3 _vDir)
{
	m_pMainCamera->Add_Pos(_fSpeed, _vDir);
}

void CCameraMgr::Add_MainAt(_float _fSpeed, _v3 _vDir)
{
	m_pMainCamera->Add_At(_fSpeed, _vDir);
}

void CCameraMgr::Set_MainAt(_v3 _vAt)
{
	m_pMainCamera->Set_At(_vAt);
}

void CCameraMgr::Set_LockAngleX(_float _fLockAngleX)
{
	m_pMainCamera->Set_LockAngleX(_fLockAngleX);
}

void CCameraMgr::MainCamera_Oscillatation_SetUp(_float _fDuration, _float _fFrequency, _float _fPower, _float _fMutiply, CCamera::CAM_OSC_TYPE _eOSCType)
{
	m_pMainCamera->Camera_Oscillatation_SetUp(_fDuration, _fFrequency, _fPower, _fMutiply, _eOSCType);
}

_v3 * CCameraMgr::Calc_MainCamPosAt()
{
	return m_pMainCamera->Calc_Pos_and_At();
}

void CCameraMgr::Set_MainPos(_v3 _vPos)
{
	m_pMainCamera->Set_Pos(_vPos);
}

const CameraView CCameraMgr::Get_CamView()
{
	return m_pMainCamera->Get_ViewType();
}

const _mat& CCameraMgr::Get_ViewMat()
{
	m_matView = m_pMainCamera->Get_ViewMat();
	return m_matView;
}

const _mat& CCameraMgr::Get_ProjMat()
{
	m_matProj = m_pMainCamera->Get_ProjMat();
	return m_matProj;
}

const _v3 & CCameraMgr::Get_Right()
{
	m_vRight = m_pMainCamera->Get_Right();
	return m_vRight;
}

const _v3 & CCameraMgr::Get_Up()
{
	m_vUp = m_pMainCamera->Get_Up();
	return m_vUp;
}

const _v3 & CCameraMgr::Get_Look()
{
	m_vLook = m_pMainCamera->Get_Look();
	return m_vLook;
}

const _v3 & CCameraMgr::Get_Pos()
{
	m_vPos = m_pMainCamera->Get_Pos();
	return m_vPos;
}

_v3 CCameraMgr::Get_At()
{
	return m_pMainCamera->Get_LookAt();
}

const _float & CCameraMgr::Get_XAngle()
{
	m_fX_Angle = m_pMainCamera->Get_XAngle();
	return m_fX_Angle;
}

const _float & CCameraMgr::Get_YAngle()
{
	m_fY_Angle = m_pMainCamera->Get_YAngle();
	return m_fY_Angle;
}

CCamera* CCameraMgr::Find_Camera(const _ushort& _eCameraClass, const _tchar* pCameraTag)
{
	auto	iter = find_if(m_pmapCamera[_eCameraClass].begin(), m_pmapCamera[_eCameraClass].end(), CTag_Finder(pCameraTag));

	if (iter == m_pmapCamera[_eCameraClass].end())
		return nullptr;

	return iter->second;
}

_bool CCameraMgr::Calc_Distance(CCamera * _pOld, CCamera * _pNew, _v3 * _Distance)
{
	//_v3 tmpDir , vOld, vNew;
	//
	//vOld = TARGET_TO_TRANS(_pOld)->Get_Pos();
	//vNew = TARGET_TO_TRANS(_pNew)->Get_Pos();
	//
	//
	//*_Distance = vOld - vNew;
	//m_fLength = D3DXVec3Length(&(vOld - vNew));
	//
	//
	//CALC::V3_Dir_Normal(&tmpDir, &vOld, &vNew);
	//
	//TARGET_TO_TRANS(_pOld)->Add_Pos(10.f, tmpDir);
	//
	//return (m_fLength <= 0 ? true : false);

	return true;
}

void CCameraMgr::Free(void)
{
	Safe_Delete_Array(m_pCamTag);

	for (_ushort i = 0; i < m_wContainerSize; ++i)
	{
		for (auto& Pair : m_pmapCamera[i])
			Safe_Release(Pair.second);

		m_pmapCamera[i].clear();
	}

	Safe_Delete_Array(m_pmapCamera);
}



