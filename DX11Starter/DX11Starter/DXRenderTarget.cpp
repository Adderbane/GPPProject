#include "DXRenderTarget.h"



DXRenderTarget::DXRenderTarget(ID3D11Device* device, D3D11_TEXTURE2D_DESC texDesc, D3D11_RENDER_TARGET_VIEW_DESC rtvDesc, D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc)
{
	ID3D11Texture2D* tex;
	device->CreateTexture2D(&texDesc, 0, &tex);
	device->CreateRenderTargetView(tex, &rtvDesc, &rtv);
	device->CreateShaderResourceView(tex, &srvDesc, &srv);
	tex->Release();
}


DXRenderTarget::~DXRenderTarget()
{
	srv->Release();
	rtv->Release();
}

ID3D11RenderTargetView* DXRenderTarget::GetRTV()
{
	return rtv;
}

ID3D11ShaderResourceView* DXRenderTarget::GetSRV()
{
	return srv;
}
