
#include <nori/integrator.h>
#include <nori/scene.h>
#include <nori/bsdf.h>
#include <nori/emitter.h>
#include <nori/sampler.h>
#include <nori/warp.h>

NORI_NAMESPACE_BEGIN

class EMSPATHMATSIntegrator : public Integrator {
public:
	EMSPATHMATSIntegrator(const PropertyList& props) {
		m_q = 0.05; //TODO: Chech this
	}

	Color3f Li(const Scene* scene, Sampler* sampler, const Ray3f& ray) const {
		Intersection its;

		// If not visible return black
		if (!scene->rayIntersect(ray, its))
			return Color3f(0.0f);


		//Get Le

		Color3f Le = Color3f(0.0f);
		const Emitter* emi2 = its.mesh->getEmitter();
		if (emi2) {
			//Its an Emitter.
			EmitterQueryRecord iRec2(ray.o, its.p, its.shFrame.n);
			Le = emi2->eval(iRec2);
		}
		
		const BSDF* bsdf = its.mesh->getBSDF();
		// No Ld

		const std::vector<Mesh*> emmits = scene->getEmmitMeshes();

		int rand_index = rand() % emmits.size();

		Emitter* emmit = emmits.at(rand_index)->getEmitter();

		EmitterQueryRecord iRec(its.p);

		Color3f Ld = emmits.size() * emmit->sample(iRec, sampler->next2D());


		Intersection its2;
		bool hit_res = scene->rayIntersect(iRec.shadowRay, its2);

		if (!hit_res || its2.mesh->getEmitter() != emmit)
			Ld = Color3f(0);

		float cos0 = std::abs(its.geoFrame.n.dot(iRec.wi));


		BSDFQueryRecord query1 = BSDFQueryRecord(its.toLocal(iRec.wi),
			its.toLocal(-ray.d), ESolidAngle);


		Color3f bsdfVal1 = bsdf->eval(query1);

		Ld = Ld * bsdfVal1 * cos0;


		
		

		//Get Li
		
		Vector3f toCam = -ray.d.normalized();


		BSDFQueryRecord query(its.toLocal(toCam)); //wi Camera, wo sampled ray
		//query.p = its.p;
		Color3f bsdfVal = bsdf->sample(query, sampler->next2D());
		

		//Check if intersect with emitter
		Ray3f lightRay(its.p, its.toWorld(query.wo));


		//if (sampler->next1D() > m_q)
		//	return  Le + Ld + this->Li(scene, sampler, lightRay) * bsdfVal / (1.f - m_q);
		//else
		//	return  Le + Ld;
		
	
		Color3f res;

		if (sampler->next1D() > m_q)
			/*res =  Le + 0.5*Ld+ 0.5*this->Li(scene, sampler, lightRay) * bsdfVal / (1.f - m_q);*/
			res = Le + Ld;

		else
			res =  Le;
		return res;
	}


	std::string toString() const {
		return "DirectPATHMATSIntegrator[]";
	}

private:
	float m_q;
}
;

NORI_REGISTER_CLASS(EMSPATHMATSIntegrator, "path_ems");
NORI_NAMESPACE_END
