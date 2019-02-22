#include <string>
#include "Octree.h"


//using namespace ENGINE;

// irradiance sampling
Octree<IrradianceSample>::Octree()
{
	root = new OctNode<IrradianceSample>();


	minbound = glm::vec3(-10);
	maxbound = glm::vec3(10);
	maxdepth = 4;

}

Octree<IrradianceSample>::Octree(glm::vec3 minb, glm::vec3 maxb, int md)
{
	root = new OctNode<IrradianceSample>;
	minbound = minb;
	maxbound = maxb;
	maxdepth = md;
	expand(root, md);
}

void Octree<IrradianceSample>::expand(OctNode<IrradianceSample> *node, int levels)
{
	if (levels == 0) return;
	for (int n = 0; n < 8; n++)
	{
		//printf("expand %d %d\n", n, levels);
		node->child[n] = new OctNode<IrradianceSample>();
		expand(node->child[n], levels - 1);
	}
}

void Octree<IrradianceSample>::add(IrradianceSample &dataitem)
{

	OctNode<IrradianceSample> *next = root;
	float xrange = maxbound.x - minbound.x;
	float yrange = maxbound.y - minbound.y;
	float zrange = maxbound.z - minbound.z;

	float xmin = minbound.x;
	float ymin = minbound.y;
	float zmin = minbound.z;

	char x, y, z;

	for (int n = 0; n < maxdepth; n++)
	{
		xrange /= 2.0;
		yrange /= 2.0;
		zrange /= 2.0;

		if (dataitem.pos.x < xmin + xrange)
			x = 0;
		else
			x = 1;

		if (dataitem.pos.y < ymin + yrange)
			y = 0;
		else
			y = 1;

		if (dataitem.pos.z < zmin + zrange)
			z = 0;
		else
			z = 1;

		int location = x + 2 * y + 4 * z;

		xmin += x * xrange;
		ymin += y * yrange;
		zmin += z * zrange;

		// if sample range expands the division,
		// add sample to this layer of tree
		if (x == 1)
			if (dataitem.pos.x - dataitem.radius < xmin + xrange)
				break;
		if (y == 1)
			if (dataitem.pos.y - dataitem.radius < ymin + yrange)
				break;
		if (z == 1)
			if (dataitem.pos.z - dataitem.radius < zmin + zrange)
				break;

		next = next->child[location];

	}
	next->stack.add(dataitem);
}


IrradianceSample *Octree<IrradianceSample>::search(glm::vec3 pos, glm::vec3 norm)
{
	OctNode<IrradianceSample> *next = root;

	float xrange = maxbound.x - minbound.x;
	float yrange = maxbound.y - minbound.y;
	float zrange = maxbound.z - minbound.z;

	float xmin = minbound.x;
	float ymin = minbound.y;
	float zmin = minbound.z;

	IrradianceSample samples[40];

	char sampleCount = 0;

	char x, y, z;

	for (uint n = 0; n < maxdepth; n++)
	{
		for (uint i = 0; i < next->stack.size; i++)
		{
			if (glm::length(pos - next->stack.item[i].pos) < next->stack.item[i].radius)
			{
				if (glm::dot(next->stack.item[i].norm, norm) > 1.0-angleMargin)
				{
					//if (glm::dot(glm::normalize(pos - next->item.item[i].pos), next->item.item[i].norm) > -.1)
					{
						samples[sampleCount] = next->stack.item[i];
						sampleCount++;
						if (sampleCount == 40) break;
					}
				}
			}
		}
		if (sampleCount == 40) break;
		xrange /= 2.0;
		yrange /= 2.0;
		zrange /= 2.0;

		

		if (pos.x < xmin + xrange)
			x = 0;
		else
			x = 1;

		if (pos.y < ymin + yrange)
			y = 0;
		else
			y = 1;

		if (pos.z < zmin + zrange)
			z = 0;
		else
			z = 1;

		int location = x + 2 * y + 4 * z;

		xmin += x * xrange;
		ymin += y * yrange;
		zmin += z * zrange;

		next = next->child[location];
	}

	if (sampleCount > 0)
	{
		glm::vec3 light = samples[0].light;
		float weight[40];
		float p0, p1;
		
		for (int i = 0; i < sampleCount; i++)
		{
			p0 = (glm::length(pos - samples[i].pos) / samples[i].radius);
			p1 = sqrt(1.01f - glm::dot(norm, samples[i].norm));
			weight[i] = 1.0f /glm::max(.01f,  p0 + p1  );
			weight[i] = glm::min(.5f, weight[i]);
		}
		
		/*
		for (int i = 0; i < sampleCount; i++)
		{
			p0 = glm::length(pos - samples[i].pos) / (.5*samples[i].radius);
			p1 = sqrt(1.01f - glm::dot(norm, samples[i].norm))/(1-cos(.174));
			weight[i] = 1 - glm::max(p0, p1);
		}
		*/
		glm::vec3 top= glm::vec3(0);
		for (int i = 0; i < sampleCount; i++)
		{

			top.r += weight[i] * (samples[i].light.r +glm::dot(glm::cross(samples[i].norm, norm), samples[i].rotGradient[0]) + glm::dot(pos - samples[i].pos, samples[i].transGradient[0]) );
			top.g += weight[i] * (samples[i].light.g +glm::dot(glm::cross(samples[i].norm, norm), samples[i].rotGradient[1]) + glm::dot(pos - samples[i].pos, samples[i].transGradient[1]));
			top.b += weight[i] * (samples[i].light.b +glm::dot(glm::cross(samples[i].norm, norm), samples[i].rotGradient[2]) + glm::dot(pos - samples[i].pos, samples[i].transGradient[2]));
		
			//top.r += weight[i] * (glm::dot(glm::cross(samples[i].norm, norm), samples[i].rotGradient[0]));
			//top.g += weight[i] * (glm::dot(glm::cross(samples[i].norm, norm), samples[i].rotGradient[1]));
			//top.b += weight[i] * (glm::dot(glm::cross(samples[i].norm, norm), samples[i].rotGradient[2]));

		}
		float bottom = 0;
		for (int i = 0; i < sampleCount; i++)
			bottom += weight[i];
		
		light = top / bottom;
		
		glm::vec3 rotgrad[3] = { glm::vec3(0) };
		glm::vec3 transgrad[3] = { glm::vec3(0) };
		return &IrradianceSample(light, 0.f, glm::vec3(0), glm::vec3(0), rotgrad);
	}
	else
		return NULL;
}

bool Octree<IrradianceSample>::scanRecords(glm::vec3 pos, glm::vec3 norm)
{
	OctNode<IrradianceSample> *next = root;

	float xrange = maxbound.x - minbound.x;
	float yrange = maxbound.y - minbound.y;
	float zrange = maxbound.z - minbound.z;

	float xmin = minbound.x;
	float ymin = minbound.y;
	float zmin = minbound.z;

	char x, y, z;

	for (uint n = 0; n < maxdepth; n++)
	{
		;
		for (uint i = 0; i < next->stack.size; i++)
		{
			if (glm::length(pos - next->stack.item[i].pos) < next->stack.item[i].radius)
			{
				if (glm::dot(next->stack.item[i].norm, norm) > 1.0f - angleMargin)
				{
					//if (glm::dot(glm::normalize(pos - next->item.item[i].pos), next->item.item[i].norm) > -.1)
					{
						return true;
					}
				}
			}
		}

		xrange /= 2.0f;
		yrange /= 2.0f;
		zrange /= 2.0f;

		if (pos.x < xmin + xrange)
			x = 0;
		else
			x = 1;

		if (pos.y < ymin + yrange)
			y = 0;
		else
			y = 1;

		if (pos.z < zmin + zrange)
			z = 0;
		else
			z = 1;

		int location = x + 2 * y + 4 * z;

		xmin += x * xrange;
		ymin += y * yrange;
		zmin += z * zrange;

		next = next->child[location];
	}
	
	return false;
}


// vertex data
Octree<VertexData>::Octree()
{
	root = new OctNode<VertexData>();

	minbound = glm::vec3(-10);
	maxbound = glm::vec3(10);
	maxdepth = 4;
	//expand(root, 4);
}

Octree<VertexData>::Octree(int md)
{
	root = new OctNode<VertexData>;
	minbound = glm::vec3(-10);
	maxbound = glm::vec3(10);
	maxdepth = md;
	expand(root, md);
}

Octree<VertexData>::Octree(glm::vec3 minb, glm::vec3 maxb, int md)
{
	root = new OctNode<VertexData>;
	minbound = minb;
	maxbound = maxb;
	maxdepth = md;
	expand(root, md);
}


void Octree<VertexData>::expand(OctNode<VertexData> *node, int levels)
{
	if (levels == 0)
	{
		return;
	}
	for (int n = 0; n < 8; n++)
	{
		node->child[n] = new OctNode<VertexData>();
		if (levels == 1) 
		{
			//printf("create stack\n");
			node->child[n]->stack = Stack<VertexData>(8);
		}


		expand(node->child[n], levels - 1);
	}
}


void Octree<VertexData>::clean(OctNode<VertexData> *node)
{
	if (node->child[0] == NULL)
	{
		node->stack.size = 0;
	}
	else
	{
		for (int n = 0; n<7; n++)
			clean(node->child[n]);
	}

}

void Octree<VertexData>::add(VertexData &dataitem)
{

	OctNode<VertexData> *next = root;
	float xrange = maxbound.x - minbound.x;
	float yrange = maxbound.y - minbound.y;
	float zrange = maxbound.z - minbound.z;

	float xmin = minbound.x;
	float ymin = minbound.y;
	float zmin = minbound.z;

	char x, y, z;

	for (int n = 0; n < maxdepth; n++)
	{
		xrange /= 2.0;
		yrange /= 2.0;
		zrange /= 2.0;

		if (dataitem.pos.x < xmin + xrange)
			x = 0;
		else
			x = 1;

		if (dataitem.pos.y < ymin + yrange)
			y = 0;
		else
			y = 1;

		if (dataitem.pos.z < zmin + zrange)
			z = 0;
		else
			z = 1;

		int location = x + 2 * y + 4 * z;

		xmin += x * xrange;
		ymin += y * yrange;
		zmin += z * zrange;

		if (next->child[location] != NULL) next = next->child[location];

	}

	next->stack.add(dataitem);
}

// return index of existing item, otherwise return -1
int Octree<VertexData>::search(VertexData &dataitem)
{
	OctNode<VertexData> *next = root;

	float xrange = maxbound.x - minbound.x;
	float yrange = maxbound.y - minbound.y;
	float zrange = maxbound.z - minbound.z;

	float xmin = minbound.x;
	float ymin = minbound.y;
	float zmin = minbound.z;

	char sampleCount = 0;

	glm::vec3 pos = dataitem.pos;
	glm::vec3 norm = dataitem.norm;

	char x, y, z;

	for (int n = 0; n < maxdepth; n++)
	{
		if (n == maxdepth)
		{
			for (int i = 0; i < next->stack.size; i++)
			{
				if (glm::length(pos - next->stack.item[i].pos) < .001)
				{
					if (glm::length(norm - next->stack.item[i].norm) < .001)
					{
						return next->stack.item[i].index;
					}
				}

			}
		}
		xrange /= 2.0;
		yrange /= 2.0;
		zrange /= 2.0;

		if (pos.x < xmin + xrange)
			x = 0;
		else
			x = 1;

		if (pos.y < ymin + yrange)
			y = 0;
		else
			y = 1;

		if (pos.z < zmin + zrange)
			z = 0;
		else
			z = 1;

		int location = x + 2 * y + 4 * z;

		xmin += x * xrange;
		ymin += y * yrange;
		zmin += z * zrange;

		
		if (next->child[location] != NULL) next = next->child[location];
	}

	return -1;
}