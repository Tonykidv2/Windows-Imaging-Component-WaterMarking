#include "Defines.h"
#include <iostream>
#include <wincodec.h>
#include <fstream>
#include <vector>
using namespace std;


#define SafeRelease(X) {if(X) {X->Release(); X = nullptr;}}

HRESULT LoadBitmapFromFile(
	const wchar_t *uri,
	unsigned int destinationWidth,
	unsigned int destinationHeight,
	IWICBitmapFrameDecode** bitmapSource);

HRESULT SaveBitmapToFile(IWICBitmap* updatedBitmap,
	const wchar_t *uriOriginalFile,
	const wchar_t *uriUpdatedFile);

HRESULT GetBitmapFromSource(IWICBitmapSource* wicBitmapSorce,
	IWICBitmap** Bitmap);

HRESULT GetBitmapFromScaler(IWICBitmapScaler* wicBitmapSource,
	IWICBitmap** Bitmap);

HRESULT LoadScaledBitmapFromFile(
	const wchar_t *uri,
	unsigned int destinationWidth,
	unsigned int destinationHeight,
	IWICBitmap** updatedBitmap);

inline BYTE ByteLerp(int b1, int b2, float percent)
{
	return BYTE(b1 + (b2 + b1) * percent);
}

int main(int argc, char* argv[])
{
	/*if (argc != 2)
	{
		cout << "Incorrect Parameters" << endl;
		system("pause");
		return -1;
	}*/
	float opacity = 25.0f;
	int Scale = 2;
	IWICBitmapFrameDecode* bitmapBaseSource = nullptr;
	IWICBitmapFrameDecode* bitmapScaleSource = nullptr;
	IWICBitmap *pIBaseBitmap = nullptr;
	IWICBitmap *pIScaledBitmap = nullptr;


	CoInitialize(nullptr);
	HRESULT hr = S_OK;
	/////////////Getting backimage Image
	hr = LoadBitmapFromFile(L"..\\Resources\\butterfly.bmp", 0, 0, &bitmapBaseSource);
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
	hr = LoadBitmapFromFile(L"..\\Resources\\butterfly.bmp", 0, 0, &bitmapScaleSource);
	if (!SUCCEEDED(hr))
	{
		cout << "Something went wrong!!!" << endl;
		system("pause");
		return -1;
	}
	hr = LoadScaledBitmapFromFile(L"..\\Resources\\butterfly.bmp", width/Scale, height/Scale, &pIScaledBitmap);
	if (!SUCCEEDED(hr))
	{
		cout << "Something went wrong!!!" << endl;
		system("pause");
		return -1;
	}
	UINT WaterMarkWidth;
	UINT WaterMarkHeight;
	pIScaledBitmap->GetSize(&WaterMarkWidth, &WaterMarkHeight);
	//hr = SaveBitmapToFile(pIScaledBitmap, L"..\\Resources\\butterfly.bmp", L"OutputScaled.png");
	//if (!SUCCEEDED(hr))
	//{
	//	cout << "Something went wrong!!!" << endl;
	//	system("pause");
	//	return -1;
	//}
	//SafeRelease(pIScaledBitmap);
	//SafeRelease(bitmapScaleSource);
	//hr = LoadBitmapFromFile(L"OutputScaled.png", 0, 0, &bitmapScaleSource);
	//if (!SUCCEEDED(hr))
	//{
	//	cout << "Something went wrong!!!" << endl;
	//	system("pause");
	//	return -1;
	//}
	//hr = GetBitmapFromSource(bitmapScaleSource, &pIScaledBitmap);
	//if (!SUCCEEDED(hr))
	//{
	//	cout << "Something went wrong!!!" << endl;
	//	system("pause");
	//	return -1;
	//}
	//pIScaledBitmap->GetSize(&WaterMarkWidth, &WaterMarkHeight);
	////I don't need it anymore
	//remove("OutputScaled.png");
#pragma region minuplulate pixel data
	/*WICPixelFormatGUID pixelFormt;
	IWICBitmapSource* iBitmap;
	hr = WICConvertBitmapSource(GUID_WICPixelFormat32bppBGRA, bitmapBaseSource, &iBitmap);
	iBitmap->GetPixelFormat(&pixelFormt);
	IWICBitmap* formatedBitmap;
	GetBitmapFromSource(iBitmap, &formatedBitmap);*/


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
	////////////

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

	//this is good to straight blit
	for (size_t x = 0; x < uiScalesWidth; x++)
	{
		for (size_t y = 0; y < uiScaledHeight; y+=3)
		{
			if ((x * cbScaledStride) + y >= cbScaledBufferSize)
				break;
			//B,G,R//
			//pv[(x * cbStride) + y]	 = 0;
			//pv[(x * cbStride) + y + 1] = 0;
			//pv[(x * cbStride) + y + 2] = 255;
			//pv[(x * cbStride) + y + 3] = 255;
			pv[(x * cbStride) + y]	 = pvScaled[(x * cbScaledStride) + y] ;
			pv[(x * cbStride) + y + 1] = pvScaled[(x * cbScaledStride) + y + 1];
			pv[(x * cbStride) + y + 2] = pvScaled[(x * cbScaledStride) + y + 2];
			//pv[(x * cbStride) + y + 3] = 255;
		}
	}

	

	pIScaledLock->Release();
	pILock->Release();
	hr = SaveBitmapToFile(pIBaseBitmap, L"..\\Resources\\butterfly.bmp", L"Output.png");
	hr = SaveBitmapToFile(pIScaledBitmap, L"..\\Resources\\butterfly.bmp", L"OutputScaled.png");

#pragma endregion

	CoUninitialize();
	/*pIBitmap->Release();
	bitmapSource->Release();*/
	return hr;
}

HRESULT LoadBitmapFromFile(
	const wchar_t *uri,
	unsigned int destinationWidth,
	unsigned int destinationHeight,
	IWICBitmapFrameDecode** bitmapSource)
{
	HRESULT hr = S_OK;

	IWICBitmapDecoder* decoder = nullptr;
	IWICBitmapEncoder* encoder = nullptr;
	IWICFormatConverter* converter = nullptr;
	IWICBitmapScaler* scaler = nullptr;
	IWICImagingFactory* wicFactory = nullptr;

	
	hr = CoCreateInstance(
		CLSID_WICImagingFactory, nullptr,
		CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&wicFactory);
	

	if (SUCCEEDED(hr))
	{
		hr = wicFactory->CreateDecoderFromFilename(
			uri,
			nullptr,
			GENERIC_READ,
			WICDecodeMetadataCacheOnDemand,
			&decoder);
	}

	if (SUCCEEDED(hr))
	{
		// Create the initial frame.
		hr = decoder->GetFrame(0, bitmapSource);
	}

	if (SUCCEEDED(hr))
	{
		hr = wicFactory->CreateFormatConverter(&converter);
	}

	if (SUCCEEDED(hr))
	{
		// If a new width or height was specified, create an
		// IWICBitmapScaler and use it to resize the image.
		if (destinationWidth != 0 || destinationHeight != 0)
		{
			unsigned int originalWidth, originalHeight;
			hr = (*bitmapSource)->GetSize(&originalWidth, &originalHeight);
			if (SUCCEEDED(hr))
			{
				if (destinationWidth == 0)
				{
					float scalar = static_cast<float>(destinationHeight) / static_cast<float>(originalHeight);
					destinationWidth = static_cast<unsigned int>(scalar * static_cast<float>(originalWidth));
				}
				else if (destinationHeight == 0)
				{
					float scalar = static_cast<float>(destinationWidth) / static_cast<float>(originalWidth);
					destinationHeight = static_cast<unsigned int>(scalar * static_cast<float>(originalHeight));
				}

				hr = wicFactory->CreateBitmapScaler(&scaler);
				if (SUCCEEDED(hr))
				{
					hr = scaler->Initialize(
						*bitmapSource,
						destinationWidth,
						destinationHeight,
						WICBitmapInterpolationModeLinear);
				}
				if (SUCCEEDED(hr))
				{
					hr = converter->Initialize(
						scaler,
						GUID_WICPixelFormat24bppBGR,
						WICBitmapDitherTypeNone,
						nullptr,
						0.f,
						WICBitmapPaletteTypeMedianCut);
				}
			}
		}
		else // Don't scale the image.
		{
			hr = converter->Initialize(
				*bitmapSource,
				GUID_WICPixelFormat24bppBGR,
				WICBitmapDitherTypeNone,
				nullptr,
				0.f,
				WICBitmapPaletteTypeMedianCut);
		}
	}
	
	SafeRelease(wicFactory);
	SafeRelease(converter);
	SafeRelease(scaler);
	SafeRelease(decoder);

	return hr;
}

HRESULT SaveBitmapToFile(IWICBitmap* updatedBitmap,
	const wchar_t *uriOriginalFile,
	const wchar_t *uriUpdatedFile)
	{
		unsigned int frameCount = 0;
		unsigned int width = 0;
		unsigned int height = 0;

		bool isUsingTempFile = false;

		std::wstring outputFilePath;
		GUID containerFormat = GUID_ContainerFormatBmp;
		WICPixelFormatGUID pixelFormat = GUID_WICPixelFormatDontCare;

		IWICImagingFactory* factory = nullptr;
		IWICStream* stream = nullptr;
		IStream* sharedStream = nullptr;
		IWICBitmapDecoder* decoder = nullptr;
		IWICBitmapEncoder* encoder = nullptr;
		IWICMetadataBlockWriter* blockWriter = nullptr;
		IWICMetadataBlockReader* blockReader = nullptr;

		HRESULT hr = hr = CoCreateInstance(
			CLSID_WICImagingFactory, nullptr,
			CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&factory);

		if (SUCCEEDED(hr))
		{
			factory->CreateDecoderFromFilename(uriOriginalFile, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);
			
		}

		if (SUCCEEDED(hr))
		{
			hr = decoder->GetFrameCount(&frameCount);
			
		}

		// Calculate temporary file name if an updated file was not specified
		if (nullptr == uriUpdatedFile)
		{
			// Indicate that we're using a tempoary file for saving
			isUsingTempFile = true;

			// Get location of temporary folder
			wchar_t tempFilePathBuffer[MAX_PATH];
			unsigned long count = GetTempPath(MAX_PATH, tempFilePathBuffer);

			if (count > MAX_PATH || count == 0)
			{
				// Unable to get temporary path (use current directory)
				outputFilePath.append(uriOriginalFile);
			}
			else
			{
				// Get file name by itself
				outputFilePath.append(uriOriginalFile);
				outputFilePath = outputFilePath.substr(outputFilePath.find_last_of('\\') + 1);

				// Insert temporary folder before the file name
				outputFilePath.insert(0, tempFilePathBuffer);
			}
		}
		else
		{
			outputFilePath.append(uriUpdatedFile);
		}

		// File extension to determine which container format to use for the output file
		std::wstring fileExtension(outputFilePath.substr(outputFilePath.find_last_of('.')));

		// Convert all characters to lower case
		std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), tolower);

		if (SUCCEEDED(hr))
		{
			// Default value is bitmap encoding
			if (fileExtension.compare(L".jpg") == 0 ||
				fileExtension.compare(L".jpeg") == 0 ||
				fileExtension.compare(L".jpe") == 0 ||
				fileExtension.compare(L".jfif") == 0)
			{
				containerFormat = GUID_ContainerFormatJpeg;
			}
			else if (fileExtension.compare(L".tif") == 0 ||
				fileExtension.compare(L".tiff") == 0)
			{
				containerFormat = GUID_ContainerFormatTiff;
			}
			else if (fileExtension.compare(L".gif") == 0)
			{
				containerFormat = GUID_ContainerFormatGif;
			}
			else if (fileExtension.compare(L".png") == 0)
			{
				containerFormat = GUID_ContainerFormatPng;
			}
			else if (fileExtension.compare(L".wmp") == 0)
			{
				containerFormat = GUID_ContainerFormatWmp;
			}
			else if (fileExtension.compare(L".bmp") == 0)
			{
				containerFormat = GUID_ContainerFormatBmp;
			}
			hr = factory->CreateEncoder(containerFormat, nullptr, &encoder);
		}

		if (SUCCEEDED(hr))
		{
			// Create a stream for the encoder
			hr = factory->CreateStream(&stream);
		}

		if (SUCCEEDED(hr))
		{
			// Update temporary file name if needed
			if (isUsingTempFile)
			{
				outputFilePath.append(L".tmp");
			}

			// Initialize the stream using the output file path
			hr = stream->InitializeFromFilename(outputFilePath.c_str(), GENERIC_WRITE);
		}

		if (SUCCEEDED(hr))
		{
			// Create encoder to write to image file
			hr = encoder->Initialize(stream, WICBitmapEncoderNoCache);
		}

		// Process each frame
		for (unsigned int i = 0; i < frameCount && SUCCEEDED(hr); i++)
		{
			//Frame variables
			IWICBitmapFrameDecode* frameDecode;
			IWICBitmapFrameEncode* frameEncode;
			IWICMetadataQueryReader* frameQueryReader;
			IWICMetadataQueryWriter* frameQueryWriter;

			//Get and create image frame
			if (SUCCEEDED(hr))
			{
				hr = decoder->GetFrame(i, &frameDecode);
			}

			if (SUCCEEDED(hr))
			{
				hr = encoder->CreateNewFrame(&frameEncode, nullptr);
			}

			//Initialize the encoder
			if (SUCCEEDED(hr))
			{
				hr = frameEncode->Initialize(nullptr);
			}

			//Get and set size
			if (SUCCEEDED(hr))
			{
				if (i == 0)
				{
					hr = updatedBitmap->GetSize(&width, &height);
				}
				else
				{
					hr = frameDecode->GetSize(&width, &height);
				}
			}

			if (SUCCEEDED(hr))
			{
				hr = frameEncode->SetSize(width, height);
			}

			//Set pixel format
			if (SUCCEEDED(hr))
			{
				frameDecode->GetPixelFormat(&pixelFormat);
			}

			if (SUCCEEDED(hr))
			{
				hr = frameEncode->SetPixelFormat(&pixelFormat);
			}

			//Check that the destination format and source formats are the same
			bool formatsEqual = false;

			if (SUCCEEDED(hr))
			{
				GUID srcFormat;
				GUID destFormat;

				hr = decoder->GetContainerFormat(&srcFormat);

				if (SUCCEEDED(hr))
				{
					hr = encoder->GetContainerFormat(&destFormat);
				}

				if (SUCCEEDED(hr))
				{
					formatsEqual = (srcFormat == destFormat) ? true : false;
				}
			}

			if (SUCCEEDED(hr) && formatsEqual)
			{
				//Copy metadata using metadata block reader/writer
				frameDecode->QueryInterface(&blockReader);
				frameEncode->QueryInterface(&blockWriter);
				
				if (nullptr != blockReader && nullptr != blockWriter)
				{
					blockWriter->InitializeFromBlockReader(blockReader);
				}
			}

			if (SUCCEEDED(hr))
			{
				if (i == 0)
				{
					// Copy updated bitmap to output
					hr = frameEncode->WriteSource(updatedBitmap, nullptr);
				}
				else
				{
					// Copy existing image to output
					hr = frameEncode->WriteSource(static_cast<IWICBitmapSource*>(frameDecode), nullptr);
				}
			}

			//Commit the frame
			if (SUCCEEDED(hr))
			{
				hr = frameEncode->Commit();
			}
		}

		if (SUCCEEDED(hr))
		{
			encoder->Commit();
		}

		// Ensure that the input and output files are not locked by releasing corresponding objects
		if (stream)
		{
			SafeRelease(stream);
		}

		if (decoder)
		{
			SafeRelease(decoder);
		}

		if (encoder)
		{
			SafeRelease(encoder);
		}

		if (blockWriter)
		{
			SafeRelease(blockWriter);
		}

		if (blockReader)
		{
			SafeRelease(blockReader);
		}

		if (SUCCEEDED(hr) && isUsingTempFile)
		{
			// Move temporary file to current file
			if (!::CopyFileW(outputFilePath.c_str(), uriOriginalFile, false))
			{
				hr = E_FAIL;
			}

			// Delete the temporary file
			::DeleteFileW(outputFilePath.c_str());
		}

		return hr;
	}

HRESULT GetBitmapFromSource(IWICBitmapSource* wicBitmapSorce,
	IWICBitmap** Bitmap)
	{
		IWICImagingFactory* wicfactory = nullptr;

		HRESULT hr = CoCreateInstance(
			CLSID_WICImagingFactory, nullptr,
			CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&wicfactory);

		if (SUCCEEDED(hr))
		{
			hr = wicfactory->CreateBitmapFromSource(
				wicBitmapSorce,				// Create a bitmap from the image frame
				WICBitmapCacheOnDemand,		// Cache metadata when needed
				Bitmap);					// Pointer to the bitmap
		}

		SafeRelease(wicfactory);

		return hr;
	}

HRESULT GetBitmapFromScaler(IWICBitmapScaler* wicBitmapSource,
	IWICBitmap** Bitmap)
{
	IWICImagingFactory* wicfactory = nullptr;

	HRESULT hr = CoCreateInstance(
		CLSID_WICImagingFactory, nullptr,
		CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&wicfactory);

	if (SUCCEEDED(hr))
	{
		hr = wicfactory->CreateBitmapFromSource(
			wicBitmapSource,				// Create a bitmap from the image frame
			WICBitmapCacheOnDemand,		// Cache metadata when needed
			Bitmap);					// Pointer to the bitmap
	}

	SafeRelease(wicfactory);

	return hr;
}

HRESULT LoadScaledBitmapFromFile(
	const wchar_t *uri,
	unsigned int destinationWidth,
	unsigned int destinationHeight,
	IWICBitmap** updatedBitmap)
{

	IWICBitmapFrameDecode* bitmapSource = nullptr;
	IWICImagingFactory* wicFactory = nullptr;
	IWICBitmapScaler* scaler = nullptr;

	HRESULT hr;

	hr = CoCreateInstance(
		CLSID_WICImagingFactory, nullptr,
		CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&wicFactory);

	if (SUCCEEDED(hr))
	{
		hr = LoadBitmapFromFile(uri, 0, 0, &bitmapSource);
	}
	if (SUCCEEDED(hr))
	{
		hr = wicFactory->CreateBitmapScaler(&scaler);
	}
	if (SUCCEEDED(hr))
	{
		hr = scaler->Initialize(
			bitmapSource,
			destinationWidth,
			destinationHeight,
			WICBitmapInterpolationModeLinear);
	}
	if (SUCCEEDED(hr))
	{
		hr = GetBitmapFromScaler(scaler, updatedBitmap);
	}

	SafeRelease(wicFactory);
	SafeRelease(scaler);
	SafeRelease(bitmapSource);
	return hr;
}

