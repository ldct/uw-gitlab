#version 330

struct LightSource {
    vec3 position;
    vec3 rgbIntensity;
};

in VsOutFsIn {
	vec3 position_ES; // Eye-space position
	vec3 normal_ES;   // Eye-space normal
	LightSource light;
} fs_in;


out vec4 fragColour;

struct Material {
    vec3 kd;
    vec3 ks;
    float shininess;
};
uniform Material material;

uniform vec3 ambientIntensity;

vec3 phongModel(vec3 p, vec3 n) {
	LightSource light = fs_in.light;

    // Direction from fragment to light source.
    vec3 l = normalize(light.position - p);

    // Direction from fragment to viewer (origin - p).
    vec3 v = normalize(-p);

    float n_dot_l = max(dot(n, l), 0.0);

	vec3 diffuse;
	diffuse = material.kd * n_dot_l;

    vec3 specular = vec3(0.0);

    if (n_dot_l > 0.0) {
		// Halfway vector.
		vec3 h = normalize(v + l);
        float n_dot_h = max(dot(n, h), 0.0);

        specular = material.ks * pow(n_dot_h, material.shininess);
    }

    return ambientIntensity + light.rgbIntensity * (diffuse + specular);
}

uniform int mode;

void main() {
    if (mode == 0) {
        fragColour = vec4(material.kd, 1.0);
    } else if (mode == 1) {
        fragColour = vec4(0.5,0.0,0.5,0.5);
    } else {
    	fragColour = vec4(phongModel(fs_in.position_ES, fs_in.normal_ES), 1.0);
    }
}
