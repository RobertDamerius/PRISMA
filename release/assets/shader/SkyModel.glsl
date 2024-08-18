/**
 * @brief Calculate the intersection of a ray with a sphere.
 * @param[in] p Position of the ray origin relative to the center of the sphere.
 * @param[in] dir The normalized direction vector of the ray.
 * @param[in] r Radius of the sphere.
 * @return Two ray length values (L1, L2) where the ray "p + L*dir" intersects with the sphere.
 * If an intersection point exists L2 >= L1. If no intersection exists, L1 > L2.
 */
vec2 RaySphereIntersection(vec3 p, vec3 dir, float r){
    float b = dot(p, dir);
    float d = r * r + b * b - dot(p, p);
    if(d < 0.0){
        return vec2(10000.0, -10000.0);
    }
    float ds = sqrt(d);
    return vec2(-ds - b, ds - b);
}


/**
 * @brief Calculate the color for atmospheric scattering.
 * @param[out] sunVisibility Scalar value indicating whether the sun is visible (1.0) or occluded by the planet (0.0).
 * @param[in] viewDirectionWorldSpace Normalized view direction of the camera given in world-space.
 * @param[in] eyePosition World-space position relative to the center of the planet from where to start the ray-tracing for atmospheric scattering, e.g. vec3(0, 6372000, 0).
 * @param[in] directionToSun Normalized direction vector from the eyePosition to the sun given in world-space.
 * @param[in] sunIntensity Intensity of the sun, e.g. 20.
 * @param[in] radiusPlanet Radius of the planet in meters, e.g. 6371000.
 * @param[in] radiusAtmosphere Radius of the atmosphere in meters, e.g. 6471000.
 * @param[in] coefficientRayleigh Precomputed Rayleigh scattering coefficients for red, green and blue wavelengths in 1/m, e.g. vec3(5.5e-6, 13.0e-6, 22.4e-6).
 * @param[in] coefficientMie Precomputed Mie scattering coefficient in 1/m, e.g. 21e-6.
 * @param[in] scaleHeightRayleigh Scale height for Rayleigh scattering in meters, e.g. 8000.
 * @param[in] scaleHeightMie Scale height for Mie scattering in meters, e.g. 1200.
 * @param[in] scatterAmountMie Amount of Mie scattering, usually in range (-1,0,1), e.g. 0.758.
 * @return Atmospheric scattering color.
 */
vec3 AtmosphericScattering(out float sunVisibility, vec3 viewDirectionWorldSpace, vec3 eyePosition, vec3 directionToSun, float sunIntensity, float radiusPlanet, float radiusAtmosphere, vec3 coefficientRayleigh, float coefficientMie, float scaleHeightRayleigh, float scaleHeightMie, float scatterAmountMie){
    // find primary ray intersection points
    float radiusOrigin = length(eyePosition);
    vec2 intersectAtmosphere = RaySphereIntersection(eyePosition, viewDirectionWorldSpace, radiusAtmosphere);
    vec2 intersectPlanet = RaySphereIntersection(eyePosition, viewDirectionWorldSpace, radiusPlanet);
    float intersectBegin = 0.0f;
    float intersectEnd = 0.0f;
    sunVisibility = 1.0f;
    if(radiusOrigin >= radiusAtmosphere){ // camera located in space
        if(intersectAtmosphere.y < 0.0f){ // ray shots into space
            return vec3(0.0f);
        }
        intersectBegin = intersectAtmosphere.x;
        intersectEnd = intersectAtmosphere.y;

        // test if ray ends at the planets surface
        if((intersectPlanet.x >= 0.0f) && (intersectPlanet.x <= intersectPlanet.y)){
            intersectEnd = intersectPlanet.x;
            sunVisibility = 0.0f;
        }
    }
    else if(radiusOrigin <= radiusPlanet){ // camera located inside planet sphere
        sunVisibility = 0.0f;
        return vec3(0.0f);
    }
    else{ // camera located in atmosphere
        intersectEnd = intersectAtmosphere.y;

        // test if ray ends at the planets surface
        if((intersectPlanet.x >= 0.0f) && (intersectPlanet.x <= intersectPlanet.y)){
            intersectEnd = intersectPlanet.x;
            sunVisibility = 0.0f;
        }
    }

    // calculate step size for primary ray
    float iStepSize = (intersectEnd - intersectBegin) / float(numRayTracingSteps.x);

    // Rayleigh and Mie phases
    const float PI = 3.14159265358979323f;
    float mu = dot(viewDirectionWorldSpace, directionToSun);
    float mumu = mu * mu;
    float gg = scatterAmountMie * scatterAmountMie;
    float phaseRayleigh = 3.0 / (16.0 * PI) * (1.0 + mumu);
    float phaseMie = 3.0 / (8.0 * PI) * ((1.0 - gg) * (mumu + 1.0)) / (pow(1.0 + gg - 2.0 * mu * scatterAmountMie, 1.5) * (2.0 + gg));

    // sample the primary ray
    float iTime = 0.0; // primary ray time
    vec3 totalRlh = vec3(0,0,0); // accumulator for Rayleigh scattering
    vec3 totalMie = vec3(0,0,0); // accumulator for Mie scattering
    float iOdRlh = 0.0; // optical depth accumulators of primary ray for Rayleight scattering
    float iOdMie = 0.0; // optical depth accumulators of primary ray for Mie scattering
    for(uint i = 0; i < numRayTracingSteps.x; ++i){
        // primary ray sample position and its height above the planets surface
        vec3 iPos = eyePosition + viewDirectionWorldSpace * (iTime + iStepSize * 0.5);
        float iHeight = length(iPos) - radiusPlanet;

        // current optical depth of Rayleigh and Mie scattering
        float odStepRlh = exp(-iHeight / scaleHeightRayleigh) * iStepSize;
        float odStepMie = exp(-iHeight / scaleHeightMie) * iStepSize;

        // accumulate optical depth for primary ray
        iOdRlh += odStepRlh;
        iOdMie += odStepMie;

        // step size of secondary ray
        float jStepSize = RaySphereIntersection(iPos, directionToSun, radiusAtmosphere).y / float(numRayTracingSteps.y);

        // sample secondary ray
        float jTime = 0.0; // secondary ray time
        float jOdRlh = 0.0; // optical depth accumulators of secondary ray for Rayleight scattering
        float jOdMie = 0.0; // optical depth accumulators of secondary ray for Mie scattering
        for(int j = 0; j < numRayTracingSteps.y; ++j){
            // secondary ray sample position and its height above the planets surface
            vec3 jPos = iPos + directionToSun * (jTime + jStepSize * 0.5);
            float jHeight = length(jPos) - radiusPlanet;

            // accumulate optical depth for secondary ray
            jOdRlh += exp(-jHeight / scaleHeightRayleigh) * jStepSize;
            jOdMie += exp(-jHeight / scaleHeightMie) * jStepSize;

            // increment the secondary ray time
            jTime += jStepSize;
        }

        // calculate attenuation
        vec3 attenuation = exp(-(coefficientMie * (iOdMie + jOdMie) + coefficientRayleigh * (iOdRlh + jOdRlh)));

        // accumulate total scattering
        totalRlh += odStepRlh * attenuation;
        totalMie += odStepMie * attenuation;

        // increment the primary ray time
        iTime += iStepSize;
    }

    // final atmospheric scattering
    return sunIntensity * (phaseRayleigh * coefficientRayleigh * totalRlh + phaseMie * coefficientMie * totalMie);
}


/**
 * @brief Calculate the final sky color based on atmospheric scattering.
 * @param[in] viewDirectionWorldSpace The normalized view direction from the camera to the fragment, given in world space.
 * @param[in] parameter A structure containing the atmosphere parameters.
 * @return The resulting sky color.
 */
vec3 SkyModel(vec3 viewDirectionWorldSpace, AtmosphereParameter parameter){
    #ifdef PRISMA_CONFIGURATION_ENABLE_ATMOSPHERIC_SCATTERING
    float sunVisibility;
    vec3 atmosphereColor = AtmosphericScattering(
        sunVisibility, // output
        viewDirectionWorldSpace,
        parameter.eyePosition,
        -parameter.sunLightDirectionWorldSpace,
        parameter.sunIntensity,
        parameter.radiusPlanet,
        parameter.radiusAtmosphere,
        parameter.coefficientRayleigh,
        parameter.coefficientMie,
        parameter.scaleHeightRayleigh,
        parameter.scaleHeightMie,
        parameter.scatterAmountMie
    );
    atmosphereColor = vec3(1.0f) - exp(-parameter.atmosphereExposure * atmosphereColor);
    #else
    float sunVisibility = 1.0f;
    vec3 atmosphereColor = vec3(0.0f);
    #endif

    float sunDisc = sunVisibility * smoothstep(parameter.cosSunDiscOuterCutOff, parameter.cosSunDiscInnerCutOff, dot(viewDirectionWorldSpace, -parameter.sunLightDirectionWorldSpace));
    vec3 skyColor = mix(atmosphereColor, parameter.skyColorMix.rgb, parameter.skyColorMix.a);
    vec3 sunColor = parameter.sunColor * parameter.sunDiscIntensity * sunDisc;
    return skyColor + sunColor;
}

