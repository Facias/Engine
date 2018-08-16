#include "Octree.h"
#include <string>

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
	next->item.add(dataitem);
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

	for (int n = 0; n < maxdepth; n++)
	{
		for (int i = 0; i < next->item.size; i++)
		{
			if (glm::length(pos - next->item.item[i].pos) < next->item.item[i].radius)
			{
				if (glm::dot(next->item.item[i].norm, norm) > 1.0-angleMargin)
				{
					//if (glm::dot(glm::normalize(pos - next->item.item[i].pos), next->item.item[i].norm) > -.1)
					{
						samples[sampleCount] = next->item.item[i];
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
			weight[i] = 1.0 /glm::max(.01f,  p0 + p1  );
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

	for (int n = 0; n < maxdepth; n++)
	{
		;
		for (int i = 0; i < next->item.size; i++)
		{
			if (glm::length(pos - next->item.item[i].pos) < next->item.item[i].radius)
			{
				if (glm::dot(next->item.item[i].norm, norm) > 1.0 - angleMargin)
				{
					//if (glm::dot(glm::normalize(pos - next->item.item[i].pos), next->item.item[i].norm) > -.1)
					{
						return true;
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

		next = next->child[location];
	}
	
	return false;
}