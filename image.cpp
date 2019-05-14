#include "sphere.h"
#include "hitablelist.h"
#include "float.h"
#include "camera.h"
#include "material.h"


glm::vec3 color(const ray& r, hitable *world, int depth){
	hit_record rec;
	if (world->hit(r, 0.001, FLT_MAX, rec)){
		ray scattered;
		glm::vec3 attenuation;
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)){
			return attenuation*color(scattered, world, depth+1);
		}
		else {
			return glm::vec3(0,0,0);
		}
	}
	else {
		glm::vec3 unit_direction = r.direction() / glm::length(r.direction());
		float t = 0.5 * (unit_direction.y + 1.0);
		return float(1 - t) * glm::vec3(1.0, 1.0, 1.0) + t * glm::vec3(0.5, 0.7, 1.0);
	}
}

hitable *random_scene() {
    int n = 500;
    hitable **list = new hitable*[n+1];
    list[0] =  new sphere(glm::vec3(0,-1000,0), 1000, new lambertian(glm::vec3(0.5, 0.5, 0.5)));
    int i = 1;
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            float choose_mat = rand()%(RAND_MAX)/double(RAND_MAX);
            glm::vec3 center(a+0.9*(rand()%(RAND_MAX)/double(RAND_MAX)),0.2,b+0.9*(rand()%(RAND_MAX)/double(RAND_MAX))); 
            if ((center-glm::vec3(4,0.2,0)).length() > 0.9) { 
                if (choose_mat < 0.8) {  // diffuse
                    list[i++] = new sphere(center, 0.2, new lambertian(glm::vec3(
						(rand()%(RAND_MAX)/double(RAND_MAX))*(rand()%(RAND_MAX)/double(RAND_MAX)), 
						(rand()%(RAND_MAX)/double(RAND_MAX))*(rand()%(RAND_MAX)/double(RAND_MAX)), 
						(rand()%(RAND_MAX)/double(RAND_MAX))*(rand()%(RAND_MAX)/double(RAND_MAX)))));
                }
                else if (choose_mat < 0.95) { // metal
                    list[i++] = new sphere(center, 0.2,
                            new metal(glm::vec3(0.5*(1 + (rand()%(RAND_MAX)/double(RAND_MAX))), 
							0.5*(1 + (rand()%(RAND_MAX)/double(RAND_MAX))), 
							0.5*(1 + (rand()%(RAND_MAX)/double(RAND_MAX)))),  
							0.5*(rand()%(RAND_MAX)/double(RAND_MAX))));
                }
                else {  // glass
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }
            }
        }
    }

    list[i++] = new sphere(glm::vec3(0, 1, 0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(glm::vec3(-4, 1, 0), 1.0, new lambertian(glm::vec3(0.4, 0.2, 0.1)));
    list[i++] = new sphere(glm::vec3(4, 1, 0), 1.0, new metal(glm::vec3(0.7, 0.6, 0.5), 0.0));

    return new hitable_list(list,i);
}

int main() {
	int nx = 1200;
	int ny = 800;
	int ns = 100;
	hitable *list[5];
	list[0] = new sphere(glm::vec3(0,0,-1), 0.5, new lambertian(glm::vec3(0.1, 0.2, 0.5)));
	list[1] = new sphere(glm::vec3(0,-100.5, -1), 100.0, new lambertian(glm::vec3(0.8, 0.8, 0.0)));
	list[2] = new sphere(glm::vec3(1,0,-3), 0.5, new metal(glm::vec3(0.8, 0.6, 0.2), 0.0));
	//list[3] = new sphere(glm::vec3(-1,0,-1), 0.5, new metal(glm::vec3(0.8, 0.8, 0.8), 0.6));
	list[3] = new sphere(glm::vec3(-1,0,-1), 0.5, new dielectric(1.5));
	list[4] = new sphere(glm::vec3(-1,0,-1), -0.45, new dielectric(1.5));
	hitable *world = new hitable_list(list, 5);
	world = random_scene();

	glm::vec3 lookfrom(13,3,2);
	glm::vec3 lookat(0,0,0);
	//float dist_to_focus = (lookfrom - lookat).length();
	float dist_to_focus = 10.0;
	float aperture = 0.1;
	camera cam(lookfrom, lookat, glm::vec3(0,1,0), 20, float(nx)/float(ny),aperture, dist_to_focus);

	std::ofstream outfile;
	outfile.open("image.ppm", std::ios::out);
	outfile << "P3\n" << nx << " " << ny << "\n255\n";
	for (int j = ny - 1; j >= 0; j--) {
		for (int i = 0; i < nx; i++) {
			glm::vec3 col(0, 0, 0);
			for(int s = 0; s < ns; s++){
				float u = float(i + rand()%(RAND_MAX)/double(RAND_MAX)) / float(nx);
				float v = float(j + rand()%(RAND_MAX)/double(RAND_MAX)) / float(ny);
				ray r = cam.get_ray(u, v);
				//glm::vec3 p = r.point_at_parameter(2.0);
				col += color(r, world, 0);
			}
			col /= float(ns);
			col = glm::vec3(glm::sqrt(col.x), glm::sqrt(col.y), glm::sqrt(col.z));
			int ir = int(255.99 * col.r);
			int ig = int(255.99 * col.g);
			int ib = int(255.99 * col.b);
			outfile << ir << " " << ig << " " << ib << "\n";
		}
	}
	outfile.close();

	return 0;
}