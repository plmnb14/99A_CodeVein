#ifndef Buffer_Sphere_h__
#define Buffer_Sphere_h__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CBuffer_Sphere : public CVIBuffer
{
private:
	explicit CBuffer_Sphere(_Device pGraphicDev);
	explicit CBuffer_Sphere(const CBuffer_Sphere& rhs);
	virtual ~CBuffer_Sphere() = default;

public:
	virtual HRESULT Ready_Component_Prototype();
	virtual HRESULT Ready_Component(void* pArg);
	void Render_VIBuffer();

public:
	static CBuffer_Sphere* Create(_Device pGraphicDev);

public:
	virtual CComponent* Clone_Component(void* pArg = nullptr);
	virtual void Free();
};

END

#endif // SphereCol_L_h__
