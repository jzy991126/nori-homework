/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Wenzel Jakob

    Nori is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License Version 3
    as published by the Free Software Foundation.

    Nori is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <nori/object.h>

NORI_NAMESPACE_BEGIN



struct EmitterQueryRecord {
    /// Origin point from which we sample the emitter
    Point3f ref;
    /// Sampled point on the emitter
    Point3f p;
    /// Normal at the emitter point
    Normal3f n;
    /// Direction between the hit point and the emitter point
    Vector3f wi;
    /// Probability
    float pdf;
    /// Shadow ray
    Ray3f shadowRay;

    /// Create an unitialized query record
    EmitterQueryRecord() = default;

    /// Create a new query record that can be used to sample a emitter
    EmitterQueryRecord(const Point3f& ref) : ref(ref) { }

    /**
     * \brief Create a query record that can be used to query the
     * sampling density after having intersected an area emitter
     */
    EmitterQueryRecord(const Point3f& ref, const Point3f& p, const Normal3f& n) :
        ref(ref), p(p), n(n) {
        wi = (p - ref).normalized();
    }
};

/**
 * \brief Superclass of all emitters
 */
class Emitter : public NoriObject {

protected:
    Mesh* m_mesh;
public:

    /**
     * \brief Return the type of object (i.e. Mesh/Emitter/etc.) 
     * provided by this instance
     * */
    EClassType getClassType() const { return EEmitter; }

    virtual Color3f sample(EmitterQueryRecord& IRec, const Point2f& sample) const = 0;

    virtual Color3f eval(const EmitterQueryRecord& lRec) const = 0;


    virtual float pdf(const EmitterQueryRecord& lRec) const = 0;

    void setMesh(Mesh* mesh) { m_mesh = mesh; }
};

NORI_NAMESPACE_END
