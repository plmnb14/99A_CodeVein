#pragma once

#include "GameObject.h"

BEGIN(Engine)

class CTransform;
class CShader;
class CMesh_Static;
class CCollider;
class CRenderer;
class COptimization;

class ENGINE_DLL CRenderObject : public CGameObject
{
private:
	typedef struct tagPBRShaderInfo
	{
		_float	fEmissivePower = 0.f;
		_float	fSpecularPower = 0.f;
		_float	fRoughnessPower = 0.f;
		_float	fMinSpecular = 0.f;
		_float	fID_R = 0.f;
		_float  fID_G = 0.f;
		_float  fID_B = 0.f;
		_float  fRimPower = 0.f;
		_float  fRimAlpha = 0.f;
	}PBR;

public:
	typedef struct tagObjInitInfo
	{
		tagObjInitInfo(_tchar* _meshName, _v3 vPos, _v3 vAngle, _v3 vScale , _uint iIndex)
			: vPos(vPos), vAngle(vAngle), vScale(vScale), iIndex(iIndex)
		{
			lstrcpy(szMeshName, _meshName);
		}

		_tchar szMeshName[STR_128];

		_v3 vPos;
		_v3 vAngle;
		_v3 vScale;

		_uint iIndex;

	}OBJ_INFO;

protected:
	explicit CRenderObject(_Device _pGraphicDev);
	explicit CRenderObject(const CRenderObject& rhs);
	explicit CRenderObject(const CRenderObject& rhs , _bool _OnTool);
	virtual ~CRenderObject();

public:
	virtual _int Update_GameObject(_double _TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_Instancing_SetPass(CShader* pShader);
	virtual HRESULT Render_GameObject_SetPass(CShader* pShader, _int iPass, _bool _bIsForMotionBlur = false);

public:
	virtual void Update_Collider();

public:
	HRESULT Add_Essentional();
	HRESULT Add_Essentional_Copy();
	HRESULT Default_Setting();

public:
	void Change_Mesh(const _tchar* _MeshName);

public:
	virtual void Set_RenderGroup(RENDERID _eGroup);
	virtual void Set_Index(_int _iIndex) { m_iIndex = _iIndex; }
	virtual void Set_Selected(_bool _bSelected) { m_bIsSelected = _bSelected; }
	virtual void Set_OnTool(_bool _bOnTool) { m_bOnTool = _bOnTool; }

public:
	virtual _tchar*	Get_Name() { return m_szName; }
	virtual _int	Get_Index() { return m_iIndex; }
	virtual _bool   Get_Selected() { return m_bIsSelected; }

protected:
	CTransform*			m_pTransform = nullptr;
	CCollider*			m_pCollider = nullptr;
	CMesh_Static*		m_pMesh_Static = nullptr;
	CRenderer*			m_pRenderer = nullptr;
	CShader*			m_pShader = nullptr;
	COptimization*		m_pOptimization = nullptr;

	// 텍스쳐, 메쉬 관련 변수
protected:
	_tchar			m_szName[STR_128] = L"";
	_int			m_iIndex = 0;
	_ulong			m_dwPassNum = 0;


private:
	virtual void SetUp_IndividualShaderValue();

private:
	_ulong			m_dwSubsetCnt = 0;

	_bool			m_bUpdated = false;

	// For	MotionBlur
	_mat	m_matLastWVP;


	// 툴에서 사용되는 변수
protected:
	_bool			m_bIsSelected = false;
	_bool			m_bOnTool = false;
	_bool			m_bAdvencedCull = false;

	// 오브젝트 렌더 그룹 변수
private:
	RENDERID		m_eGroup = RENDER_NONALPHA;
	PBR				m_tPBRInfo;

private:
	virtual HRESULT Initialize_For_Protoype();
	virtual HRESULT Initialize();
	virtual HRESULT LateInit_GameObject();
	virtual void	Init_Shader(CShader* pShader);

	virtual HRESULT Ready_GameObject(void* pAvg);
	virtual HRESULT	Add_Components(_tchar * szMeshName);

private:
	virtual void Check_Stage_01();
	virtual void Check_Stage_02();
	virtual void Check_Stage_03();

public:
	static CRenderObject* Create_For_Tool(_Device _pGraphicDev);
	static CRenderObject* CreateClone_For_Tool(CRenderObject* _pCopy, _bool _OnTool);
	static CRenderObject* Create(_Device _pGraphicDev);
	static CRenderObject* CreateClone(CRenderObject* _pCopy);

public:
	virtual void Free();

public:
	virtual CGameObject* Clone_GameObject(void* pArg);
};

END

