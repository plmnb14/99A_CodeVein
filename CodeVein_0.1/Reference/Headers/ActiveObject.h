#pragma once

#include "GameObject.h"

BEGIN(Engine)

class CTransform;
class CShader;
class CMesh_Static;
class CCollider;
class CRenderer;
class COptimization;
class CBattleAgent;

class ENGINE_DLL CActiveObject : public CGameObject
{
public:
	typedef struct tagObjInitInfo
	{
		tagObjInitInfo(_tchar* _meshName, _v3 vPos, _v3 vAngle, _v3 vScale, _uint iObjIndex, _uint iActiveIndex)
			: vPos(vPos), vAngle(vAngle), vScale(vScale), iObjIndex(iObjIndex), iActiveIndex(iActiveIndex)
		{
			lstrcpy(szMeshName, _meshName);
		}

		_tchar szMeshName[STR_128];

		_v3 vPos;
		_v3 vAngle;
		_v3 vScale;

		_uint iObjIndex;
		_uint iActiveIndex;

	}ACTOBJ_INFO;

enum ActiveObj { ATV_MISTLETOE, ATV_DOOR, ATV_BOX, ATV_BOX_LID, ATV_END };

protected:
	explicit CActiveObject(_Device _pGraphicDev);
	explicit CActiveObject(const CActiveObject& rhs);
	virtual ~CActiveObject();

public:
	virtual _int Update_GameObject(_double _TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur = false);
	virtual HRESULT Render_GameObject_Instancing_SetPass(CShader* pShader);


public:
	void Chaning_AtvMesh(const _tchar* _MeshName);

public:
	HRESULT Default_Setting();

public:
	virtual _tchar* Get_AtvName() { return m_szAciveName; }
	virtual _int	Get_AtvNumber() { return m_iAtvNumber; }

public:
	virtual void	Set_Index(_int iIndex) { m_iIndex = iIndex; }

protected:
	CTransform*		m_pTransform = nullptr;
	CCollider*		m_pCollider = nullptr;
	//CMesh_Dynamic*	m_pMesh_Dynamic = nullptr;
	CMesh_Static*	m_pMesh_Static = nullptr;
	CRenderer*		m_pRenderer = nullptr;
	CShader*		m_pShader = nullptr;
	CBattleAgent*	m_pBattleAgent = nullptr;
	COptimization*	m_pOptimization = nullptr;

	_int			m_iAtvNumber = 0;
	_int			m_iIndex = 0;

	_ulong			m_PassNum = 0;

	_tchar			m_szAciveName[STR_128] = L"";

	_bool			m_bCheck_Mistletoe = false;
	// 임시적으로 멤버변수 불값 사용. 후에 수정
	_bool			m_bCheck_BoxLid = false;
	_bool			m_bCheck_Boxopen = false;
	_bool			m_bCheck_Boxopen_end = false;

private:
	RENDERID		m_pGrouop = RENDER_ALPHA;

private:
	virtual HRESULT Ready_GameObject(void * pArg);
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT LateInit_GameObject();
	virtual void	Init_Shader(CShader* pShader);
	virtual HRESULT	Add_Components(_tchar* _meshName);
	virtual HRESULT Setup_Arg(void * pArg);

public:
	static CActiveObject* Create(_Device _pGraphicDev);

public:
	virtual void Free();

public:
	virtual CGameObject* Clone_GameObject(void * pArg);
};

END