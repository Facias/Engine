#include "VBOIndexer.h"
#include <vector>
#include <map>

#include <string.h> // for memcmp
#include "Stack.h"


// Returns true iif v1 can be considered equal to v2
bool is_near(float v1, float v2) 
{
	return fabs(v1 - v2) < 0.01f;
}

// Searches through all already-exported vertices
// for a similar one.
// Similar = same position + same UVs + same normal
bool getSimilarVertexIndex(
	glm::vec3 & in_vertex,
	glm::vec2 & in_uv,
	glm::vec3 & in_normal,
	glm::vec3 & in_colors,
	Stack<glm::vec3> & out_vertices,
	Stack<glm::vec2> & out_uvs,
	Stack<glm::vec3> & out_normals,
	Stack<glm::vec3> & out_colors,

	unsigned short & result
	) 
{
	// Lame linear search
	for (unsigned int i = 0; i<out_vertices.size; i++) 
	{
		if (
			is_near(in_vertex.x, out_vertices.item[i].x) &&
			is_near(in_vertex.y, out_vertices.item[i].y) &&
			is_near(in_vertex.z, out_vertices.item[i].z) &&

			is_near(in_uv.x, out_uvs.item[i].x) &&
			is_near(in_uv.y, out_uvs.item[i].y) &&

			is_near(in_normal.x, out_normals.item[i].x) &&
			is_near(in_normal.y, out_normals.item[i].y) &&
			is_near(in_normal.z, out_normals.item[i].z) &&

			is_near(in_colors.x, out_colors.item[i].x) &&
			is_near(in_colors.y, out_colors.item[i].y) &&
			is_near(in_colors.z, out_colors.item[i].z)
			) 
		{
			result = i;
			return true;
		}
	}
	// No other vertex could be used instead.
	// Looks like we'll have to add it to the VBO.
	return false;
}

void indexVBO_slow(
	Stack<glm::vec3> & in_vertices,
	Stack<glm::vec2> & in_uvs,
	Stack<glm::vec3> & in_normals,
	Stack<glm::vec3> & in_colors,

	Stack<unsigned short> & out_indices,
	Stack<glm::vec3> & out_vertices,
	Stack<glm::vec2> & out_uvs,
	Stack<glm::vec3> & out_normals,
	Stack<glm::vec3> & out_colors
	) 
{
	// For each input vertex
	for (unsigned int i = 0; i<in_vertices.size; i++)
	{

		// Try to find a similar vertex in out_XXXX
		unsigned short index;
		bool found = getSimilarVertexIndex(in_vertices.item[i], in_uvs.item[i], in_normals.item[i], in_colors.item[i], out_vertices, out_uvs, out_normals, out_colors, index);

		if (found) 
		{ // A similar vertex is already in the VBO, use it instead !
			out_indices.add(index);
		}
		else { // If not, it needs to be added in the output data.
			out_vertices.add(in_vertices.item[i]);
			out_uvs.add(in_uvs.item[i]);
			out_normals.add(in_normals.item[i]);
			out_indices.add((unsigned short)out_vertices.size - 1);
		}
	}
}



bool getSimilarVertexIndex_fast(
	PackedVertex & packed,
	std::map<PackedVertex, unsigned short> & VertexToOutIndex,
	unsigned short & result
	) 
{
	std::map<PackedVertex, unsigned short>::iterator it = VertexToOutIndex.find(packed);
	if (it == VertexToOutIndex.end()) 
	{
		return false;
	}
	else 
	{
		result = it->second;
		return true;
	}
}

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
	) 
{
	std::map<PackedVertex, unsigned short> VertexToOutIndex;

	PackedVertex packed;
	// For each input vertex
	for (unsigned int i = 0; i<in_vertices.size; i++) 
	{
		

		packed.position = in_vertices.item[i];
		packed.uv = in_uvs.item[i];
		packed.normal = in_normals.item[i];
		if (in_colors.size > 0) packed.color = in_colors.item[i];
		if (in_tangents.size > 0) packed.tangent = in_tangents.item[i];

		// Try to find a similar vertex in out_XXXX
		unsigned short index;
		bool found = getSimilarVertexIndex_fast(packed, VertexToOutIndex, index);

		if (found)
		{ // A similar vertex is already in the VBO, use it instead !
			out_indices.add(index);

		}
		else 
		{ // If not, it needs to be added in the output data.
			out_vertices.add(in_vertices.item[i]);
			out_uvs.add(in_uvs.item[i]);
			out_normals.add(in_normals.item[i]);
			if (in_colors.size > 0) out_colors.add(in_colors.item[i]);
			if (in_tangents.size > 0) out_colors.add(in_tangents.item[i]);

			unsigned short newindex = (unsigned short)out_vertices.size - 1;
			out_indices.add(newindex);
			VertexToOutIndex[packed] = newindex;
		}
	}
}


