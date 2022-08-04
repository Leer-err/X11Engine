#include "Loader.h"

#include <memory>

using std::unique_ptr;

ComPtr<ID3D11ShaderResourceView> Loader::LoadTextureFromFile(const char* filename)
{
	ComPtr<IWICBitmapDecoder> decoder;
	ComPtr<IWICBitmapFrameDecode> frameDecoder;
	WICPixelFormatGUID format;
	UINT width, height;

	const auto& tex = m_textureRegistry.find(filename);
	if (tex != m_textureRegistry.end()) {
		return tex->second;
	}

	int size = strlen(filename) + 1;
	wchar_t* wpath = new wchar_t[size];
	mbstowcs(wpath, filename, size);

	m_factory->CreateDecoderFromFilename(wpath, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);

	decoder->GetFrame(0, &frameDecoder);

	frameDecoder->GetSize(&width, &height);

	unique_ptr<char[]> buffer(new char [height * width * 4]);
	frameDecoder->GetPixelFormat(&format);
	if (format == GUID_WICPixelFormat32bppBGRA) {
		frameDecoder->CopyPixels(nullptr, width * 4, width * height * 4, reinterpret_cast<BYTE*>(buffer.get()));
		ID3D11Texture2D* tex = Graphics::get()->CreateTexture2D(DXGI_FORMAT_B8G8R8A8_UNORM, false, false, width, height, buffer.get());
		ID3D11ShaderResourceView* res = Graphics::get()->CreateTexture2DSRV(tex, DXGI_FORMAT_B8G8R8A8_UNORM);
		m_textureRegistry.emplace(filename, res);
		return res;
	}
	return {};
}

Material Loader::LoadMaterial(const aiMaterial* material)
{
	Material mat;
	if (material->GetTextureCount(aiTextureType_BASE_COLOR) > 0) {
		aiString relPath;
		if (material->GetTexture(aiTextureType_BASE_COLOR, 0, &relPath) == AI_SUCCESS) {
			string path = m_currentPath + "\\aseets\\" + relPath.data;
			mat.baseColor = LoadTextureFromFile(path.c_str());
		}
	}
	else {
		string path = m_currentPath + "\\aseets\\TexturePlaceholder.png";
		mat.baseColor = LoadTextureFromFile(path.c_str());
	}

	if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
		aiString relPath;
		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &relPath) == AI_SUCCESS) {
			string path = m_currentPath + "\\aseets\\" + relPath.data;
			mat.diffuse = LoadTextureFromFile(path.c_str());
		}
	}
	else {
		string path = m_currentPath + "\\aseets\\WhitePlaceholder.png";
		mat.diffuse = LoadTextureFromFile(path.c_str());
	}

	if (material->GetTextureCount(aiTextureType_SPECULAR) > 0) {
		aiString relPath;
		if (material->GetTexture(aiTextureType_SPECULAR, 0, &relPath) == AI_SUCCESS) {
			string path = m_currentPath + "\\aseets\\" + relPath.data;
			mat.specular = LoadTextureFromFile(path.c_str());
		}
	}
	else {
		string path = m_currentPath + "\\aseets\\WhitePlaceholder.png";
		mat.specular = LoadTextureFromFile(path.c_str());
	}

	if (material->GetTextureCount(aiTextureType_EMISSIVE) > 0) {
		aiString relPath;
		if (material->GetTexture(aiTextureType_EMISSIVE, 0, &relPath) == AI_SUCCESS) {
			string path = m_currentPath + "\\aseets\\" + relPath.data;
			mat.emission = LoadTextureFromFile(path.c_str());
		}
	}
	else {
		string path = m_currentPath + "\\aseets\\BlackPlaceholder.png";
		mat.emission = LoadTextureFromFile(path.c_str());
	}

	ComPtr<ID3DBlob> vertexShader = CompileShaderFromFile(L"VertexShader.hlsl", "vs_5_0", shaderFlags);

	mat.inputLayout = Graphics::get()->CreateInputLayoutFromShader(vertexShader.Get());
	mat.pixelShader = Graphics::get()->CreatePixelShader(CompileShaderFromFile(L"PixelShader.hlsl", "ps_5_0", shaderFlags));
	mat.vertexShader = Graphics::get()->CreateVertexShader(vertexShader.Get());
	return mat;
}

ID3DBlob* Loader::CompileShaderFromFile(const wchar_t* filename, const char* target, UINT flags)
{
	ID3DBlob* res;
	D3DCompileFromFile(filename, nullptr, nullptr, "main", target, flags, NULL, &res, nullptr);
	return res;
}

Model* Loader::LoadModelFromFile(const char* filename)
{
	Model model;
	Assimp::Importer importer;
	string path = m_currentPath + filename;

	const auto& mod = m_modelRegistry.find(filename);
	if (mod != m_modelRegistry.end()) {
		return &(mod->second);
	}

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

	return &(m_modelRegistry.emplace(filename, model).first->second);
}
