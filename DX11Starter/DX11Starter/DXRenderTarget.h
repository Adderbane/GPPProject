#pragma once

#include<d3d11.h>

class DXRenderTarget
{
public:
	DXRenderTarget(ID3D11Device* device, D3D11_TEXTURE2D_DESC texDesc, D3D11_RENDER_TARGET_VIEW_DESC rtvDesc, D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc);
	~DXRenderTarget();

	ID3D11RenderTargetView* GetRTV();
	ID3D11ShaderResourceView* GetSRV();

private:
	ID3D11RenderTargetView* rtv;
	ID3D11ShaderResourceView* srv;
};

