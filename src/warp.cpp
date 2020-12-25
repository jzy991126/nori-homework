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

#include <nori/warp.h>
#include <nori/vector.h>
#include <nori/frame.h>

NORI_NAMESPACE_BEGIN

Point2f Warp::squareToUniformSquare(const Point2f &sample) {
    return sample;
}

float Warp::squareToUniformSquarePdf(const Point2f &sample) {
    return ((sample.array() >= 0).all() && (sample.array() <= 1).all()) ? 1.0f : 0.0f;
}

Point2f Warp::squareToTent(const Point2f &sample) {

    float x = sample.x() <= 0.5 ? sqrt(2 * sample.x()) - 1 : 1- sqrt(2 * (1 - sample.x()));
    float y = sample.y() <= 0.5 ? sqrt(2 * sample.y()) - 1 : 1- sqrt(2 * (1 - sample.y()));
    return Point2f(x, y);
    /*throw NoriException("Warp::squareToTent() is not yet implemented!");*/
}

float Warp::squareToTentPdf(const Point2f &p) {
    /*throw NoriException("Warp::squareToTentPdf() is not yet implemented!");*/
    //return ((p.array() >= -1).all() && (p.array() <= 1).all()) ? 0.25*(1-abs(p.x()))* (1 - abs(p.y())) : 0.0f;
     //return ((p.array() >= -1).all() && (p.array() <= 1).all()) ? 0.25 : 0.0f;

    return ((p.array() >= -1).all() && (p.array() <= 1).all()) ? (1 - abs(p.x())) * (1 - abs(p.y())) : 0.0f;
}

Point2f Warp::squareToUniformDisk(const Point2f &sample) {
    /*throw NoriException("Warp::squareToUniformDisk() is not yet implemented!");*/
    /*return sample * 2 - Point2f(1, 1);*/
    Point2f a(sin(sample.x()*2*M_PI),cos(sample.x()*2*M_PI ));
    return sqrt(sample.y()) * a;
}

float Warp::squareToUniformDiskPdf(const Point2f &p) {
    /*throw NoriException("Warp::squareToUniformDiskPdf() is not yet implemented!");*/

    return  p.squaredNorm() <= 1.0 ? INV_PI : 0;
    //p.norm()
    //if(p.squaredNorm()<=1)
    //return INV_PI; 
}

Vector3f Warp::squareToUniformSphere(const Point2f &sample) {
    /*throw NoriException("Warp::squareToUniformSphere() is not yet implemented!");*/
    Point2f a = squareToUniformDisk(sample);

    float r2 = a.squaredNorm();

    float x = 2 * a.x() * sqrt(1 - r2);
    float y = 2 * a.y() * sqrt(1 - r2);
    float z = 1 - 2 * r2;

    return Vector3f(x, y, z);

}

float Warp::squareToUniformSpherePdf(const Vector3f &v) {
    /*throw NoriException("Warp::squareToUniformSpherePdf() is not yet implemented!");*/

    return 0.25 * INV_PI;
}

Vector3f Warp::squareToUniformHemisphere(const Point2f &sample) {
    //throw NoriException("Warp::squareToUniformHemisphere() is not yet implemented!");
    Vector3f a = squareToUniformSphere(sample);
    if (a.z() >= 0)
        return a;
    else
        return Vector3f(a.x(), a.y(), -a.z());
}

float Warp::squareToUniformHemispherePdf(const Vector3f &v) {
    /*throw NoriException("Warp::squareToUniformHemispherePdf() is not yet implemented!");*/
    
    return v.z() >= 0 ? 0.5 * INV_PI : 0;
}

Vector3f Warp::squareToCosineHemisphere(const Point2f &sample) {
    float theta = acos(2 * sample.x() - 1) / 2;
    float z = cos(theta);
    float r = sqrtf(1.f - z * z);
    float phi = 2 * M_PI * sample.y();
    return Vector3f(r * cos(phi), r * sin(phi), z);
}

float Warp::squareToCosineHemispherePdf(const Vector3f &v) {
    return v.z() > 0 ? v.z() / M_PI : 0.f;
}

Vector3f Warp::squareToBeckmann(const Point2f &sample, float alpha) {
    float phi = 2 * M_PI * sample.y();
    float theta = atan(sqrtf(-1 * alpha * alpha * log(1 - sample.x())));
    float z = cos(theta);
    float r = sqrtf(1.f - z * z);
    return Vector3f(r * cos(phi), r  * sin(phi), z);
}

float Warp::squareToBeckmannPdf(const Vector3f &m, float alpha) {
    float theta = acos(m.z());
    float D = 1 / (M_PI * alpha * alpha * cos(theta) * cos(theta) * cos(theta) * cos(theta)) * exp(-tan(theta) * tan(theta) / (alpha * alpha));
    return m.z() > 0 ? D * cos(theta) : 0.f;
}

NORI_NAMESPACE_END
