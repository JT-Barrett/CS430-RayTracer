#define SCENE_FILE "example.json"
#define OUTPUT_FILE "output.ppm"
#define ORIGIN 0
#define MAX_DEPTH 255
#define EXPECTED_ARGS 5


//MACROS
#define degrees_to_rad(degrees) (degrees * M_PI / 180.0)

//Needed Structs and Types

typedef struct Object{
  int kind; // 0 = cylinder, 1 = sphere, 2 = plane, 3 = camera
  double diffuse_color[3];
  double specular_color[3];
  double reflectivity;
  double refractivity;
  double ior;
  union {
    struct {
      double center[3];
      double radius;
    } sphere;
    struct {
      double center[3];
      double width;
      double height;
      double normal[3];
    } plane;
    struct {
      double width;
      double height;
    } camera;
    struct light{
      double center[3];
      double color[3];
      double direction[3];
      double theta;
      double radial_a2;
      double radial_a1;
      double radial_a0;
      double angular_a0;
    } light;
  };
} Object;

typedef struct dPixel{
  double r,g,b;
} dPixel;

typedef struct Pixel{
  unsigned char r,g,b;
} Pixel;

typedef Object Scene[128];
//Main method prototypes
int read_scene(char* json, Scene scene);
int raycast (Pixel *buffer, Scene scene, int num_objects, int width, int height);
int ppm_output(Pixel  *buffer, char *output_file_name, int size, int depth, int width, int height);

//Parser prototypes
double* next_vector(FILE* json);
double next_number(FILE* json);
char* next_string(FILE* json);
void skip_ws(FILE* json);
void expect_c(FILE* json, int d);
int next_c(FILE* json);

//Intersect prototypes and methods
double sphere_intersection(double* Ro, double* Rd, double* C, double r);
double plane_intersection(double* Ro, double* Rd, double* c, double *n);
static inline double sqr(double v) {
  return v*v;
}

void shoot_ray(Object *scene, int num_objects, double *Ro, double *Rd, double *t, int *o);
void shoot_ray_shadow(Object *scene, int num_objects, int best_obj, double *Ro, double *Rd, double *t, int *o);
void shade(Object *sceneRef, int num_objects, double best_t, int best_obj, double *Ro, double *Rd, double *color);


typedef double* V3;

static inline void v3_add(V3 a, V3 b, V3 c){
  c[0] = a[0] + b[0];
  c[1] = a[1] + b[1];
  c[2] = a[2] + b[2];
}

static inline void v3_sub(V3 a, V3 b, V3 c){
  c[0] = a[0] - b[0];
  c[1] = a[1] - b[1];
  c[2] = a[2] - b[2];
}

static inline void v3_scale(V3 a, double s, V3 c){
  c[0] = s * a[0];
  c[1] = s * a[1];
  c[2] = s * a[2];  
}

static inline double v3_dot(V3 a, V3 b){
  return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

static inline void v3_angle(V3 a, V3 b, double c){

}

static inline void  v3_reflect(V3 a, V3 b, double c){

}

static inline void v3_cross(V3 a, V3 b, V3 c){
  c[0] = a[1] * b[2] - a[2] * b[1];
  c[1] = a[2] * b[0] - a[0] * b[2];
  c[2] = a[0] * b[1] - a[1] * b[0];
}

static inline void v3_normalize(double* v) {
  double len = sqrt(sqr(v[0]) + sqr(v[1]) + sqr(v[2]));
  v[0] /= len;
  v[1] /= len;
  v[2] /= len;
}

static inline double v3_mag(double* v){
  double d = (double)sqrt(sqr(v[0]) + sqr(v[1]) + sqr(v[2]));
  return d;
}

static inline int clamp(double d){
  int final;
  if (d > 255.0) final = 255;
  else if (d < 0.0) final = 0;
  else final = floor(d);
  return final;
}

static inline double distance(V3 Ro, V3 p){
  return sqrt(sqr(Ro[0] - p[0]) + sqr(Ro[1] - p[1]) + sqr(Ro[2] - p[2]));
}

static inline double frad(double a0, double a1, double a2, double dl){
  return 1/(a2*sqr(dl) + a1*dl + a0);
}

static inline double fang(V3 Rd, V3 Ld, double theta, double a0){
  v3_normalize(Rd);
  v3_normalize(Ld);
  //v3_scale(Rd, -1, Rd);
  if(theta == 0)  
    return 1;
  double l_theta = v3_dot(Ld, Rd);
  if (acos(l_theta) > theta)
    return 0;
  else{
    return pow(l_theta, a0);
  }
}

static inline double deg_to_rad(double deg){
  double d = deg * M_PI / 180.0;
  return d;
} 