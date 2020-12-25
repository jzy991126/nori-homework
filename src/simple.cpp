#include <nori/integrator.h>
#include <nori/scene.h>
#include <iostream>

NORI_NAMESPACE_BEGIN

class SimpleIntegrator : public Integrator {
private:
    Point3f position;
    Color3f energy;
public:
    SimpleIntegrator(const PropertyList& props) {
        position = props.getPoint("position");
        energy = props.getColor("energy");
        std::cout << "success!" << std::endl;

        /* No parameters this time */
    }

    Color3f Li(const Scene* scene, Sampler* sampler, const Ray3f& ray) const {
        /* Find the surface that is visible in the requested direction */
        Intersection its;
        if (!scene->rayIntersect(ray, its))
            return Color3f(0.0f);
        
        Intersection its2;

        Vector3f dir = (its.p - position).normalized();
        Ray3f ray2(position, dir);


        scene->rayIntersect(ray2, its2);

        float lighted = 0;
        if (its2.p.isApprox(its.p, 1e-2))
            lighted = 1;

        float cosin = (-dir).dot(its.shFrame.n);

        return 0.25f*INV_PI*INV_PI*lighted*energy*fmax(cosin,0)/(position-its.p).squaredNorm();

        //return Color3f(res.x(), res.y(), res.z());


        /* Return the component-wise absolute
           value of the shading normal as a color */
        //Normal3f n = its.shFrame.n.cwiseAbs();
        //return Color3f(n.x(), n.y(), n.z());


    }

    std::string toString() const {
        return "SimpleIntegrator[]";
    }
};

NORI_REGISTER_CLASS(SimpleIntegrator, "simple");
NORI_NAMESPACE_END