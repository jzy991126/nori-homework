#include <iostream>

#include "nori/membree.h"

void errorFunction(void* userPtr, enum RTCError error, const char* str)
{
	printf("error %d: %s\n", error, str);
}

void M_embree::initializeDevice()
{
	device = rtcNewDevice(nullptr);

	if (!device)
		printf("error %d: cannot create device\n", rtcGetDeviceError(nullptr));

	rtcSetDeviceErrorFunction(device, errorFunction, nullptr);
}

unsigned int M_embree::addMesh(std::vector<Eigen::Vector3f>& _vertices, std::vector<uint32_t> _indices)
{
	RTCGeometry geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);

	auto* vertices = (float*)rtcSetNewGeometryBuffer(geom,
		RTC_BUFFER_TYPE_VERTEX,
		0,
		RTC_FORMAT_FLOAT3,
		3 * sizeof(float),
		_vertices.size());

	auto* indices = (unsigned*)rtcSetNewGeometryBuffer(geom,
		RTC_BUFFER_TYPE_INDEX,
		0,
		RTC_FORMAT_UINT3,
		3 * sizeof(unsigned),
		_indices.size());

	if (vertices && indices)
	{
		//vertices[0] = 0.f; vertices[1] = 0.f; vertices[2] = 0.f;
		//vertices[3] = 1.f; vertices[4] = 0.f; vertices[5] = 0.f;
		//vertices[6] = 0.f; vertices[7] = 1.f; vertices[8] = 0.f;

		//indices[0] = 0; indices[1] = 1; indices[2] = 2;

		for (int i = 0; i < _vertices.size(); i++)
		{
			vertices[i * 3] = _vertices[i].x();
			vertices[i * 3 + 1] = _vertices[i].y();
			vertices[i * 3 + 2] = _vertices[i].z();
		}

		for (int i = 0; i < _indices.size(); i++)
		{
			indices[i] = _indices[i];
		}
	}

	rtcCommitGeometry(geom);

	/*
	 * In rtcAttachGeometry(...), the scene takes ownership of the geom
	 * by increasing its reference count. This means that we don't have
	 * to hold on to the geom handle, and may release it. The geom object
	 * will be released automatically when the scene is destroyed.
	 *
	 * rtcAttachGeometry() returns a geometry ID. We could use this to
	 * identify intersected objects later on.
	 */
	unsigned int id = rtcAttachGeometry(scene, geom);
	rtcReleaseGeometry(geom);
	return id;


}

void M_embree::endAdd()
{
	rtcCommitScene(scene);
}

RTCRayHit M_embree::castRay(Eigen::Vector3f origin, Eigen::Vector3f dir, float tnear, float tfar)const
{
	struct RTCIntersectContext context {};
	rtcInitIntersectContext(&context);

	/*
	 * The ray hit structure holds both the ray and the hit.
	 * The user must initialize it properly -- see API documentation
	 * for rtcIntersect1() for details.
	 */
	struct RTCRayHit rayhit {};
	rayhit.ray.org_x = origin.x();
	rayhit.ray.org_y = origin.y();
	rayhit.ray.org_z = origin.z();
	rayhit.ray.dir_x = dir.x();
	rayhit.ray.dir_y = dir.y();
	rayhit.ray.dir_z = dir.z();
	rayhit.ray.tnear = tnear;
	rayhit.ray.tfar = tfar;
	rayhit.ray.mask = -1;
	rayhit.ray.flags = 0;
	rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
	rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

	/*
	 * There are multiple variants of rtcIntersect. This one
	 * intersects a single ray with the scene.
	 */
	rtcIntersect1(scene, &context, &rayhit);

	return rayhit;

	//std::cout << origin << std::endl;
	//if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID)
	//{
	//	/* Note how geomID and primID identify the geometry we just hit.
	//	 * We could use them here to interpolate geometry information,
	//	 * compute shading, etc.
	//	 * Since there is only a single triangle in this scene, we will
	//	 * get geomID=0 / primID=0 for all hits.
	//	 * There is also instID, used for instancing. See
	//	 * the instancing tutorials for more information */
	//	//printf("Found intersection on geometry %d, primitive %d at tfar=%f\n",
	//	//	rayhit.hit.geomID,
	//	//	rayhit.hit.primID,
	//	//	rayhit.ray.tfar);
	//	//printf("%f %f %f %f %f", rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z, rayhit.hit.u, rayhit.hit.v);
	//}
	//else
	//	std::cout << "did not find" << std::endl;
}
M_embree::M_embree()
{
	initializeDevice();
	scene = rtcNewScene(device);
}