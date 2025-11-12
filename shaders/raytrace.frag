#version 410 core
out vec4 FragColor;

uniform vec2 uResolution;
uniform float uTime;
uniform vec3 uCameraPos;
uniform vec3 uCameraTarget;

#define NUM_SPHERES 3

struct Sphere {
    vec3 center;
    float radius;
    vec3 color;
    float reflect; // 0..1
};

// Hardcoded scene (you can edit)
Sphere spheres[NUM_SPHERES];

void initScene() {
    spheres[0].center = vec3( 0.0,  0.0, -3.5); spheres[0].radius = 0.8; spheres[0].color = vec3(1.0,0.45,0.3); spheres[0].reflect = 0.2;
    spheres[1].center = vec3(-1.4, -0.2, -4.0); spheres[1].radius = 0.6; spheres[1].color = vec3(0.2,0.8,0.4); spheres[1].reflect = 0.1;
    spheres[2].center = vec3( 1.2, -0.4, -4.2); spheres[2].radius = 0.7; spheres[2].color = vec3(0.35,0.5,1.0); spheres[2].reflect = 0.3;
}

float intersectSphere(vec3 ro, vec3 rd, Sphere s) {
    vec3 oc = ro - s.center;
    float b = dot(oc, rd);
    float c = dot(oc, oc) - s.radius * s.radius;
    float disc = b*b - c;
    if (disc < 0.0) return -1.0;
    float sqrtD = sqrt(disc);
    float t0 = -b - sqrtD;
    float t1 = -b + sqrtD;
    if (t0 > 0.0001) return t0;
    if (t1 > 0.0001) return t1;
    return -1.0;
}

vec3 reflectVec(vec3 I, vec3 N) {
    return I - 2.0 * dot(I, N) * N;
}

vec3 shade(vec3 ro, vec3 rd) {
    // Simple raymarch-ish: find nearest sphere
    float tMin = 1e20;
    int hitIndex = -1;
    for (int i = 0; i < NUM_SPHERES; ++i) {
        float t = intersectSphere(ro, rd, spheres[i]);
        if (t > 0.0 && t < tMin) { tMin = t; hitIndex = i; }
    }
    if (hitIndex == -1) {
        // background gradient
        float t = 0.5 * (rd.y + 1.0);
        return mix(vec3(0.1,0.12,0.18), vec3(0.5,0.6,0.8), t);
    }

    // compute hit info
    vec3 hitPos = ro + tMin * rd;
    vec3 N = normalize(hitPos - spheres[hitIndex].center);

    // Simple directional light
    vec3 lightDir = normalize(vec3(-0.6, 0.8, 0.5));
    // Shadow test: cast ray from hitPos to light
    vec3 shadowOrigin = hitPos + N * 0.001;
    bool inShadow = false;
    for (int i = 0; i < NUM_SPHERES; ++i) {
        if (i == hitIndex) continue;
        float t = intersectSphere(shadowOrigin, lightDir, spheres[i]);
        if (t > 0.0) { inShadow = true; break; }
    }

    float diff = max(dot(N, lightDir), 0.0);
    vec3 baseColor = spheres[hitIndex].color;
    vec3 col = baseColor * (inShadow ? 0.15 : (0.1 + 0.9 * diff));

    // simple specular (Blinn)
    vec3 viewDir = normalize(uCameraPos - hitPos);
    vec3 halfv = normalize(lightDir + viewDir);
    float spec = pow(max(dot(N, halfv), 0.0), 32.0);
    col += vec3(1.0) * spec * 0.25;

    // reflection (single bounce)
    float refl = spheres[hitIndex].reflect;
    if (refl > 0.01) {
        vec3 R = reflectVec(rd, N);
        vec3 reflCol = vec3(0.0);
        // simple secondary intersection test
        float t2min = 1e20;
        int hit2 = -1;
        vec3 rOrigin = hitPos + N * 0.001;
        for (int i = 0; i < NUM_SPHERES; ++i) {
            float t2 = intersectSphere(rOrigin, R, spheres[i]);
            if (t2 > 0.0 && t2 < t2min) { t2min = t2; hit2 = i; }
        }
        if (hit2 != -1) {
            vec3 hit2Pos = rOrigin + t2min * R;
            vec3 N2 = normalize(hit2Pos - spheres[hit2].center);
            float diff2 = max(dot(N2, lightDir), 0.0);
            reflCol = spheres[hit2].color * (0.1 + 0.9 * diff2);
        } else {
            reflCol = vec3(0.05, 0.07, 0.1); // faint env
        }
        col = mix(col, reflCol, refl);
    }

    return col;
}

vec3 getRayDir(vec2 fragCoord, vec2 resolution, vec3 camPos, vec3 camTarget) {
    // Camera basis
    vec3 forward = normalize(camTarget - camPos);
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(forward, up));
    up = normalize(cross(right, forward));

    float fov = radians(45.0);
    float aspect = resolution.x / resolution.y;
    vec2 ndc = (fragCoord / resolution) * 2.0 - 1.0;
    ndc.x *= aspect;
    float px = ndc.x * tan(fov * 0.5);
    float py = -ndc.y * tan(fov * 0.5);

    vec3 rd = normalize(px * right + py * up + forward);
    return rd;
}

void main() {
    initScene();

    vec2 fragCoord = gl_FragCoord.xy;
    vec2 res = uResolution;

    vec3 camPos = uCameraPos;
    vec3 camTarget = uCameraTarget;

    vec3 rd = getRayDir(fragCoord, res, camPos, camTarget);
    vec3 col = shade(camPos, rd);

    // gamma
    col = pow(col, vec3(1.0 / 2.2));
    FragColor = vec4(col, 1.0);
}
