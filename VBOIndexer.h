#ifndef VBOINDEXER_H_
#define VBOINDEXER_H_

#include "glm.hpp"
#include "Stack.h"


	struct PackedVertex
	{
		glm::vec3 position;
		glm::vec2 uv;
		glm::vec3 normal;
		glm::vec3 color;
		glm::vec3 tangent;
		glm::vec3 binormal;
		bool operator<(const PackedVertex that) const
		{
			return memcmp((void*)this, (void*)&that, sizeof(PackedVertex))>0;
		}
	};

	void indexVBO(
		Stack<glm::vec3> & in_vertices,
		Stack<glm::vec2> & in_uvs,
		Stack<glm::vec3> & in_normals,
		Stack<glm::vec3> & in_colors,
		Stack<glm::vec3> & in_tangents,

		Stack<unsigned short> & out_indices,
		Stack<glm::vec3> & out_vertices,
		Stack<glm::vec2> & out_uvs,
		Stack<glm::vec3> & out_normals,
		Stack<glm::vec3> & out_colors,
		Stack<glm::vec3> & out_tangents
		);





#endif

