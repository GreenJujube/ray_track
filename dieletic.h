#ifndef DIELETIC_H
#define DIELETIC_H

#include "material.h"
#include "metal.h"
#include "vec3.h"
#include <cmath>

class dielectric:public material{
public:
    dielectric(float ri): ref_idx(ri) {}
    virtual int scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scatter) const;
    float ref_idx;
    float air_ref_idx = 1.0;
    float refract_over = 1.0;
};
/*
bool refract(const vec3 &v, const vec3 &n, float ni_over_nt, vec3 &refracted){
    vec3 I = unit_vector(v);
    float in = dot(I,n);
    float nt_over_ni = 1/ni_over_nt;
    float sqrt0 = 1-nt_over_ni*nt_over_ni*(1-in*in);
    if(sqrt0 > 0){
        refracted = sqrt(sqrt0)*in*n+nt_over_ni*(I-in*n);
        return 1;
    }
    else return 0;
    

}*/
int refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted) {
    vec3 uv = unit_vector(v);
    float dt = dot(uv, n);
    float discriminant = 1.0 - ni_over_nt*ni_over_nt*(1-dt*dt);
    if (discriminant > 0) {
        refracted = ni_over_nt*(uv - dt*n) - sqrt(discriminant)*n;
        return true;
    }
    else 
        return false;
}

float schlick(float cosine, float ref_idx){
    float r0 = (1-ref_idx)/(1+ref_idx);
    r0 = r0*r0;
    return r0+(1-r0)*pow((1-cosine),5);
}

int dielectric::scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered) const{
    vec3 outward_normal;
    vec3 reflected = reflect(r_in.direction(), rec.normal);
    float ni_over_nt;
    attenuation = vec3(1.0, 1.0, 1.0);
    float consine = 0;
    if(dot(r_in.direction(), rec.normal) > 0) {
        outward_normal = -rec.normal;
        ni_over_nt = ref_idx/r_in.get_ref_idx();
        consine = dot(r_in.direction(), rec.normal)/(r_in.direction().length()*rec.normal.length());

    }
    else{
        outward_normal = rec.normal;
        ni_over_nt = r_in.get_ref_idx()/ref_idx;
        consine = -dot(r_in.direction(), rec.normal)/(r_in.direction().length()*rec.normal.length());

    }
    float reflect_prob;
    vec3 refracted;
    if(refract(r_in.direction(), outward_normal, ni_over_nt, refracted)){
        reflect_prob = schlick(consine, ref_idx);
    }
    else reflect_prob = 1;
    if ((rand()%(100)/(float)(100)) < reflect_prob) {
        scattered = ray(rec.p, reflected, r_in.get_ref_idx());
    }
    else scattered = ray(rec.p, refracted, ref_idx);
    return 1;
}



#endif