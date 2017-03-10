#pragma once

inline BYTE ByteLerp(int b1, int b2, float percent)
{
	BYTE returning = BYTE((b2 - b1) * percent + b1);
	return returning;
}

HRESULT LoadBitmapFromFile(
	const wchar_t *uri,
	unsigned int destinationWidth,
	unsigned int destinationHeight,
	IWICBitmapSource** bitmapSource);

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

void Greeting();
void CheckOpacity(float& _opacity);
void CheckScale(int& _scale);
void CheckIgnoreColor(bool& _ignoring, int& _r, int& _g, int& _b);