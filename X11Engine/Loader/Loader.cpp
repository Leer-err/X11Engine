#include "Loader.h"

#include <memory>
#include <filesystem>
#include <tchar.h>

using std::unique_ptr;

ComPtr<ID3D11Texture2D> Loader::LoadTextureFromFile(const char* filename)
{
	ComPtr<IWICImagingFactory> factory;
	ComPtr<IWICBitmapDecoder> decoder;
	ComPtr<IWICBitmapFrameDecode> frameDecoder;
	WICPixelFormatGUID format;
	UINT width, height;

	CoInitialize(NULL);

	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory));

	int size = strlen(filename) + 1;
	wchar_t* wpath = new wchar_t[size];
	mbstowcs(wpath, filename, size);

	factory->CreateDecoderFromFilename(wpath, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);

	decoder->GetFrame(0, &frameDecoder);

	frameDecoder->GetSize(&width, &height);

	unique_ptr<char[]> buffer(new char [height * width * 4]);
	frameDecoder->GetPixelFormat(&format);
	if (format == GUID_WICPixelFormat32bppBGRA) {
		hr = frameDecoder->CopyPixels(nullptr, width * 4, width * height * 4, reinterpret_cast<BYTE*>(buffer.get()));
		return Graphics::get().CreateTexture(width, height, buffer.get());
	}
	return {};
}

Material Loader::LoadMaterial(const aiMaterial* material)
{
	if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
		aiString path;
		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
			return { LoadTextureFromFile(path.data) };
		}
	}
}

Model Loader::LoadModelFromFile(const char* filename)
{
	Model model;
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename,
		aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

	for (int i = 0; i < scene->mNumMeshes; i++) {
		vector<vertex> vert;
		vector<uint32_t> ind;

		const aiMesh* pMesh = scene->mMeshes[i];

		for (int j = 0; j < pMesh->mNumVertices; j++) {
			const aiVector3D pos = pMesh->mVertices[j];
			const aiVector3D uv = pMesh->mTextureCoords[0][j];
			vert.emplace_back(vector3{ pos.x, pos.y, pos.z }, vector2{ uv.x, uv.y });
		}
		for (int j = 0; j < pMesh->mNumFaces; j++) {
			const aiFace face = pMesh->mFaces[j];
			ind.push_back(face.mIndices[0]);
			ind.push_back(face.mIndices[1]);
			ind.push_back(face.mIndices[2]);
		}

		model.meshes.emplace_back(vert, ind, pMesh->mMaterialIndex);
	}

	for (int i = 0; i < scene->mNumMaterials; i++) {
		const aiMaterial* pMaterial = scene->mMaterials[i];
		model.materials.push_back(LoadMaterial(pMaterial));
	}

	return model;
}
