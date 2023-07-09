#pragma once

#include "Export_Function.h"
#include "TempUI.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;

END

class CSelectFont : public CTempUI
{
private:
	explicit CSelectFont(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSelectFont();

public:
	HRESULT Ready_Object() override;
	_int Update_Object(const _float& fTimeDelta) override;
	void LateUpdate_Object(void) override;
	void Render_Object(void) override;

private:
	HRESULT		Add_Component(void);
	void	    Key_Input(void);

private:
	CRcTex*			m_pBufferCom = nullptr;
	CTexture*		m_pTextureCom = nullptr;

public:
	static CSelectFont*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;

};

