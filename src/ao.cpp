#include <nori/integrator.h>
#include <nori/scene.h>
#include <iostream>
#include <nori/warp.h>

NORI_NAMESPACE_BEGIN

class AoIntegrator : public Integrator {
public:
    AoIntegrator(const PropertyList& props) {

        /* No parameters this time */
    }

    Color3f Li(const Scene* scene, Sampler* sampler, const Ray3f& ray) const {

        
        Intersection its;
        if (!scene->rayIntersect(ray, its))
            return Color3f(0.0f);

        Vector3f out = Warp::squareToCosineHemisphere(sampler->next2D());
        float pdf = Warp::squareToCosineHemispherePdf(out);

        out = its.geoFrame.toWorld(out).normalized();

        float lighted = 1;

        Ray3f ray2(its.p, out);
        Intersection its2;
        if (scene->rayIntersect(ray2, its2))
            lighted = 0;

        float cosin = (its.geoFrame.n.normalized()).dot(out);
        cosin = fmax(0, cosin);

        float res = lighted * cosin * INV_PI / pdf;

        return Color3f(res, res, res);

    }

    std::string toString() const {
        return "SimpleIntegrator[]";
    }
};

NORI_REGISTER_CLASS(AoIntegrator, "ao");
NORI_NAMESPACE_END