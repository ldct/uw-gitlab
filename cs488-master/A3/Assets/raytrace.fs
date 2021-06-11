// cribbed from https://www.shadertoy.com/view/Xls3R7
// and https://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm

#version 330

uniform vec2 iResolution;
uniform bool merge_enabled;

const float maxd = 50.0;
const float precis = 0.001; // intersection epsilon
const float grad_step = 0.001;

in vec4 gl_FragCoord ;

out vec4 fragColour;

float sdEllipsoid( vec3 p, vec3 r ) {
  float k0 = length(p/r);
  float k1 = length(p/(r*r));
  return k0*(k0-1.0)/k1;
}

float sdRoundCone( vec3 p, float r1, float r2, float h ) {
  vec2 q = vec2( length(p.xz), p.y );

  float b = (r1-r2)/h;
  float a = sqrt(1.0-b*b);
  float k = dot(q,vec2(-b,a));

  if( k < 0.0 ) return length(q) - r1;
  if( k > a*h ) return length(q-vec2(0.0,h)) - r2;

  return dot(q, vec2(a,b) ) - r1;
}

float sphere(vec3 pos) {
	return length(pos)-1.0;
}

float dot2( vec2 v ) { return dot(v,v); }
float dot3( vec3 v ) { return dot(v,v); }
float sdJoint3DSphere( in vec3 p, in float l, in float a, in float w) {
  if( abs(a)<0.001 ) return length(p-vec3(0,clamp(p.y,0.0,l),0))-w;

  vec2  sc = vec2(sin(a),cos(a));
  float ra = 0.5*l/a;
  p.x -= ra;
  vec2 q = p.xy - 2.0*sc*max(0.0,dot(sc,p.xy));
  float u = abs(ra)-length(q);
  float d2 = (q.y<0.0) ? dot2( q+vec2(ra,0.0) ) : u*u;
  float s = sign(a);
  return sqrt(d2+p.z*p.z)-w;
}

float udTriangle( vec3 p, vec3 a, vec3 b, vec3 c ) {
  vec3 ba = b - a; vec3 pa = p - a;
  vec3 cb = c - b; vec3 pb = p - b;
  vec3 ac = a - c; vec3 pc = p - c;
  vec3 nor = cross( ba, ac );

  return sqrt(
    (sign(dot(cross(ba,nor),pa)) +
     sign(dot(cross(cb,nor),pb)) +
     sign(dot(cross(ac,nor),pc))<2.0)
     ?
     min( min(
     dot3(ba*clamp(dot(ba,pa)/dot3(ba),0.0,1.0)-pa),
     dot3(cb*clamp(dot(cb,pb)/dot3(cb),0.0,1.0)-pb) ),
     dot3(ac*clamp(dot(ac,pc)/dot3(ac),0.0,1.0)-pc) )
     :
     dot(nor,pa)*dot(nor,pa)/dot3(nor) );
}

float bentTriangle(vec3 p, vec3 a, vec3 b, vec3 tc, float a0) {
    const float k = 0.5;
    float c = cos(a0-k*p.x);
    float s = sin(a0-k*p.x);
    mat2  m = mat2(c,-s,s,c);
    vec3  q = vec3(m*p.xy,p.z);
    return udTriangle(q, a, b, tc);
}

float bentCone(vec3 p) {
    const float k = 0.01;
    float c = cos(-1.3+k*p.x);
    float s = sin(-1.3+k*p.x);
    mat2  m = mat2(c,-s,s,c);
    vec3  q = vec3(m*p.xy,p.z);
    return sdRoundCone(q, 0.3, 0.01, 4.0);
}

float smin_poly( float a, float b, float k ) {
    // TODO: there's a ~7% FPS penalty for using a uniform !!! wtf
    if (!merge_enabled) {
        return min(a, b);
    }
    float h = clamp( 0.5+0.5*(b-a)/k, 0.0, 1.0 );
    return mix( b, a, h ) - k*h*(1.0-h);
}

uniform vec3 body_main_pos; uniform vec3 body_main_scale; uniform mat3 body_main_rotinv;

uniform vec3 neck_0_pos; uniform vec3 neck_0_scale; uniform mat3 neck_0_rotinv; uniform float neck_0_aux;
uniform vec3 neck_1_pos; uniform vec3 neck_1_scale; uniform mat3 neck_1_rotinv; uniform float neck_1_aux;
uniform vec3 neck_2_pos; uniform vec3 neck_2_scale; uniform mat3 neck_2_rotinv; uniform float neck_2_aux;
uniform vec3 neck_3_pos; uniform vec3 neck_3_scale; uniform mat3 neck_3_rotinv; uniform float neck_3_aux;
uniform vec3 neck_4_pos; uniform vec3 neck_4_scale; uniform mat3 neck_4_rotinv; uniform float neck_4_aux;
uniform vec3 neck_5_pos; uniform vec3 neck_5_scale; uniform mat3 neck_5_rotinv; uniform float neck_5_aux;
uniform vec3 neck_6_pos; uniform vec3 neck_6_scale; uniform mat3 neck_6_rotinv; uniform float neck_6_aux;
uniform vec3 neck_7_pos; uniform vec3 neck_7_scale; uniform mat3 neck_7_rotinv; uniform float neck_7_aux;
uniform vec3 neck_8_pos; uniform vec3 neck_8_scale; uniform mat3 neck_8_rotinv; uniform float neck_8_aux;

uniform vec3 head_bulb_pos;
uniform vec3 head_bulb_scale;
uniform mat3 head_bulb_rotinv;
uniform float head_bulb_aux;

uniform vec3 h1_pos;
uniform vec3 h1_scale;
uniform mat3 h1_rotinv;
uniform float h1_aux;

uniform vec3 h2_pos;
uniform vec3 h2_scale;
uniform mat3 h2_rotinv;
uniform float h2_aux;

uniform vec3 h3_pos;
uniform vec3 h3_scale;
uniform mat3 h3_rotinv;
uniform float h3_aux;

float min3(float a, float b, float c) {
    return min(min(a,b),c);
}
float min7(float a, float b, float c, float d, float e, float f, float g) {
    return min(
        min(min(a,b),min(c,d)),
        min(min(e,f),g)
    );
}

vec3 rotateY(vec3 p, float a) {
    float sa = sin(a);
    float ca = cos(a);
    return vec3(ca*p.x + sa*p.z, p.y, -sa*p.x + ca*p.z);
}

vec3 rotateZ(vec3 p, float a) {
    float sa = sin(a);
    float ca = cos(a);
    return vec3(ca*p.x + sa*p.y, p.z, -sa*p.x + ca*p.y);
}

float scene_brown(vec3 pos) {

    float body_main = sdEllipsoid(body_main_rotinv*(pos - body_main_pos), body_main_scale);

    float neck_0 = sdRoundCone(neck_0_rotinv*(pos - neck_0_pos), neck_0_scale.x, neck_0_aux*neck_0_scale.x, (neck_0_scale.y));
    float neck_1 = sdRoundCone(neck_1_rotinv*(pos - neck_1_pos), neck_1_scale.x, neck_1_aux*neck_1_scale.x, (neck_1_scale.y));
    float neck_2 = sdRoundCone(neck_2_rotinv*(pos - neck_2_pos), neck_2_scale.x, neck_2_aux*neck_2_scale.x, (neck_2_scale.y));
    float neck_3 = sdRoundCone(neck_3_rotinv*(pos - neck_3_pos), neck_3_scale.x, neck_3_aux*neck_3_scale.x, (neck_3_scale.y));
    float neck_4 = sdRoundCone(neck_4_rotinv*(pos - neck_4_pos), neck_4_scale.x, neck_4_aux*neck_4_scale.x, (neck_4_scale.y));
    float neck_5 = sdRoundCone(neck_5_rotinv*(pos - neck_5_pos), neck_5_scale.x, neck_5_aux*neck_5_scale.x, (neck_5_scale.y));
    float neck_6 = sdRoundCone(neck_6_rotinv*(pos - neck_6_pos), neck_6_scale.x, neck_6_aux*neck_6_scale.x, (neck_6_scale.y));
    float neck_7 = sdRoundCone(neck_7_rotinv*(pos - neck_7_pos), neck_7_scale.x, neck_7_aux*neck_7_scale.x, (neck_7_scale.y));
    float neck_8 = sdRoundCone(neck_8_rotinv*(pos - neck_8_pos), neck_8_scale.x, neck_8_aux*neck_8_scale.x, (neck_8_scale.y));

    float head_bulb = sdEllipsoid(head_bulb_rotinv*(pos - head_bulb_pos), head_bulb_scale);

    float tail = bentCone(body_main_rotinv*(pos - body_main_pos));

    float bulb = smin_poly(neck_8, head_bulb, 0.5);
    float body_tail_0 = smin_poly(neck_0, smin_poly(body_main, tail, 0.01), 0.9);

    return min3(
        bulb, body_tail_0, min7(neck_1, neck_2, neck_3, neck_4, neck_5, neck_6, neck_7)
    );
}

float leaf(vec3 pos, float extent, float a0) {
    return bentTriangle(pos, vec3(0.0,0.0,0.3), vec3(0.0,0.0,-0.3), vec3(extent,0.0,0.0), a0)-0.01;
}

float scene_yellow(vec3 pos) {
    float h1 = sdEllipsoid(h1_rotinv*(pos - h1_pos), h1_scale);
    float h2 = sdEllipsoid(h2_rotinv*(pos - h2_pos), h2_scale);
    float h3 = sdEllipsoid(h3_rotinv*(pos - h3_pos), h3_scale);

    return min(h1,min(h2,h3));
}

float scene_green(vec3 pos) {

    float ret = 10000.0;

    for (int i=0; i<10; i++) {
        ret = min(ret, leaf(
            rotateY(head_bulb_rotinv*(pos-head_bulb_pos),i*3.88), // 3.88 = pi*phi
            2.0 + 0.25*sin(i*200.0),
            1.0 + 0.1*sin(i*100.0)
        ));
    }

    return ret;
}

float scene(vec3 pos) {
    return min(
        scene_brown(pos),
        min(scene_yellow(pos), scene_green(pos))
    );
}

float calcIntersection( in vec3 ro, in vec3 rd ) {
    float h = precis*2.0;
    float t = 0.0;
	float res = -1.0;
    for (int i=0; i<150; i++) {
        if( h<precis||t>maxd ) break;
	    h = scene( ro+rd*t );
        t += h;
    }
    return t;
}

vec3 calcNormal( in vec3 pos ) {
    const float eps = 0.002;

    const vec3 v1 = vec3( 1.0,-1.0,-1.0);
    const vec3 v2 = vec3(-1.0,-1.0, 1.0);
    const vec3 v3 = vec3(-1.0, 1.0,-1.0);
    const vec3 v4 = vec3( 1.0, 1.0, 1.0);

	return normalize( v1*scene( pos + v1*eps ) +
					  v2*scene( pos + v2*eps ) +
					  v3*scene( pos + v3*eps ) +
					  v4*scene( pos + v4*eps ) );
}

const vec3 GREEN = vec3(0.0,1.0,0.0);
const vec3 BROWN = vec3(0.5, 0.35, 0.05);
const vec3 YELLOW = vec3(0.984,0.965,0.0);

vec3 scene_mat(vec3 pos) {
    if (scene_brown(pos) < scene_yellow(pos)) {
        if (scene_brown(pos) < scene_green(pos)) {
            return BROWN;
        } else {
            return GREEN;
        }
    } else if (scene_green(pos) < scene_yellow(pos)) {
        return GREEN;
    } else {
        return YELLOW;
    }
}

vec3 calcLight(in vec3 p, in vec3 lightp, in vec3 lightc, in vec3 camdir) {
	vec3 n = calcNormal(p);
    vec3 l = normalize(lightp - p);
    vec3 v = normalize(-p);
    vec3 h = normalize(v + l);

    float n_dot_l = dot(n, l);

    float cosr = max(dot(n, h), 0.0);

    vec3 mat = scene_mat(p);

    vec3 ambiant = vec3(0.2);
    vec3 diffuse = (0.7 * n_dot_l) * mat;
    vec3 specular = (0.1 * pow(cosr, 16.0)) * mat;

    return lightc * (ambiant + diffuse + specular);
}

vec3 illuminate( in vec3 pos , in vec3 camdir ) {
    vec3 l2 = calcLight(pos, vec3(-5.0,-5.0,0.0), vec3(1.0), camdir);
    return l2;
}

mat3 calcLookAtMatrix( in vec3 ro, in vec3 ta, in float roll ) {
    vec3 ww = normalize( ta - ro );
    vec3 uu = normalize( cross(ww,vec3(sin(roll),cos(roll),0.0) ) );
    vec3 vv = normalize( cross(uu,ww));
    return mat3( uu, vv, ww );
}



vec3 gradient(vec3 p) {
    return normalize(vec3(
        scene(vec3(p.x + grad_step, p.y, p.z)) - scene(vec3(p.x - grad_step, p.y, p.z)),
        scene(vec3(p.x, p.y + grad_step, p.z)) - scene(vec3(p.x, p.y - grad_step, p.z)),
        scene(vec3(p.x, p.y, p.z  + grad_step)) - scene(vec3(p.x, p.y, p.z - grad_step))
    ));
}

vec3 comp(vec4 c, vec3 bg) {
    float alpha = c.a;
    return alpha*c.rgb/alpha + (1.0-alpha)*bg;
}

void main() {
    vec3 fragCoord = gl_FragCoord.xyz;
    vec2 xy = (fragCoord.xy - iResolution.xy/2.0) / iResolution.xy.y;

    vec3 ro = vec3(0.0, 0.0,0.0);
    vec3 camtar = vec3(0.0,0.0,-1.0);

    // todo: camMat can probably be cached, or even hardcoded
    mat3 camMat = calcLookAtMatrix( ro, camtar, 0.0 );
	vec3 rd = normalize( camMat * vec3(xy,1.0) );

    vec3 col = vec3(0.0,0.0,0.0);

    float dist = calcIntersection(ro, rd);

    if (dist > maxd - 1.0) {
        col = vec3(0.85); // bgColor - keep in sync with A3::init
    } else {
    	vec3 pos = ro + dist * rd;
        col = illuminate(pos, rd);
    }

	fragColour = vec4(col,1.0);
}
