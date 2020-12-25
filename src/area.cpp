
#include<nori/emitter.h>
#include<nori/common.h>
#include<nori/mesh.h>

NORI_NAMESPACE_BEGIN


class AreaLight : public Emitter
{

public:
	Color3f radiance;
public:

	AreaLight(const PropertyList& props)
	{
		radiance = props.getColor("radiance");
	}

	Color3f sample(EmitterQueryRecord& lRec, const Point2f& sample)const override
	{
		
		SampleData sRec =  m_mesh->sampleSurface(sample);

		//ShapeQueryRecord sRec(lRec.ref);
		//m_shape->sampleSurface(sRec, sample);
		lRec.p = sRec.point;
		lRec.pdf = sRec.pdf;
		lRec.n = sRec.normal;
		lRec.wi = (lRec.p - lRec.ref).normalized();

	

		Ray3f shadowRay(lRec.ref, lRec.wi, Epsilon,
			(lRec.p - lRec.ref).norm() - Epsilon);
		lRec.shadowRay = shadowRay;
		if (pdf(lRec) > 0)
			return eval(lRec) / pdf(lRec);
		else
			return Color3f(0.f);
		return { 0,0,0 };

	}

	Color3f eval(const EmitterQueryRecord& lRec) const override
	{
		float cos_theta_i = lRec.n.dot(-lRec.wi);

		if ((cos_theta_i) >= 0) {
			return radiance;
		}
		else {
			return Color3f(0.f);
		}
	}

	float pdf(const EmitterQueryRecord& lRec) const override
	{
		float _pdf = lRec.pdf;

		float cosin = std::abs(lRec.n.dot(-lRec.wi));

		return _pdf * (lRec.p - lRec.ref).squaredNorm() / cosin;
	}

	std::string toString() const override{
		return "AreaLight";
	}
};


NORI_REGISTER_CLASS(AreaLight, "area");


NORI_NAMESPACE_END