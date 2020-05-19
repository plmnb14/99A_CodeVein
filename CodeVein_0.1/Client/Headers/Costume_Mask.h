#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CCostume_Mask : public CGameObject
{

public:
	typedef enum tagMaskType
	{
		Mask_06, Mask_07, Mask_08, Mask_10, Mask_11,
		Mask_Drape, Mask_Gauntlet, Mask_Stinger, Mask_Ivy,
		Mask_End
	}MASK_TYPE;

public:
	typedef struct tagInitInfo
	{
		tagInitInfo(_mat* pmatParent, _mat* pmatBone, _v4 vColorValue, MASK_TYPE _eMaskType)
			:eMaskType(_eMaskType), pmatParent(pmatParent), pmatBone(pmatBone), vColorValue(vColorValue)
		{}

		MASK_TYPE		eMaskType = Mask_End;
		_mat*		pmatParent = nullptr;
		_mat*		pmatBone = nullptr;
		_v4			vColorValue = _v4(0.f, 0.f, 0.f, 0.f);

	}_INFO;

private:
	explicit CCostume_Mask(_Device pGraphicDev);
	explicit CCostume_Mask(const CCostume_Mask& rhs);
	virtual ~CCostume_Mask() = default;

public:
	virtual void			Set_AttachBoneMartix(_mat* _matAttachBone) { m_pmatBone = _matAttachBone; }
	virtual void			Set_ParentMatrix(_mat* _matParent) { m_pmatParent = _matParent; }
	virtual void			Change_AccMesh(MASK_TYPE _eMaskType);

public:
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_Instancing_SetPass(CShader* pShader);
	virtual HRESULT Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur = false);

private:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual HRESULT Add_Components(const _tchar * pMeshName);
	virtual HRESULT Setup_Default();
	virtual HRESULT SetUp_ConstantTable(CShader* pShader);

private:
	virtual void Calc_AttachBoneTransform();

private:
	CTransform*				m_pTransform = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	CShader*				m_pShader = nullptr;
	CMesh_Static*			m_pStaticMesh = nullptr;
	CBattleAgent*			m_pBattleAgent = nullptr;

private:
	_mat*					m_pmatParent = nullptr;
	_mat*					m_pmatBone = nullptr;

private:
	MASK_TYPE				m_eMaskType = Mask_06;
	_v4						m_vColorValue = {};

public:
	static CCostume_Mask* Create(_Device pGraphicDev);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void free();
};

END