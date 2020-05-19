#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"
#include "ClothManager.h"

BEGIN(Client)

class CCostume_Hair : public CGameObject
{

public:
	typedef struct tagInitInfo
	{
		tagInitInfo(_mat* pmatParent, _mat* pmatBone, _v4 vColorValue)
			: pmatParent(pmatParent) , pmatBone(pmatBone) , vColorValue(vColorValue)
		{}

		_mat*		pmatParent = nullptr;
		_mat*		pmatBone = nullptr;
		_v4			vColorValue = _v4(0.f, 0.f, 0.f, 0.f);
	}_INFO;

private:
	explicit CCostume_Hair(_Device pGraphicDev);
	explicit CCostume_Hair(const CCostume_Hair& rhs);
	virtual ~CCostume_Hair() = default;

public:
	virtual _v4				Get_Color() { return m_vColorValue; }

public:
	virtual void			Set_AttachBoneMartix(_mat* _matAttachBone) { m_pmatBone = _matAttachBone; }
	virtual void			Set_ParentMatrix(_mat* _matParent) { m_pmatParent = _matParent; }

	virtual void			Change_HairMesh(CClothManager::Cloth_Static _eHairType);
	virtual void			Set_Color(_v4 _vColor) { m_vColorValue = _vColor; }

public:
	virtual _int	Update_GameObject(_double TimeDelta);
	_int	Update_GameObject(_double TimeDelta, _bool bClearCol);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_Instancing_SetPass(CShader* pShader);
	virtual HRESULT Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur = false);

private:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual HRESULT Add_Components();
	virtual HRESULT Setup_Default();
	virtual HRESULT SetUp_ConstantTable(CShader* pShader);

private:
	virtual void Calc_AttachBoneTransform();

private:
	void Change_Vertex();

private:
	CTransform*						m_pTransform = nullptr;
	CRenderer*						m_pRenderer = nullptr;
	CShader*						m_pShader = nullptr;
	CMesh_Static*					m_pStaticMesh = nullptr;
	CBattleAgent*					m_pBattleAgent = nullptr;

private:
	_mat*							m_pmatParent = nullptr;
	_mat*							m_pmatBone = nullptr;

private:
	CClothManager::Cloth_Static		m_eHairType = CClothManager::Hair_01;
	_v4								m_vColorValue = {1.f, 0.f, 0.f, 1.f};

public:
	static CCostume_Hair* Create(_Device pGraphicDev);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void free();
};

END