#include "Loader.h"

Texture Loader::LoadTextureFromFile(const wchar_t* filename)
{
	ComPtr<IWICImagingFactory> factory;
	ComPtr<IWICBitmapDecoder> decoder;
	ComPtr<IWICBitmapFrameDecode> frameDecoder;
	WICPixelFormatGUID format;
	UINT width, height;

	CoInitialize(NULL);

	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory));

	factory->CreateDecoderFromFilename(filename, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);

	decoder->GetFrame(0, &frameDecoder);

	frameDecoder->GetSize(&width, &height);

	unique_ptr<char[]> buffer(new char [height * width * 4]);
	frameDecoder->GetPixelFormat(&format);
	if (format == GUID_WICPixelFormat32bppBGRA) {
		hr = frameDecoder->CopyPixels(nullptr, width * 4, width * height * 4, reinterpret_cast<BYTE*>(buffer.get()));
		return { (int)width, (int)height, buffer.get() };
	}
	return Texture();
}