#pragma once

#include "..\Headers\Cloth.h"

BEGIN(Client)

class CClothStatic final : public CCloth
{
public:
	enum ClothStatic_Name {
		Hair_Long, Hair_Ponytail
	};

	typedef struct tagInitInfo
	{
		tagInitInfo(ClothStatic_Name _eName)
			: eName(_eName)
		{}

		ClothStatic_Name	eName;

	}INFO;

protected:
	explicit CClothStatic(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CClothStatic(const CClothStatic& rhs);
	virtual ~CClothStatic() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_SetPass(CShader* pShader, _int iPass, _bool _bIsForMotionBlur = false);

private:
	void Init_HairLong();
	void Init_HairPonyTail();

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Static*		m_pMesh_Static = nullptr;
	COptimization*		m_pOptimizationCom = nullptr;


private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable();

public:
	static CClothStatic* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

};

END