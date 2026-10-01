/* NOTES:
 * the book: https://raytracing.github.io/books/RayTracingInOneWeekend.html#surfacenormalsandmultipleobjects/anintervalclass
 * i and j in Camera::render() are swapped from the book; use i where they use j and vice versa
 * Point, Color, Vector classes from the book have all been combined into Vector. There is no operator overloading, so use the math functions defined in the Vector class
*/

#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <cstdlib>
#include <memory>   // needed for shared_ptr / make_shared
#include <limits>   // needed for numeric_limits
using namespace std;

// Constants that will help in the program later
const double PI = 3.1415926535897932385;
const double PI_RECIPROCAL = 1.0 / PI;
const double INFTY = std::numeric_limits<double>::infinity();

// Converts degrees to radians
double degrees_to_radians(double degrees) {
  return degrees * PI / 180.0;
}

// Generates random number from 0 to 1
double random_num() {
  int random = rand();
  return random / (RAND_MAX + 1.0);
}

// Generates random number between any interval
double random_num(double min, double max) {
  return min + random_num() * (max - min);
}

// Can create closed and open interval between two numbers
class Interval
{
private:
  double min;
  double max;

public:
  Interval(double min1, double max1)
  {
    min = min1;
    max = max1;
  }

  bool in_closed_interval(double t) {
    return (t >= this->min && t <= this->max) ? true : false;
  }

  bool in_open_interval(double t) {
    return (t > this->min && t < this->max) ? true : false;
  }

  double range() {
    return max - min;
  }

  double getMax() { return max; }
  double getMin() { return min; }

  double within_interval(double x) { // makes sure that x is within [min, max] and returns one of the extrema if out of bounds
    if (x < min) { return min; }
    if (x > max) { return max; }

    return x;
  }
};


struct Vector // Vector structure
{
  private:
    double x, y, z; // Components of the vector

  public:
    Vector(double x1 = 0, double y1 = 0, double z1 = 0) // Constructor to initialize a vector. Will set each component to 0 by default
    {
        x = x1;
        y = y1;
        z = z1;
    }

    // Getter methods
    double getX()
    {
        return this->x;
    }

    double getY()
    {
        return this->y;
    }

    double getZ()
    {
        return this->z;
    }

    // Calculating the cross product of two vectors
    Vector cross(Vector other) {
      double det_i = this->y*other.getZ() - this->z*other.getY();
      double det_j = this->z*other.getX() - this->x*other.getZ();
      double det_k = this->x*other.getY() - this->y*other.getX();

      return Vector(det_i, det_j, det_k);
    }

    // Vector math

    Vector plus(double k)
    {
        return Vector(this->x + k, this->y + k, this->z + k);
    }

    Vector plus(Vector other)
    {
        return Vector(this->x + other.getX(), this->y + other.getY(), this->z + other.getZ());
    }

    Vector minus(double k)
    {
        return Vector(this->x - k, this->y - k, this->z - k);
    }

    Vector minus(Vector other)
    {
        return Vector(this->x - other.getX(), this->y - other.getY(), this->z - other.getZ());
    }

    Vector times(double k)
    {
        return Vector(this->x * k, this->y * k, this->z * k);
    }

    double dot(Vector other)
    {
        return (this->x * other.getX() + this->y * other.getY() + this->z * other.getZ());
    }

    Vector divide(double k)
    {
        return Vector(this->x / k, this->y / k, this->z / k);
    }

    Vector random()
    {
      return Vector(random_num(), random_num(), random_num());
    }

    Vector random(double min, double max)
    {
      return Vector(random_num(min, max), random_num(min, max), random_num(min, max));
    }

    // Useful methods for camera computation
    double magnitude() {
      return sqrt(x*x + y*y + z*z);
    }

    Vector normalize() {
      return Vector(this->x/magnitude(), this->y/magnitude(), this->z/magnitude()); // Divides the vector by its length i.e. normalizes the vector
    }

    Vector random_normalization() {
      while (true) {
        Vector random_vector = random(-1, 1);
        double magnitude_squared = random_vector.magnitude()*random_vector.magnitude();
        if (1e-160 < magnitude_squared && magnitude_squared <= 1) {
          return random_vector.normalize();
        }
      }
    }

    Vector hemisphere_random(Vector normal_vector) {
      Vector on_sphere = random_normalization();
      if(on_sphere.dot(normal_vector) > 0.0) { return on_sphere; }
      return on_sphere.times(-1);

    }

    Vector reflect(Vector vector, Vector unit_vector) {
      return vector.minus(unit_vector.times(2 * vector.dot(unit_vector)));
    }

    Vector refract(Vector normal, double eta) {
      double cos_theta = min(normal.dot(this->times(-1)), 1.0);
      Vector perpendicular = (this->plus(normal.times(cos_theta))).times(eta);
      Vector parallel = normal.times(-sqrt(abs(1.0 - perpendicular.magnitude()*perpendicular.magnitude())));
      return perpendicular.plus(parallel);
    }

    double gamma_from_linear(double component) {
      if (component > 0) {return sqrt(component); }
      return 0;
    }

    bool close_to_zero() {
      return (abs(x) < 1e-8) && (abs(y) < 1e-8) && (abs(z) < 1e-8);
    }

    void log_data()
    {
      clog << this->x << " " << this->y << " " << this->z << "\n";
    }

    void log_as_color()
    {
      x = gamma_from_linear(x);
      y = gamma_from_linear(y);
      z = gamma_from_linear(z);

      //Translate [0, 1] range into [0, 255] RGB range
      Interval component = Interval(0.000, 0.999);
      int rbyte = int(256 * component.within_interval(x));
      int gbyte = int(256 * component.within_interval(y));
      int bbyte = int(256 * component.within_interval(z));

      clog << rbyte << " " << gbyte << " " << bbyte << "\n";
    }

    void print_color()
    {
      x = gamma_from_linear(x);
      y = gamma_from_linear(y);
      z = gamma_from_linear(z);

      //Translate [0, 1] range into [0, 255] RGB range
      Interval component = Interval(0.000, 0.999);
      int rbyte = int(256 * component.within_interval(x));
      int gbyte = int(256 * component.within_interval(y));
      int bbyte = int(256 * component.within_interval(z));

      cout << rbyte << " " << gbyte << " " << bbyte << "\n";
    }
};

class Ray {
private:
  Vector O;
  Vector D; // Using the Ray parametric formula O + tD

public:
  Ray(Vector O1 = Vector(), Vector D1 = Vector()) // Constructor  ith O1 and D1 both initially set to (0, 0, 0)
  {
    O = O1;
    D = D1;
  }

  // Getter Methods

  Vector getO() {
    return this->O;
  }

  Vector getD() {
    return this->D;
  }

  // Determining the ray is at what point at input t

  Vector at(double t) {
    return this->O.plus(this->D.times(t));
  }
};

class Material;

class Hit
{
private:
  double t;
  bool front;
  Vector P;
  Vector norm;
  shared_ptr<Material> mat;

public:
  Hit(double t1 = 0.0, bool front1 = false, Vector P1 = Vector(), Vector Norm1 = Vector())
  {
    t = t1;
    front = front1;
    P = P1;
    norm = Norm1;
  }

  void set_face_norm(Ray& r, Vector& out_norm) {
    if (r.getD().dot(out_norm) < 0) { // If the ray and the normal are in opposite directions then the ray is outside the hittable object
      front = true;
      norm = out_norm; // Set normal to point outwards
    } else { // Otherwise the ray is on the inside of the object
      front = false;
      norm = (Vector()).minus(out_norm); // Set the normal to point inwards
    }
  }

  double getT() { return this->t; }
  bool getFront() { return this->front; }
  Vector getP() { return this->P; }
  Vector getNorm() { return this->norm; }
  shared_ptr<Material> getMat() { return this->mat; }

  void setT(double t1) { this->t = t1; }
  void setfront(bool front1) { this->front = front1; }
  void setP(Vector P1) { this->P = P1; }
  void setNorm(Vector Norm1) { this->norm = Norm1; }
  void setMat(shared_ptr<Material> mat) { this->mat = mat; }
};

class Hittable {
public:
  virtual ~Hittable() = default;
  virtual bool hit(Ray& r, Interval& t, Hit& h) = 0;
};

class Sphere : public Hittable
{
private:
  Vector C; // Center of the sphere
  double radius;
  shared_ptr<Material> mat;

public:
  Sphere(Vector C1 = Vector(), double radius1 = 0.0) // Constructor
  {
    C = C1;
    radius = radius1;
  }

  Sphere(Vector C1, double radius1, shared_ptr<Material> mat1) {
    C = C1;
    radius = radius1;
    mat = mat1;
  }
    
  // Applies the quadratic formula to find at what t the ray hits
  bool hit(Ray& r, Interval& t, Hit& h) override {
    // Deriving a, b, and c of at^2 + bt + c = 0
    double a = r.getD().dot(r.getD());
    double b = r.getD().dot(this->C.minus(r.getO()));
    double c = (this->C.minus(r.getO())).dot(this->C.minus(r.getO())) - this->radius*this->radius;

    double disc = b*b - a * c;
    if(disc < 0)
    {
      return false; // Ray does not intersect with the sphereAS 
    }

    double root = 0.0;

    // Calculating both possible roots
    double root1 = (b - sqrt(disc)) / (a);
    double root2 = (b + sqrt(disc)) / (a);

    // Checking to find which t fits in the interval of [t_min, t_max]
    if(t.in_closed_interval(root1)) {
      root = root1;
    } else if (t.in_closed_interval(root2)) {
      root = root2;
    } else {
      return false;
    }

    // Setting the record of where it was hit
    h.setT(root);
    h.setP(r.at(h.getT()));
    Vector out_norm = (h.getP().minus(this->C)).divide(this->radius);
    h.set_face_norm(r, out_norm);
    h.setMat(mat);

    return true;
  }

};

class HittableList : public Hittable // Code taken from https://raytracing.github.io/books/RayTracingInOneWeekend.html#surfacenormalsandmultipleobjects/alistofhittableobjects
{
public:
  vector<shared_ptr <Hittable> > objects;

  HittableList()
  {

  }

  HittableList(shared_ptr<Hittable> object) { add(object); }

  void clear()
  {
    objects.clear();
  }

  void add(shared_ptr<Hittable> object)
  {
    objects.push_back(object);
  }

  bool hit(Ray& r, Interval& t, Hit& h) override {
    Hit temp_record;
    double closest = t.getMax();
    bool hit_anything = false;

    for(shared_ptr<Hittable> object : objects) {
      Interval i = Interval(t.getMin(), closest);
      if(object->hit(r, i, temp_record)) {
        hit_anything = true;
        closest = temp_record.getT();
        h = temp_record;
      }
    }

    return hit_anything;
  }
};

class Material { // Code taken from https://raytracing.github.io/books/RayTracingInOneWeekend.html#surfacenormalsandmultipleobjects/alistofhittableobjects
  public:
    virtual ~Material() = default;

    virtual bool scatter(Ray& r_in, Hit& rec, Vector& attenuation, Ray& scattered) = 0;

};

class Lambertian : public Material {
  public:
    Lambertian(Vector albedo1) : albedo(albedo1) {}

    bool scatter(Ray& r_in, Hit& rec, Vector& attenuation, Ray& scattered) override {
        Vector scatter_direction = Vector(0, 0, 0);
        scatter_direction = (rec.getNorm()).plus(scatter_direction.random_normalization());

        if (scatter_direction.close_to_zero()) { scatter_direction = rec.getNorm(); }

        scattered = Ray(rec.getP(), scatter_direction);
        attenuation = albedo;
        return true;
    }

  private:
    Vector albedo;
};

class Metal : public Material {
  public:
    Metal(Vector albedo1, double fuzz1) {
      albedo = albedo1;
      if(fuzz1 < 1)
      {
        fuzz = fuzz1;
      }
      else
      {
        fuzz = 1;
      }
    }

    bool scatter(Ray& r_in, Hit& rec, Vector& attenuation, Ray& scattered) override {
      Vector reflected = Vector();
      reflected = reflected.reflect(r_in.getD(), rec.getNorm());
      reflected = reflected.normalize().plus(reflected.random_normalization().times(fuzz));
      scattered = Ray(rec.getP(), reflected);
      attenuation = albedo;
      return (scattered.getD().dot(rec.getNorm()) > 0);
    }

  private:
    Vector albedo;
    double fuzz;
};

class Dielectric : public Material {
  public:
    Dielectric(double refraction_index1)
    {
      refraction_index = refraction_index1;
    }

    bool scatter(Ray& r_in, Hit& rec, Vector& attenuation, Ray& scattered) override {
        attenuation = Vector(1, 1, 1);

        double ri;
        if(rec.getFront())
        {
          ri = 1.0/refraction_index;
        }
        else
        {
          ri = refraction_index;
        }

        Vector unit_direction = r_in.getD().normalize();
        double cos_theta = min(unit_direction.dot(rec.getNorm().times(-1)), 1.0);
        double sin_theta = sqrt(1 - cos_theta*cos_theta);

        Vector direction = Vector();

        if(ri*sin_theta > 1.0 || reflectance(cos_theta, ri) > random_num())
        {
          direction = direction.reflect(unit_direction, rec.getNorm());
        }
        else
        {
          direction = unit_direction.refract(rec.getNorm(), ri);
        }

        scattered = Ray(rec.getP(), direction);
        return true;
    }

  private:
    double refraction_index;

    double reflectance(double cos, double index) {
      double r0 = ((1 - index) / (1 + index));
      r0 = r0*r0;
      return r0 + (1 - r0) * pow((1 - cos), 5);
    }

};

class Camera
{
  public:
    double aspect_ratio;
    int image_width;
    int samples_per_pixel;
    int max_depth;

    Camera(double ar = 16.0/9.0, int iw = 400, int spp = 100, int md = 50)
    {
      aspect_ratio = ar;
      image_width = iw;
      samples_per_pixel = spp;
      max_depth = md;
    }

    void render(HittableList& world)
    {
      initialize();

      // Formatting the output file so it is interpreted as an image
      cout << "P3\n" << image_width << " " << image_height << "\n255\n";

      for(int i = 0; i < image_height; i++)
      {
        clog << image_height - i << " lines remaining" << "\n";
        for(int j = 0; j < image_width; j++)
        {
          Vector pixel = Vector();
          for (int k = 0; k < samples_per_pixel; k++) {
              Ray r = get_ray(j, i);
              pixel = pixel.plus(color_of_ray(r, max_depth, world));
          }

          Vector pixel_print = pixel.divide(samples_per_pixel);
          pixel_print.print_color();
        }
      }
    }

  private:
    int image_height;
    Vector center;
    Vector pixel00_loc;
    Vector pixel_delta_u;
    Vector pixel_delta_v;

    void initialize()
    {
      image_height = int(image_width/aspect_ratio);
      image_height = (image_height < 1) ? 1 : image_height;
      center = Vector();

      double focal_length = 1.0;
      double viewport_height = 2.0;
      double viewport_width = viewport_height*(double(image_width)/image_height);

      Vector viewport_u = Vector(viewport_width, 0, 0);
      Vector viewport_v = Vector(0, -viewport_height, 0);

      // Calculate the horizontal and vertical delta vectors from pixel to pixel.
      pixel_delta_u = viewport_u.divide(image_width);
      pixel_delta_v = viewport_v.divide(image_height);

      // Calculate the location of the upper left pixel.
      Vector viewport_upper_left = center.minus(Vector(0, 0, focal_length).plus(viewport_u.times(0.5)).plus(viewport_v.times(0.5)));

      pixel00_loc = viewport_upper_left.plus((pixel_delta_u.plus(pixel_delta_v)).times(0.5));
    }

    Ray get_ray(int j, int i) {
      Vector offset = square_of_samples();
      Vector sample_pixel = pixel00_loc.plus((pixel_delta_u.times(j + offset.getX())).plus(pixel_delta_v.times(i + offset.getY())));

      Vector ray_direction = sample_pixel.minus(center);
      return Ray(center, ray_direction);
    }

    Vector square_of_samples() {
        return Vector(random_num() - 0.5, random_num() - 0.5, 0);
    }

    Vector color_of_ray(Ray r, int depth, HittableList& world)
    {
      if (depth <= 0) { return Vector(0, 0, 0); }

      Hit record;
      Interval i = Interval(0.001, INFTY);
      if(world.hit(r, i, record))
      {
        Vector attenuation;
        Ray scattered;
        if (record.getMat()->scatter(r, record, attenuation, scattered)) {
          Vector ray_color = color_of_ray(scattered, depth - 1, world);
          return Vector(attenuation.getX() * ray_color.getX(), attenuation.getY() * ray_color.getY(), attenuation.getZ() * ray_color.getZ());
        }
      }

      Vector dir = r.getD().normalize();
      double blend = 0.5*(dir.getY() + 1.0);
      Vector col = (Vector(1.0, 1.0, 1.0).times(1.0-blend)).plus(Vector(0.5, 0.7, 1.0).times(blend));
      return col;
    }
};

int main()
{
  // World list of hittable objects
  HittableList world;

  auto material_ground = make_shared<Lambertian>(Vector(0.8, 0.8, 0.0));
  auto material_center = make_shared<Lambertian>(Vector(0.1, 0.2, 0.5));
  auto material_left = make_shared<Dielectric>(1.00/1.33);
  auto material_bubble = make_shared<Dielectric>(1.00 / 1.50);
  auto material_right = make_shared<Metal>(Vector(0.8, 0.6, 0.2), 1.0);

  world.add(make_shared<Sphere>(Vector( 0.0, -100.5, -1.0), 100.0, material_ground));
  world.add(make_shared<Sphere>(Vector( 0.0, 0.0, -1.2), 0.5, material_center));
  world.add(make_shared<Sphere>(Vector(-1.0, 0.0, -1.0), 0.5, material_left));
  world.add(make_shared<Sphere>(Vector(-1.0, 0.0, -1.0), -0.4, material_bubble));
  world.add(make_shared<Sphere>(Vector( 1.0, 0.0, -1.0), 0.5, material_right));

  Camera c = Camera();

  c.render(world);

  return 0;
}
