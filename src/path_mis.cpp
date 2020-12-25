#include <nori/integrator.h>
#include <nori/scene.h>
#include <nori/bsdf.h>
#include <nori/emitter.h>
#include <nori/sampler.h>
#include <nori/warp.h>
#include <iostream>
NORI_NAMESPACE_BEGIN

const float eps = 1e-7;

class DirectPATHMISIntegrator : public Integrator {
public:
	DirectPATHMISIntegrator(const PropertyList& props) {
		m_q = 0.2;
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

		float pdfa1 = emmit->pdf(iRec);
		float pdfa2 = bsdf->pdf(query1);

		//std::cout << "a  " << pdfa1 << " " << pdfa2 << std::endl;


		Color3f bsdfVal1 = bsdf->eval(query1);

		Ld = Ld * bsdfVal1 * cos0;



		//Get Li

		Vector3f toCam = -ray.d.normalized();


		BSDFQueryRecord query(its.toLocal(toCam)); //wi Camera, wo sampled ray
		//query.p = its.p;
		Color3f bsdfVal = bsdf->sample(query, sampler->next2D());
		Ray3f lightRay(its.p, its.toWorld(query.wo));


		float pdfb1 = bsdf->pdf(query);
		float pdfb2 = 0;

		//std::cout << pdfb1 << std::endl;

		Intersection temp;




		//Check if intersect with emitter
		

		if (scene->rayIntersect(lightRay, temp))
		{
			if (temp.mesh->isEmitter())
			{
				EmitterQueryRecord eqr(temp.p);
				
				temp.mesh->getEmitter()->sample(eqr, sampler->next2D());
				pdfb2 = eqr.pdf;
				//std::cout << pdfb2  <<" ----"<< std::endl;
			}
		}

		//std::cout << "b  " << pdfb1 << " " << pdfb2 << std::endl;

		float w1, w2;

		w1 = pdfa1 + pdfa2 >= eps ? pdfa1 /(pdfa1 + pdfa2) : .0f;
		w2 = pdfb1 + pdfb2 >= eps ? pdfb1 /(pdfb1 + pdfb2) : .0f;

		if (w1 < .0f || w1>1.f) {
			w1 = .0f;
		}
		if (w2 < .0f || w2>1.f) {
			w2 = .0f;
		}

		//std::cout << w1 << " " << w2 << std::endl;


		//if (sampler->next1D() > m_q)
		//	return  Le + Ld + this->Li(scene, sampler, lightRay) * bsdfVal / (1.f - m_q);
		//else
		//	return  Le + Ld;

		//std::cout << w1 << " " << w2 << " "<<bsdfVal<<std::endl;

		Color3f res;

		if (sampler->next1D() > m_q)
			res = Le + w1 * Ld +w2 * this->Li(scene, sampler, lightRay) * bsdfVal / (1.f - m_q);
			//res = Le + Ld;

		else
			res = Le;
		return res;

	}

	std::string toString() const {
		return "DirectPATHMISIntegrator[]";
	}

private:
	float m_q;
}
;

NORI_REGISTER_CLASS(DirectPATHMISIntegrator, "path_mis");
NORI_NAMESPACE_END
