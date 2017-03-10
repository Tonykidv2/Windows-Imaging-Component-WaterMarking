#include "Defines.h"


int main(int argc, char* argv[])
{
	Greeting();
	if (argc != 3)
	{
		cout << "Incorrect Parameters" << endl;
		system("pause");
		return -1;
	}

	cout << "Images Recieved" << endl;
	float opacity;
	int Scale;
	bool Ignoreacolor = false;
	int B, G, R;
	CheckOpacity(opacity);
	CheckIgnoreColor(Ignoreacolor, R, G, B);
	CheckScale(Scale);

	//making a ratio
	opacity /= 100;
	cout << "Input Confirm Please Wait....." << endl;

	IWICBitmapSource* bitmapBaseSource = nullptr;
	IWICBitmapSource* bitmapScaleSource = nullptr;
	IWICBitmap *pIBaseBitmap = nullptr;
	IWICBitmap *pIScaledBitmap = nullptr;


	CoInitialize(nullptr);
	HRESULT hr = S_OK;
	/////////////Getting backimage Image
	size_t cSize = strlen(argv[1]) + 1;
	wchar_t wc[255];
	mbstowcs(wc, argv[1], cSize);
	hr = LoadBitmapFromFile(wc, 0, 0, &bitmapBaseSource);
	if (!SUCCEEDED(hr))
	{
		cout << "Something went wrong!!!" << endl;
		system("pause");
		return -1;
	}
	//Getting bitmap
	hr = GetBitmapFromSource(bitmapBaseSource, &pIBaseBitmap);
	if (!SUCCEEDED(hr))
	{
		cout << "Something went wrong!!!" << endl;
		system("pause");
		return -1;
	}
	UINT width;
	UINT height;
	hr = pIBaseBitmap->GetSize(&width, &height);

	////////////Getting Scaled WaterMark
	UINT WaterMarkWidth;
	UINT WaterMarkHeight;
	cSize = strlen(argv[2]) + 1;
	mbstowcs(wc, argv[2], cSize);
	hr = LoadBitmapFromFile(wc, 0, 0, &bitmapScaleSource);
	if (!SUCCEEDED(hr))
	{
		cout << "Something went wrong!!!" << endl;
		system("pause");
		return -1;
	}
	bitmapScaleSource->GetSize(&WaterMarkWidth, &WaterMarkHeight);
	if (WaterMarkWidth >= width || WaterMarkHeight >= height)
	{
		hr = LoadScaledBitmapFromFile(/*L"..\\Resources\\Mountain.png"*/wc, width / Scale, height / Scale, &pIScaledBitmap);
		if (!SUCCEEDED(hr))
		{
			cout << "Something went wrong!!!" << endl;
			system("pause");
			return -1;
		}
	}
	else
	{
		hr = LoadScaledBitmapFromFile(/*L"..\\Resources\\Mountain.png"*/wc, WaterMarkWidth, WaterMarkWidth, &pIScaledBitmap);
		if (!SUCCEEDED(hr))
		{
			cout << "Something went wrong!!!" << endl;
			system("pause");
			return -1;
		}
	}
	pIScaledBitmap->GetSize(&WaterMarkWidth, &WaterMarkHeight);

#pragma region minuplulate pixel data
	WICPixelFormatGUID pixelFormt;
	bitmapBaseSource->GetPixelFormat(&pixelFormt);
	pIScaledBitmap->GetPixelFormat(&pixelFormt);

	/////////Getting Pixel Data from Base image
	IWICBitmapLock *pILock = nullptr;
	UINT uiWidth = WaterMarkWidth;
	UINT uiHeight = WaterMarkHeight;
	UINT cbStride;
	WICRect rcLock = { 0, 0, uiWidth, uiHeight };
	hr = pIBaseBitmap->Lock(&rcLock, WICBitmapLockWrite, &pILock);
	if (!SUCCEEDED(hr))
	{
		cout << "Something went wrong!!!" << endl;
		system("pause");
		return -1;
	}
	UINT cbBufferSize = 0;
	BYTE *pv = NULL;
	hr = pILock->GetDataPointer(&cbBufferSize, &pv);
	hr = pILock->GetStride(&cbStride);
	///////////////////////////////////////////

	/////////Getting Pixel Data from the WaterMark(Scaled) image
	IWICBitmapLock *pIScaledLock = nullptr;
	UINT uiScalesWidth = WaterMarkWidth;
	UINT uiScaledHeight = WaterMarkHeight;
	UINT cbScaledStride;
	WICRect rcScaledLock = { 0, 0, uiScalesWidth, uiScaledHeight };
	hr = pIScaledBitmap->Lock(&rcScaledLock, WICBitmapLockWrite, &pIScaledLock);
	if (!SUCCEEDED(hr))
	{
		cout << "Something went wrong!!!" << endl;
		system("pause");
		return -1;
	}
	UINT cbScaledBufferSize = 0;
	BYTE *pvScaled = NULL;
	hr = pIScaledLock->GetDataPointer(&cbScaledBufferSize, &pvScaled);
	hr = pIScaledLock->GetStride(&cbScaledStride);
	/////////////////////////////////////////////

	//this is good to straight blit image
	for (size_t x = 0; x < uiScalesWidth * 3; x+=3)
	{
		for (size_t y = 0; y < uiScaledHeight; y++)
		{
			if ((int)pvScaled[(y * cbScaledStride) + x + 3] == 0.0f)
				continue;
			if (pvScaled[(y * cbScaledStride) + x] == B && Ignoreacolor)
				if (pvScaled[(y * cbScaledStride) + x] == G && Ignoreacolor)
					if (pvScaled[(y * cbScaledStride) + x] == R && Ignoreacolor)
						continue;

			pv[(y * cbStride) + x]	   = ByteLerp((int)pv[(y * cbStride) + x], (int)pvScaled[(y * cbScaledStride) + x], opacity);
			pv[(y * cbStride) + x + 1] = ByteLerp((int)pv[(y * cbStride) + x + 1], (int)pvScaled[(y * cbScaledStride) + x + 1], opacity);
			pv[(y * cbStride) + x + 2] = ByteLerp((int)pv[(y * cbStride) + x + 2], (int)pvScaled[(y * cbScaledStride) + x + 2], opacity);
			pv[(y * cbStride) + x + 3] = ByteLerp((int)pv[(y * cbStride) + x + 3], (int)pvScaled[(y * cbScaledStride) + x + 3], opacity);
		}
	}

	

	pIScaledLock->Release();
	pILock->Release();
	cSize = strlen(argv[1]) + 1;
	mbstowcs(wc, argv[1], cSize);
	hr = SaveBitmapToFile(pIBaseBitmap, /*L"..\\Resources\\butterfly.bmp"*/wc, L"Output.png");
	//hr = SaveBitmapToFile(pIScaledBitmap, L"..\\Resources\\Mountain.png", L"OutputScaled.png");

#pragma endregion
	
	CoUninitialize();
	cout << "Process Complete Please Look for image file called Output.xxx..." << endl;
	system("pause");
	/*pIBitmap->Release();
	bitmapSource->Release();*/
	return hr;
}

