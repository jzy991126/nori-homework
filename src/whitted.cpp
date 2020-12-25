#include <nori/integrator.h>
#include <nori/scene.h>
#include <nori/emitter.h>
#include <nori/sampler.h>
#include <nori/bsdf.h>
#include <iostream>

NORI_NAMESPACE_BEGIN

class WhittedIntegrator : public Integrator {
private:


public:
    WhittedIntegrator(const PropertyList& props) {

        /* No parameters this time */
    }

    Color3f Li(const Scene* scene, Sampler* sampler, const Ray3f& ray) const {
        /* Find the surface that is visible in the requested direction */
        Intersection its;
        if (!scene->rayIntersect(ray, its))
            return Color3f(0.0f);

        if (its.mesh->getBSDF()->isDiffuse())
        {
            const std::vector<Mesh*> emitters = scene->getEmmitMeshes();

            const BSDF* bsdf = its.mesh->getBSDF();

            Color3f result(0.0f);

            int randomIndex = rand() % emitters.size();

            Emitter* emi = emitters.at(randomIndex)->getEmitter();

            EmitterQueryRecord iRec = EmitterQueryRecord(its.p);

            const Emitter* emi2 = its.mesh->getEmitter();

            if (emi2)
            {
                EmitterQueryRecord iRec2(ray.o, its.p, its.shFrame.n);
                result = result + emi2->eval(iRec2);
            }

            Color3f Lo = emitters.size() * emi->sample(iRec, sampler->next2D()); //Instead of iterating over all emitter, mulply because MC

            Intersection its2;
            bool hit_res = scene->rayIntersect(iRec.shadowRay, its2);

            if (!hit_res || its2.mesh->getEmitter() != emi)
                Lo = Color3f(0);


            BSDFQueryRecord query = BSDFQueryRecord(its.toLocal(iRec.wi),
                its.toLocal(-ray.d), ESolidAngle);


            Color3f bsdfVal = bsdf->eval(query);

            float cos0 = std::abs(its.geoFrame.n.dot(iRec.wi));

            result = result + Lo * bsdfVal * cos0;
            return result;
        }

        else {

            const BSDF* bsdf = its.mesh->getBSDF();
            BSDFQueryRecord query = BSDFQueryRecord(its.toLocal(-ray.d.normalized()));

            bsdf->sample(query, sampler->next2D());
            float _pdf = bsdf->pdf(query);

            Ray3f out(its.p, its.toWorld(query.wo));

            float rand = sampler->next1D();

            if (rand < 0.95) {
                return Li(scene, sampler, out)/0.95;
            }
            else {
                return { 0,0,0 };
            }
        }



    }

    std::string toString() const {
        return "Whitted[]";
    }
};

NORI_REGISTER_CLASS(WhittedIntegrator, "whitted");
NORI_NAMESPACE_END