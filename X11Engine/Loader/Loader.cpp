#include "Loader.h"

#include <memory>

using std::unique_ptr;

ComPtr<ID3D11ShaderResourceView> Loader::LoadTextureFromFile(const char* filename)
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

	hr = factory->CreateDecoderFromFilename(wpath, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);

	decoder->GetFrame(0, &frameDecoder);

	frameDecoder->GetSize(&width, &height);

	unique_ptr<char[]> buffer(new char [height * width * 4]);
	frameDecoder->GetPixelFormat(&format);
	if (format == GUID_WICPixelFormat32bppBGRA) {
		hr = frameDecoder->CopyPixels(nullptr, width * 4, width * height * 4, reinterpret_cast<BYTE*>(buffer.get()));
		return Graphics::get().CreateShaderResource(DXGI_FORMAT_B8G8R8A8_UNORM, width, height, buffer.get());
	}
	return {};
}

Material Loader::LoadMaterial(const aiMaterial* material)
{
	Material mat;
	if (material->GetTextureCount(aiTextureType_BASE_COLOR) > 0) {
		aiString relPath;
		if (material->GetTexture(aiTextureType_BASE_COLOR, 0, &relPath) == AI_SUCCESS) {
			string path = currentPath + "\\aseets\\" + relPath.data;
			mat.baseColor = LoadTextureFromFile(path.c_str());
		}
	}
	else {
		string path = currentPath + "\\aseets\\TexturePlaceholder.png";
		mat.baseColor = LoadTextureFromFile(path.c_str());
	}

	if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
		aiString relPath;
		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &relPath) == AI_SUCCESS) {
			string path = currentPath + "\\aseets\\" + relPath.data;
			mat.diffuse = LoadTextureFromFile(path.c_str());
		}
	}
	else {
		string path = currentPath + "\\aseets\\WhitePlaceholder.png";
		mat.diffuse = LoadTextureFromFile(path.c_str());
	}

	if (material->GetTextureCount(aiTextureType_SPECULAR) > 0) {
		aiString relPath;
		if (material->GetTexture(aiTextureType_SPECULAR, 0, &relPath) == AI_SUCCESS) {
			string path = currentPath + "\\aseets\\" + relPath.data;
			mat.specular = LoadTextureFromFile(path.c_str());
		}
	}
	else {
		string path = currentPath + "\\aseets\\WhitePlaceholder.png";
		mat.specular = LoadTextureFromFile(path.c_str());
	}

	if (material->GetTextureCount(aiTextureType_EMISSIVE) > 0) {
		aiString relPath;
		if (material->GetTexture(aiTextureType_EMISSIVE, 0, &relPath) == AI_SUCCESS) {
			string path = currentPath + "\\aseets\\" + relPath.data;
			mat.emission = LoadTextureFromFile(path.c_str());
		}
	}
	else {
		string path = currentPath + "\\aseets\\BlackPlaceholder.png";
		mat.emission = LoadTextureFromFile(path.c_str());
	}
	return mat;
}

Model Loader::LoadModelFromFile(const char* filename)
{
	Model model;
	Assimp::Importer importer;
	string path = currentPath + filename;
	const aiScene* scene = importer.ReadFile(path.c_str(),
		aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

	for (int i = 0; i < scene->mNumMeshes; i++) {
		vector<vertex> vert;
		vector<uint32_t> ind;

		const aiMesh* pMesh = scene->mMeshes[i];

		for (int j = 0; j < pMesh->mNumVertices; j++) {
			const aiVector3D pos = pMesh->mVertices[j];
			const aiVector3D uv = pMesh->mTextureCoords[0][j];
			const aiVector3D normal = pMesh->mNormals[j];
			vert.emplace_back(vector3{ pos.x, pos.y, pos.z }, vector2{ uv.x, uv.y }, vector3{normal.x, normal.y, normal.z});
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
