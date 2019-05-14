#ifndef CAMERAH
#define CAMERAH

#include "ray.h"

glm::vec3 random_in_unit_disk(){
    glm::vec3 p;
    do{
        p = 2.0f * glm::vec3( rand()%(RAND_MAX)/double(RAND_MAX), rand()%(RAND_MAX)/double(RAND_MAX), 0 ) - glm::vec3(1,1,0);  
    }while(glm::dot(p,p) >= 1.0);
    return p;
}

class camera{
    public:
        camera(glm::vec3 lookfrom, glm::vec3 lookat, glm::vec3 vup, float vfov, float aspect, float aperture, float focus_dist) {//vfov is top to bottom in degrees
            lens_radius = aperture / 2;
            float theta = vfov*(atan(1)*4)/180;
            float half_height = tan(theta/2);
            float half_width = aspect * half_height;
            origin = lookfrom;
            w = (lookfrom - lookat) / glm::length(lookfrom - lookat);
            u = glm::cross(vup, w) / glm::length(glm::cross(vup, w));
            v = glm::cross(w,u);
            //lower_left_corner = glm::vec3(-half_width, -half_height, -1.0);
            lower_left_corner = origin - half_width*u*focus_dist - half_height*v*focus_dist - w*focus_dist;
            horizontal = 2 * half_width * u * focus_dist;
            vertical = 2 * half_height * v * focus_dist;
        }
    ray get_ray(float s, float t) {
        glm::vec3 rd = lens_radius*random_in_unit_disk();
        glm::vec3 offset = u * rd.x + v * rd.y;
        return ray(origin + offset, lower_left_corner + s*horizontal + t*vertical - origin - offset); 
    };

    glm::vec3 origin;
    glm::vec3 lower_left_corner;
    glm::vec3 horizontal;
    glm::vec3 vertical;
    glm::vec3 u,v,w;
    float lens_radius;
};

#endif