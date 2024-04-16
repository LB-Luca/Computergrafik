#include "model.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>

#include <assimp/cimport.h>        // Plain-C interface
#include <assimp/scene.h>          // Output data structure
#include <assimp/postprocess.h>    // Post processing flags


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/ext.hpp>

Model create_model_from_file(char const* file)
{
	Model model = {};

	char exe_path[256];
	get_exe_path(exe_path, 256 * sizeof(char));
	char abs_path[256];
	memcpy(abs_path, exe_path, 256);
	strcat(abs_path, file);

	tinyobj::ObjReaderConfig reader_config;
	reader_config.mtl_search_path = "./";

	tinyobj::ObjReader reader;

	if (!reader.ParseFromFile(abs_path, reader_config)) {
		if (!reader.Error().empty()) {
			printf("[TinyObjReader]: %s\n", reader.Error().c_str());
			getchar();
		}
		exit(-1);
	}

	if (!reader.Warning().empty()) {
		printf("[TinyObjReader]: %s\n", reader.Warning().c_str());
	}

	auto& attrib = reader.GetAttrib();
	auto& shapes = reader.GetShapes();
	auto& materials = reader.GetMaterials();

	for (size_t s = 0; s < shapes.size(); s++) {
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			model.vertex_count += shapes[s].mesh.num_face_vertices[f];
		}
	}
	model.vertices = (Vertex*)malloc(model.vertex_count * sizeof(Vertex));
	memset(model.vertices, 0, model.vertex_count * sizeof(Vertex));

	// Loop over shapes
	size_t vert_count = 0;
	tinyobj::real_t minX =  99999;
	tinyobj::real_t maxX = -99999;
	tinyobj::real_t minY =  99999;
	tinyobj::real_t maxY = -99999;
	tinyobj::real_t minZ =  99999;
	tinyobj::real_t maxZ = -99999;

	for (size_t s = 0; s < shapes.size(); s++) {
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++) {
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
				tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
				tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
				model.vertices[vert_count + 0].pos.x = vx;
				model.vertices[vert_count + 0].pos.y = vy;
				model.vertices[vert_count + 0].pos.z = vz;

				// Check for min/max x,y,z for AABB
				if (vx < minX)			minX = vx;
				else if (vx >= maxX)	maxX = vx;
				if (vy < minY)			minY = vy;
				else if (vy >= maxY)	maxY = vy;
				if (vz < minZ)			minZ = vz;
				else if (vz >= maxZ)	maxZ = vz;

				// Check if `normal_index` is zero or positive. negative = no normal data
				if (idx.normal_index >= 0) {
					tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
					tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
					tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
					model.vertices[vert_count + 0].normal.x = nx;
					model.vertices[vert_count + 0].normal.y = ny;
					model.vertices[vert_count + 0].normal.z = nz;
				}

				// Check if `texcoord_index` is zero or positive. negative = no texcoord data
				if (idx.texcoord_index >= 0) {
					tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
					tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
					model.vertices[vert_count + 0].uv.s = tx;
					model.vertices[vert_count + 0].uv.t = ty;
				}

				// Optional: vertex colors
				// tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
				// tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
				// tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];

				vert_count++;
			}
			index_offset += fv;

			// per-face material
			//shapes[s].mesh.material_ids[f];
		}
	}

	model.bounding_box.min_xyz = glm::vec4(minX, minY, minZ, 1.0f);
	model.bounding_box.max_xyz = glm::vec4(maxX, maxY, maxZ, 1.0f);

	return model;
}

Model create_model_from_file_indexed(char const* file)
{
	char exe_path[256];
	get_exe_path(exe_path, 256 * sizeof(char));
	char abs_path[256];
	memcpy(abs_path, exe_path, 256);
	strcat(abs_path, file);

	Model model = {};
	/* Find out how many vertices and indices this model has in total.
	Assimp makes sure to use multiple vertices per face by the flag 'aiProcess_JoinIdenticalVertices'.
	So we allocate exactly as much memory as is required for the vertex-buffer.
	*/
	aiScene const* scene = aiImportFile(abs_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenBoundingBoxes);
	for (int i = 0; i < scene->mNumMeshes; ++i) {
		model.vertex_count += scene->mMeshes[i]->mNumVertices;
		for (int f = 0; f < scene->mMeshes[i]->mNumFaces; ++f) {
			model.index_count += scene->mMeshes[i]->mFaces[f].mNumIndices;
			model.face_count++;
		}
	}
	model.vertices = (Vertex*)malloc(model.vertex_count * sizeof(Vertex));
	model.indices = (uint32_t*)malloc(model.index_count * sizeof(uint32_t));

	/* Go through all faces of meshes and copy. */
	uint32_t indices_copied = 0;
	uint32_t vertices_copied = 0;
	for (int m = 0; m < scene->mNumMeshes; ++m) {
		for (int f = 0; f < scene->mMeshes[m]->mNumFaces; ++f) {
			for (int i = 0; i < scene->mMeshes[m]->mFaces[f].mNumIndices; ++i) {
				/* Get current index/vertex/normal from model file */
				uint32_t* index = (uint32_t*)&(scene->mMeshes[m]->mFaces[f].mIndices[i]);
				aiVector3D vertex = scene->mMeshes[m]->mVertices[*index];

				/* copy into Model data */
				model.indices[indices_copied] = *index;
				Vertex* current_vertex = &model.vertices[*index];
				current_vertex->pos.x = vertex.x;
				current_vertex->pos.y = vertex.y;
				current_vertex->pos.z = vertex.z;

				if (scene->mMeshes[m]->HasTextureCoords(0)) {
					aiVector3D uv = scene->mMeshes[m]->mTextureCoords[0][*index];
					current_vertex->uv.x = uv.x;
					current_vertex->uv.y = uv.y;
				}

				if (scene->mMeshes[m]->HasNormals()) {
					aiVector3D normal = scene->mMeshes[m]->mNormals[*index];
					current_vertex->normal.x = normal.x;
					current_vertex->normal.y = normal.y;
					current_vertex->normal.z = normal.z;
				}

				/* add debug color */
				{
					current_vertex->color = glm::vec4(1.f, 0.84f, 0.f, 0.f);
				}

				indices_copied++;
			}
		}

		aiAABB aabb = scene->mMeshes[m]->mAABB;
		model.bounding_box.min_xyz = glm::vec4(aabb.mMin.x, aabb.mMin.y, aabb.mMin.z, 1.0f);
		model.bounding_box.max_xyz = glm::vec4(aabb.mMax.x, aabb.mMax.y, aabb.mMax.z, 1.0f);
	}

	return model;
}

void assign_texture_to_model(Model * model, VkalTexture texture, uint32_t id, TextureType texture_type)
{
	assert( (texture_type < MAX_TEXTURE_TYPES) && "Unknown TextureType!" );
	if (texture_type == TEXTURE_TYPE_DIFFUSE) {
		model->material.texture_id = id;
		model->texture = texture;
		model->material.is_textured = 1;
	}
	else if (texture_type == TEXTURE_TYPE_NORMAL) {
		model->material.normal_id = id;
		model->normal_map = texture;
		model->material.has_normal_map = 1;
	}
}