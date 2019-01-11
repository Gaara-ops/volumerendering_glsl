#version 430

uniform sampler3D TexVolumeGradient;
uniform vec3 VolumeScaledSizes;

// Blinn-Phong Parameters
uniform float Kambient;
uniform float Kdiffuse;
uniform float Kspecular;
uniform float Nshininess;
uniform vec3 Ispecular;

uniform vec3 WorldLightingPos;

vec3 FetchGradientVolume (vec3 n_pos)
{
  return texture(TexVolumeGradient, n_pos / VolumeScaledSizes).xyz;
}

vec3 ShadeBlinnPhong (vec3 Wpos, vec3 Tpos, vec3 eyepos, vec3 clr)
{
  vec3 gradient_normal = FetchGradientVolume(Tpos);
  
  if (gradient_normal != vec3(0, 0, 0))
  {
    gradient_normal      = normalize(gradient_normal);
    vec3 light_direction = normalize(WorldLightingPos - Wpos);
    vec3 eye_direction   = normalize(eyepos - Wpos);
    vec3 halfway_vector  = normalize(eye_direction + light_direction);
  
    float dot_diff = max(0, dot(gradient_normal, light_direction));
    float dot_spec = max(0, dot(halfway_vector, gradient_normal));
   
    clr = (1.0 / (Kambient + Kdiffuse + Kspecular)) * (
      //////////////////////////////////////////////
      // rgb only affects ambient + diffuse
      (clr * (Kambient + Kdiffuse * dot_diff)) 
      //////////////////////////////////////////////
      // specular contribution has it's own color
      + Kspecular * Ispecular * pow(dot_spec, Nshininess)
    );
  }

  return clr;
}