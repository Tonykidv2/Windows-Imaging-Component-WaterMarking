#include "Defines.h"
#include "HelperFunctions.h"

HRESULT LoadBitmapFromFile(
	const wchar_t *uri,
	unsigned int destinationWidth,
	unsigned int destinationHeight,
	IWICBitmapSource** bitmapSource)
{
	HRESULT hr = S_OK;

	IWICBitmapDecoder* decoder = nullptr;
	IWICBitmapEncoder* encoder = nullptr;
	IWICFormatConverter* converter = nullptr;
	IWICBitmapScaler* scaler = nullptr;
	IWICImagingFactory* wicFactory = nullptr;
	IWICBitmapFrameDecode* bitmapFrame = nullptr;

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
		hr = decoder->GetFrame(0, &bitmapFrame);
	}
	if (SUCCEEDED(hr))
	{
		//Making Sure Image has Alpha Channel
		hr = WICConvertBitmapSource(GUID_WICPixelFormat32bppBGRA, bitmapFrame, bitmapSource);
	}
	//if (SUCCEEDED(hr))
	//{
	//	hr = wicFactory->CreateFormatConverter(&converter);
	//}
	//
	//if (SUCCEEDED(hr))
	//{
	//	// If a new width or height was specified, create an
	//	// IWICBitmapScaler and use it to resize the image.
	//	if (destinationWidth != 0 || destinationHeight != 0)
	//	{
	//		unsigned int originalWidth, originalHeight;
	//		hr = (*bitmapSource)->GetSize(&originalWidth, &originalHeight);
	//		if (SUCCEEDED(hr))
	//		{
	//			if (destinationWidth == 0)
	//			{
	//				float scalar = static_cast<float>(destinationHeight) / static_cast<float>(originalHeight);
	//				destinationWidth = static_cast<unsigned int>(scalar * static_cast<float>(originalWidth));
	//			}
	//			else if (destinationHeight == 0)
	//			{
	//				float scalar = static_cast<float>(destinationWidth) / static_cast<float>(originalWidth);
	//				destinationHeight = static_cast<unsigned int>(scalar * static_cast<float>(originalHeight));
	//			}
	//
	//			hr = wicFactory->CreateBitmapScaler(&scaler);
	//			if (SUCCEEDED(hr))
	//			{
	//				hr = scaler->Initialize(
	//					*bitmapSource,
	//					destinationWidth,
	//					destinationHeight,
	//					WICBitmapInterpolationModeLinear);
	//			}
	//			if (SUCCEEDED(hr))
	//			{
	//				hr = converter->Initialize(
	//					scaler,
	//					/*GUID_WICPixelFormat24bppBGR*/GUID_WICPixelFormat32bppBGRA,
	//					WICBitmapDitherTypeNone,
	//					nullptr,
	//					0.f,
	//					WICBitmapPaletteTypeMedianCut);
	//			}
	//		}
	//	}
	//	else // Don't scale the image.
	//	{
	//		hr = converter->Initialize(
	//			*bitmapSource,
	//			/*GUID_WICPixelFormat24bppBGR*/GUID_WICPixelFormat32bppBGRA,
	//			WICBitmapDitherTypeNone,
	//			nullptr,
	//			0.f,
	//			WICBitmapPaletteTypeMedianCut);
	//	}
	//	WICConvertBitmapSource(GUID_WICPixelFormat32bppBGRA, *bitmapSource, (*bitmapSource));
	//}

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
		/*IWICMetadataQueryReader* frameQueryReader;
		IWICMetadataQueryWriter* frameQueryWriter;*/

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

	IWICBitmapSource* bitmapSource = nullptr;
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

void Greeting()
{
	cout << "//////////////////////////////////////////////////////////////////" << endl;
	cout << "//                                                              //" << endl;
	cout << "//                                                              //" << endl;
	cout << "//                         Welcome User                         //" << endl;
	cout << "//           Window Imaging Component WaterMark App             //" << endl;
	cout << "//                         Version 0.2.1                        //" << endl;
	cout << "//                    Creator: Anthony Ramos                    //" << endl;
	cout << "//             Please Read the ReadMe.txt for Help              //" << endl;
	cout << "//                                                              //" << endl;
	cout << "//                                                              //" << endl;
	cout << "//////////////////////////////////////////////////////////////////" << endl;
	system("pause");
	cout << endl;
}

void CheckOpacity(float & _opacity)
{

	cout << "Please Enter opacity Level between 0 - 100:  ";
	cin >> _opacity;
	while (true)
	{
		if (_opacity >= 0 && _opacity <= 100)
		{
			break;
		}
		cout << endl;
		cin.ignore();
		cout << "Please Enter opacity Level between 0 - 100:  ";
		cin >> _opacity;
	}
}

void CheckScale(int & _scale)
{
	cout << "*Image is currently stretched to the base image if the waterMark image is bigger than the base Image*" << endl;
	cout << "Please Enter Scale Level between 0 - 100:  ";
	cin >> _scale;
	while (true)
	{
		if (_scale >= 0 && _scale <= 100)
		{
			break;
		}
		cin.ignore();
		cout << "Please Enter Scale Level between 0 - 100:  ";
		cin >> _scale;
	}
}

void CheckIgnoreColor(bool & _ignoring, int & _r, int & _g, int & _b)
{
	char input;
	cout << "Would you like to ignore a Color Y/N: ";
	cin >> input;
	while (true)
	{
		if (input == 'y' || input == 'Y')
		{
			_ignoring = true;
			break;
		}
		if (input == 'n' || input == 'N')
		{
			_ignoring = false;
			break;
		}
		cout << endl;
		cin.ignore();
		cout << "Would you like to ignore a Color Y/N: ";
		cin >> input;
	}

	while (_ignoring)
	{
		cout << "Please Enter R Value 0 - 255: ";
		cin >> _r;
		if (_r >= 0 && _r <= 255)
			break;
		cout << endl;
		cin.ignore();
	}
	while (_ignoring)
	{
		cout << "Please Enter G Value 0 - 255: ";
		cin >> _g;
		if (_g >= 0 && _g <= 255)
			break;
		cout << endl;
		cin.ignore();
	}
	while (_ignoring)
	{
		cout << "Please Enter B Value 0 - 255: ";
		cin >> _b;
		if (_b >= 0 && _b <= 255)
			break;
		cout << endl;
		cin.ignore();
	}
}