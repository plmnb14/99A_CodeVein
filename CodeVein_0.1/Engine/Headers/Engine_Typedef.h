#ifndef Engine_Typedef_h__
#define Engine_Typedef_h__

namespace Engine
{
	typedef		bool					_bool;

	typedef		signed char				_char;
	typedef		signed char				_byte;
	typedef		unsigned char			_ubyte;

	typedef		wchar_t					_tchar;

	typedef		signed short			_short;
	typedef		unsigned short			_ushort;

	typedef		signed int				_int;
	typedef		unsigned int			_uint;

	typedef		signed long				_long;
	typedef		unsigned long			_ulong;

	typedef		float					_float;
	typedef		double					_double;

	////////////////////////////////////////////////////////////////////////
	// Interface

	typedef		LPDIRECT3D9				_DirectX;
	typedef		LPDIRECT3DDEVICE9		_Device;
	typedef		LPDIRECT3DTEXTURE9		_Tex;

	////////////////////////////////////////////////////////////////////////
	// Default Struct
	typedef		D3DLIGHT9				_Light;

	typedef		D3DXVECTOR3				_v3;
	typedef		D3DXVECTOR2				_v2;
	typedef		D3DXVECTOR4				_v4;

	typedef		D3DXMATRIX				_mat;
}

#endif // Engine_Typedef_h__
