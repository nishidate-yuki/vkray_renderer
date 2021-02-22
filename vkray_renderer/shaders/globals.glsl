struct HitPayload
{
    //uint  seed;
    bool hitted;
    vec3 contribution;  // Hit value
    vec3 throughput;  // Hit value
    vec3 hitPosition;
    vec3 hitNormal;
    bool done;
    //vec3  weight;        // weight of the contribution
    //vec3  rayOrigin;
    //vec3  rayDirection;
    //float last_pdf;
    //uint  flags;
};

const highp float M_PI = 3.14159265358979323846;
