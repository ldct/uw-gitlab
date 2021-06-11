#version 330

uniform vec3 colour;
uniform int octaves;
uniform float minimumAmbient;
uniform vec3 lightxyz;
uniform vec3 normal;

out vec4 fragColor;
in vec3 f_position;

// Code cribbed from
// https://thebookofshaders.com/13/
// https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83
// http://www.iquilezles.org/www/articles/warp/warp.htm

float hash(float n) { return fract(sin(n) * 1e4); }
float hash(vec2 p) { return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x)))); }

float noise1(float x) {
	float i = floor(x);
	float f = fract(x);
	float u = f * f * (3.0 - 2.0 * f);
	return mix(hash(i), hash(i + 1.0), u);
}

float noise2(vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

float mod289(float x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 mod289(vec4 x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 perm(vec4 x){return mod289(((x * 34.0) + 1.0) * x);}

float noise3(vec3 p){
    vec3 a = floor(p);
    vec3 d = p - a;
    d = d * d * (3.0 - 2.0 * d);

    vec4 b = a.xxyy + vec4(0.0, 1.0, 0.0, 1.0);
    vec4 k1 = perm(b.xyxy);
    vec4 k2 = perm(k1.xyxy + b.zzww);

    vec4 c = k2 + a.zzzz;
    vec4 k3 = perm(c);
    vec4 k4 = perm(c + 1.0);

    vec4 o1 = fract(k3 * (1.0 / 41.0));
    vec4 o2 = fract(k4 * (1.0 / 41.0));

    vec4 o3 = o2 * d.z + o1 * (1.0 - d.z);
    vec2 o4 = o3.yw * d.x + o3.xz * (1.0 - d.x);

    return o4.y * d.y + o4.x * (1.0 - d.y);
}

float fbm3 (vec3 st) {
    float value = 0.0;
    float amplitude = .5;
    float frequency = 0.;
    for (int i = 0; i < octaves; i++) {
        value += amplitude * noise3(st);
        st *= 2.;
        amplitude *= .5;
    }
    return value;
}

float pattern( in vec3 p )
{
    vec3 q = vec3(
		fbm3( p + vec3(0.0,0.0,0.0) ),
        fbm3( p + vec3(5.2,1.3,2.2) ),
		fbm3( p + vec3(9.7,2.3,7.7) )
	);

    vec3 r = vec3(
		fbm3(p + 4.0*q + vec3(1.7,9.2,8.8) ),
        fbm3(p + 4.0*q + vec3(8.3,2.8,3.4) ),
        fbm3(p + 4.0*q + vec3(2.3,15.8,33.4) )
	);

    return fbm3( p + 4.0*r );
}


vec3 phongModel(vec3 fragPosition, vec3 fragNormal, vec3 textureColour) {

    vec3 light_position = lightxyz;

    vec3 ray = light_position - fragPosition;

    vec3 l = normalize(ray);
    vec3 v = normalize(-fragPosition.xyz);

    float light_attenuation = max(dot(fragNormal, l), 0.0)/length(ray);
    light_attenuation *= 3.0; // light strength
    light_attenuation = min(light_attenuation, 0.99);
    light_attenuation = max(minimumAmbient, light_attenuation);

	vec3 diffuse;
	diffuse = textureColour * light_attenuation;

    return diffuse;
}

void main() {
	float k = pattern(f_position.xyz);

    vec3 op_colour = vec3(1.0,1.0,1.0);
    // could be computed as uniform to save time
    op_colour.r = (colour.r - 1)*(colour.r - 1);
    op_colour.g = (colour.g - 1)*(colour.g - 1);
    op_colour.b = (colour.b - 1)*(colour.b - 1);


	fragColor = vec4(
        phongModel(
            f_position.xyz,
            normal,
            mix(colour, op_colour, k/2)),
		1.0f);
}
