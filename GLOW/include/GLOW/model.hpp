#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

uint32_t loadModel(const char *path, float **positions, float **normals, uint32_t **indices) {

    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);	
	
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        return 0;
    }

	aiNode *node = scene->mRootNode->mChildren[0];
	aiMesh *mesh = scene->mMeshes[node->mMeshes[0]]; 
  
	uint32_t numVertices = mesh->mNumVertices;

	*positions = (float*)malloc(numVertices * 3 * sizeof(float));
	*normals   = (float*)malloc(numVertices * 3 * sizeof(float));

	for(uint32_t i = 0; i < mesh->mNumVertices; i++) {
		(*positions)[i*3 + 0] = mesh->mVertices[i].x;
		(*positions)[i*3 + 1] = mesh->mVertices[i].y;
		(*positions)[i*3 + 2] = mesh->mVertices[i].z;

        (*normals)[i*3 + 0] = mesh->mNormals[i].x;
		(*normals)[i*3 + 1] = mesh->mNormals[i].y;
		(*normals)[i*3 + 2] = mesh->mNormals[i].z;

	}

    return mesh->mNumVertices;
}
