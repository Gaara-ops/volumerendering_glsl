#version 430

in vec3 vert_eye;
in vec3 vert_dir;

layout (location = 0) out vec4 FragColor;

uniform sampler3D TexVolume;
uniform sampler1D TexTransferFunc;

uniform vec3 VolumeScaledSizes;

uniform int ApplyPhongShading;

uniform float StepSize;

//////////////////////////////////////////////////////////////////////////////////////////////////
// From blinnphonggradient.frag
vec3 ShadeBlinnPhong (vec3 Wpos, vec3 Tpos, vec3 eyepos, vec3 clr);
//////////////////////////////////////////////////////////////////////////////////////////////////
// From raybboxintersection.frag
struct Ray { vec3 Origin; vec3 Dir; };
bool RayAABBIntersection (vec3 vert_eye, vec3 vert_dir, vec3 aabbmin, vec3 aabbmax,
                          out Ray r, out float s1, out float rtnear, out float rtfar);
//////////////////////////////////////////////////////////////////////////////////////////////////

vec4 GetFromTransferFunction (vec3 tex_pos)
{
  float vol_density = texture(TexVolume, tex_pos / VolumeScaledSizes).r;
  return texture(TexTransferFunc, vol_density);
}

void main (void)
{
  float T = 1.0;
  float s1 = 1.0;
  vec3 hvolscalesize = VolumeScaledSizes * 0.5;

  Ray r; float tnear, tfar;
  bool inbox = RayAABBIntersection(vert_eye, vert_dir, -hvolscalesize,
                                   hvolscalesize, r, s1, tnear, tfar);

  if (!inbox) discard;
  FragColor = vec4(1.0, 1.0, 1.0, 0.0);

  // Initialization Parameters
  vec3 I = vec3(0);
  float s = 0.0;

  // Get initial pos
  vec3 pos = r.Origin + r.Dir * tnear;
  // Translate pos [VolumeSizes/2, VolumeSizes/2] to pos_from_zero [0, VolumeSizes]
  pos = pos + hvolscalesize;
  
  while (s < s1)
  {
    float d = min(StepSize, s1 - s);
    
    // Update Sample Position
    pos = pos + r.Dir * d;
    vec4 src = GetFromTransferFunction(pos);
   
    vec3 Le = src.rgb;

    // if alpha is > 0.0, apply shading
    if (src.a > 0.0)
    {
      // Apply Blinn-Phong Shading using Gradient Texture
      if (ApplyPhongShading == 1)
        Le = ShadeBlinnPhong(pos - hvolscalesize, pos, r.Origin, src.rgb);
    }

    // From "Local and Global Illumination in the Volume Rendering Integral" (2010)
    // . Integrating using ray segments
    float segT = exp(-src.a * d);
      
    // Update color and transparency
    I += T * (1.0 - segT) * Le;
    T = T * segT;

    // equals to 'opacity > 0.95', since opacity = 1.0 - T
    if (T < 0.05)
      break; 

    s = s + d;
  }
  
  FragColor = vec4(I, 1.0 - T);
}