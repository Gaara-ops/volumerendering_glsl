// Ray Axis Aligned Bounding Box intersection
#version 430

//http://prideout.net/blog/?p=64
//http://www.alanzucconi.com/2016/07/01/volumetric-rendering/
struct Ray {
  vec3 Origin;
  vec3 Dir;
};

struct AABB {
  vec3 Min;
  vec3 Max;
};

// Intersect ray with a box
// http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm
bool IntersectBox (Ray r, AABB box, out float ts, out float tf)
{
  vec3 invR = vec3(1.0) / r.Dir;
  
  vec3 tMin = invR * (box.Min - r.Origin);
  vec3 tMax = invR * (box.Max -  r.Origin);
   
  vec3 t1 = min(tMin, tMax);
  vec3 t2 = max(tMin, tMax);
  
  float tnear = max(max(t1.x, t1.y), t1.z);
  float tfar  = min(min(t2.x, t2.y), t2.z);
  
  ts = tnear;
  tf = tfar;

  return tfar > tnear;
}

bool RayAABBIntersection (vec3 vert_eye, vec3 vert_dir, vec3 aabbmin, vec3 aabbmax,
                          out Ray r, out float s1, out float rtnear, out float rtfar)
{
  r.Origin = vert_eye;
  r.Dir = normalize(vert_dir);
  
  AABB aabb_box;
  aabb_box.Min = aabbmin;
  aabb_box.Max = aabbmax;

  float tnear, tfar;
  bool hit = IntersectBox(r, aabb_box, tnear, tfar);

  if (hit)
  {
    if (tnear < 0.0) tnear = 0.0;

    s1 = abs(tfar - tnear) / 1.0;

    rtnear = tnear;
    rtfar  = tfar;
  }

  return hit;
}