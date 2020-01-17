#include "GameApp.h"
#include "d3dUtil.h"
#include "DXTrace.h"

using namespace DirectX;

GameApp::GameApp(HINSTANCE hInstance)
	: D3DApp(hInstance),
	m_RandomNumsCount()
{
}

GameApp::~GameApp()
{
}

bool GameApp::Init()
{
	if (!D3DApp::Init())
		return false;

	if (!InitResource())
		return false;

	return true;
}

void GameApp::Compute()
{
	assert(m_pd3dImmediateContext);

//#if defined(DEBUG) | defined(_DEBUG)
//	ComPtr<IDXGraphicsAnalysis> graphicsAnalysis;
//	HR(DXGIGetDebugInterface1(0, __uuidof(graphicsAnalysis.Get()), reinterpret_cast<void**>(graphicsAnalysis.GetAddressOf())));
//	graphicsAnalysis->BeginCapture();
//#endif

	// GPU排序
	m_Timer.Reset();
	m_Timer.Start();
	GPUSort();
	m_Timer.Tick();
	m_Timer.Stop();
	float gpuTotalTime = m_Timer.TotalTime();

	// 结果拷贝出来
	m_pd3dImmediateContext->CopyResource(m_pTypedBufferCopy.Get(), m_pTypedBuffer1.Get());
	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(m_pd3dImmediateContext->Map(m_pTypedBufferCopy.Get(), 0, D3D11_MAP_READ, 0, &mappedData));

//#if defined(DEBUG) | defined(_DEBUG)
//	graphicsAnalysis->EndCapture();
//#endif

	// CPU排序
	m_Timer.Reset();
	m_Timer.Start();
	std::sort(m_RandomNums.begin(), m_RandomNums.begin() + m_RandomNumsCount);
	m_Timer.Tick();
	m_Timer.Stop();
	float cpuTotalTime = m_Timer.TotalTime();

	bool isSame = !memcmp(mappedData.pData, m_RandomNums.data(),
		sizeof(UINT) * m_RandomNums.size());

	m_pd3dImmediateContext->Unmap(m_pTypedBufferCopy.Get(), 0);

	std::wstring wstr = L"排序元素数目：" + std::to_wstring(m_RandomNumsCount) +
		L"/" + std::to_wstring(m_RandomNums.size());
	wstr += L"\nGPU用时：" + std::to_wstring(gpuTotalTime) + L"秒";
	wstr += L"\nCPU用时：" + std::to_wstring(cpuTotalTime) + L"秒";
	wstr += isSame ? L"\n排序结果一致" : L"\n排序结果不一致";
	MessageBox(nullptr, wstr.c_str(), L"排序结束", MB_OK);

}



bool GameApp::InitResource()
{
	// 初始化随机数数据
	std::mt19937 randEngine;
	randEngine.seed(std::random_device()());
	std::uniform_int_distribution<UINT> powRange(9, 18);
	// 元素数目必须为2的次幂且不小于512个，并用最大值填充
	UINT elemCount = 1 << powRange(randEngine);
	m_RandomNums.assign(elemCount, UINT_MAX);
	// 填充随机数目的随机数，数目在一半容量到最大容量之间
	std::uniform_int_distribution<UINT> numsCountRange((UINT)m_RandomNums.size() / 2,
		(UINT)m_RandomNums.size());
	m_RandomNumsCount = numsCountRange(randEngine);
	std::generate(m_RandomNums.begin(), m_RandomNums.begin() + m_RandomNumsCount, [&] {return randEngine(); });

	HR(CreateTypedBuffer(m_pd3dDevice.Get(), m_RandomNums.data(), (UINT)m_RandomNums.size() * sizeof(UINT),
		m_pTypedBuffer1.GetAddressOf(), false, true));
	
	HR(CreateTypedBuffer(m_pd3dDevice.Get(), nullptr, (UINT)m_RandomNums.size() * sizeof(UINT),
		m_pTypedBuffer2.GetAddressOf(), false, true));

	HR(CreateTypedBuffer(m_pd3dDevice.Get(), nullptr, (UINT)m_RandomNums.size() * sizeof(UINT),
		m_pTypedBufferCopy.GetAddressOf(), true, true));

	HR(CreateConstantBuffer(m_pd3dDevice.Get(), nullptr, sizeof(CB),
		m_pConstantBuffer.GetAddressOf(), false, true));

	// 创建着色器资源视图
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R32_UINT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = (UINT)m_RandomNums.size();
	HR(m_pd3dDevice->CreateShaderResourceView(m_pTypedBuffer1.Get(), &srvDesc,
		m_pDataSRV1.GetAddressOf()));
	HR(m_pd3dDevice->CreateShaderResourceView(m_pTypedBuffer2.Get(), &srvDesc,
		m_pDataSRV2.GetAddressOf()));

	// 创建无序访问视图
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = DXGI_FORMAT_R32_UINT;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.Flags = 0;
	uavDesc.Buffer.NumElements = (UINT)m_RandomNums.size();
	HR(m_pd3dDevice->CreateUnorderedAccessView(m_pTypedBuffer1.Get(), &uavDesc,
		m_pDataUAV1.GetAddressOf()));
	HR(m_pd3dDevice->CreateUnorderedAccessView(m_pTypedBuffer2.Get(), &uavDesc,
		m_pDataUAV2.GetAddressOf()));

	// 创建计算着色器
	ComPtr<ID3DBlob> blob;
	HR(CreateShaderFromFile(L"HLSL\\BitonicSort_CS.cso",
		L"HLSL\\BitonicSort_CS.hlsl", "CS", "cs_5_0", blob.GetAddressOf()));
	HR(m_pd3dDevice->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pBitonicSort_CS.GetAddressOf()));

	HR(CreateShaderFromFile(L"HLSL\\MatrixTranspose_CS.cso",
		L"HLSL\\MatrixTranspose_CS.hlsl", "CS", "cs_5_0", blob.GetAddressOf()));
	HR(m_pd3dDevice->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pMatrixTranspose_CS.GetAddressOf()));

	// ******************
	// 设置调试对象名
	//
	D3D11SetDebugObjectName(m_pConstantBuffer.Get(), "ConstantBuffer");
	D3D11SetDebugObjectName(m_pTypedBuffer1.Get(), "TypedBuffer1");
	D3D11SetDebugObjectName(m_pTypedBuffer2.Get(), "TypedBuffer2");
	D3D11SetDebugObjectName(m_pTypedBufferCopy.Get(), "TypedBufferCopy");
	D3D11SetDebugObjectName(m_pDataUAV1.Get(), "DataUAV1");
	D3D11SetDebugObjectName(m_pDataUAV2.Get(), "DataUAV2");
	D3D11SetDebugObjectName(m_pDataSRV1.Get(), "DataSRV1");
	D3D11SetDebugObjectName(m_pDataSRV2.Get(), "DataSRV2");
	D3D11SetDebugObjectName(m_pBitonicSort_CS.Get(), "BitonicSort_CS");
	D3D11SetDebugObjectName(m_pMatrixTranspose_CS.Get(), "MatrixTranspose_CS");

	return true;
}

void GameApp::SetConstants(UINT level, UINT descendMask, UINT matrixWidth, UINT matrixHeight)
{
	CB cb = { level, descendMask, matrixWidth, matrixHeight };
	m_pd3dImmediateContext->UpdateSubresource(m_pConstantBuffer.Get(), 0, nullptr, &cb, 0, 0);
	m_pd3dImmediateContext->CSSetConstantBuffers(0, 1, m_pConstantBuffer.GetAddressOf());
}

void GameApp::GPUSort()
{
	UINT size = (UINT)m_RandomNums.size();

	m_pd3dImmediateContext->CSSetShader(m_pBitonicSort_CS.Get(), nullptr, 0);
	m_pd3dImmediateContext->CSSetUnorderedAccessViews(0, 1, m_pDataUAV1.GetAddressOf(), nullptr);

	// 按行数据进行排序，先排序level <= BLOCK_SIZE 的所有情况
	for (UINT level = 2; level <= size && level <= BITONIC_BLOCK_SIZE; level *= 2)
	{
		SetConstants(level, level, 0, 0);
		m_pd3dImmediateContext->Dispatch((size + BITONIC_BLOCK_SIZE - 1) / BITONIC_BLOCK_SIZE, 1, 1);
	}
	
	// 计算相近的矩阵宽高(宽>=高且需要都为2的次幂)
	UINT matrixWidth = 2, matrixHeight = 2;
	while (matrixWidth * matrixWidth < size)
	{
		matrixWidth *= 2;
	}
	matrixHeight = size / matrixWidth;

	// 排序level > BLOCK_SIZE 的所有情况
	ComPtr<ID3D11ShaderResourceView> pNullSRV;
	for (UINT level = BITONIC_BLOCK_SIZE * 2; level <= size; level *= 2)
	{
		// 如果达到最高等级，则为全递增序列
		if (level == size)
		{
			SetConstants(level / matrixWidth, level, matrixWidth, matrixHeight);
		}
		else
		{
			SetConstants(level / matrixWidth, level / matrixWidth, matrixWidth, matrixHeight);
		}
		// 先进行转置，并把数据输出到Buffer2
		m_pd3dImmediateContext->CSSetShader(m_pMatrixTranspose_CS.Get(), nullptr, 0);
		m_pd3dImmediateContext->CSSetShaderResources(0, 1, pNullSRV.GetAddressOf());
		m_pd3dImmediateContext->CSSetUnorderedAccessViews(0, 1, m_pDataUAV2.GetAddressOf(), nullptr);
		m_pd3dImmediateContext->CSSetShaderResources(0, 1, m_pDataSRV1.GetAddressOf());
		m_pd3dImmediateContext->Dispatch(matrixWidth / TRANSPOSE_BLOCK_SIZE, 
			matrixHeight / TRANSPOSE_BLOCK_SIZE, 1);

		// 对Buffer2排序列数据
		m_pd3dImmediateContext->CSSetShader(m_pBitonicSort_CS.Get(), nullptr, 0);
		m_pd3dImmediateContext->Dispatch(size / BITONIC_BLOCK_SIZE, 1, 1);

		// 接着转置回来，并把数据输出到Buffer1
		SetConstants(matrixWidth, level, matrixWidth, matrixHeight);
		m_pd3dImmediateContext->CSSetShader(m_pMatrixTranspose_CS.Get(), nullptr, 0);
		m_pd3dImmediateContext->CSSetShaderResources(0, 1, pNullSRV.GetAddressOf());
		m_pd3dImmediateContext->CSSetUnorderedAccessViews(0, 1, m_pDataUAV1.GetAddressOf(), nullptr);
		m_pd3dImmediateContext->CSSetShaderResources(0, 1, m_pDataSRV2.GetAddressOf());
		m_pd3dImmediateContext->Dispatch(matrixWidth / TRANSPOSE_BLOCK_SIZE,
			matrixHeight / TRANSPOSE_BLOCK_SIZE, 1);

		// 对Buffer1排序剩余行数据
		m_pd3dImmediateContext->CSSetShader(m_pBitonicSort_CS.Get(), nullptr, 0);
		m_pd3dImmediateContext->Dispatch(size / BITONIC_BLOCK_SIZE, 1, 1);
	}
}



