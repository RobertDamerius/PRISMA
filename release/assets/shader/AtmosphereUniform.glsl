// uniform block for atmosphere parameters
layout (std140, binding = $UBO_ATMOSPHERE$) uniform UBOAtmosphereParameter {
    vec4 ubo_atmosphere_pxyz_k;        // sunLightDirectionWorldSpace (x,y,z), sunDiscIntensity
    vec4 ubo_atmosphere_dxyz_cr;       // sunLightDirection (x,y,z), sunColor (r)
    vec4 ubo_atmosphere_cgb_xrg;       // sunColor (g,b), skyColorMix (r,g)
    vec4 ubo_atmosphere_xba_exy;       // skyColorMix (b,a), eyePosition (x,y)
    vec4 ubo_atmosphere_ez_i_rp_ra;    // eyePosition (z), sunIntensity, radiusPlanet, radiusAtmosphere
    vec4 ubo_atmosphere_rrgb_m;        // coefficientRayleigh(x,y,z), coefficientMie
    vec4 ubo_atmosphere_hr_hm_g_e;     // scaleHeightRayleigh, scaleHeightMie, scatterAmountMie, atmosphereExposure
    vec4 ubo_atmosphere_ci_co_fm_fr;   // cosSunDiscInnerCutOff, cosSunDiscOuterCutOff, fogDepthBegin, fogDepthEnd
    vec4 ubo_atmosphere_fxyz_bb;       // fogDensityBegin, fogDensityEnd, depthFogColor(r,g)
    vec4 ubo_atmosphere_br;            // depthFogColor(b), depthBlendOutDistanceMin, depthBlendOutDistanceRange, 0
};


// structure definition for atmosphere parameters
struct AtmosphereParameter {
    vec3 sunLightDirectionWorldSpace;   // Sun light direction (normalized) in world-space starting from the sun.
    float sunDiscIntensity;             // Intensity of the sun disk, e.g. 42.
    vec3 sunLightDirection;             // Sun light direction (normalized) in view-space starting from the sun.
    vec3 sunColor;                      // Sun light color.
    vec4 skyColorMix;                   // Color to be mixed with the atmospheric scattering based sky color.
    vec3 eyePosition;                   // World-space position relative to the center of the planet from where to start the ray-tracing for atmospheric scattering, e.g. vec3(0, 6372000, 0).
    float sunIntensity;                 // Intensity of the sun for atmospheric scattering, e.g. 20.
    float radiusPlanet;                 // Radius of the planet in meters, e.g. 6371000.
    float radiusAtmosphere;             // Radius of the atmosphere in meters, e.g. 6471000.
    vec3 coefficientRayleigh;           // Precomputed Rayleigh scattering coefficients for red, green and blue wavelengths in 1/m, e.g. vec3(5.5e-6, 13.0e-6, 22.4e-6).
    float coefficientMie;               // Precomputed Mie scattering coefficient in 1/m, e.g. 21e-6.
    float scaleHeightRayleigh;          // Scale height for Rayleigh scattering in meters, e.g. 8000.
    float scaleHeightMie;               // Scale height for Mie scattering in meters, e.g. 1200.
    float scatterAmountMie;             // Amount of Mie scattering, usually in range (-1,0,1), e.g. 0.758.
    float atmosphereExposure;           // Atmosphere exposure value, e.g. 1.
    float cosSunDiscInnerCutOff;        // cos(a1) where a1 is the inner angle where the smooth cut-off of the sun disc begins, e.g. 0.999995670984418.
    float cosSunDiscOuterCutOff;        // cos(a2) where a2 is the outer angle where the smooth cut-off of the sun disc ends, e.g. 0.999989165999595.
    float fogDepthBegin;                // Depth, where the fog begins.
    float fogDepthEnd;                  // Depth, where the fog ends.
    float fogDensityBegin;              // Density of the fog at the beginning.
    float fogDensityEnd;                // Density of the fog at the end.
    vec3 depthFogColor;                 // Color of the depth fog.
    float depthBlendOutDistanceMin;     // Minimum distance of the scene blend out, e.g. the depth where the scene blend out begins.
    float depthBlendOutDistanceRange;   // Range of the scene blend out distance.
};


uniform uvec2 numRayTracingSteps;   // number of steps for primary and secondary ray tracing for atmospheric scattering


/**
 * @brief Get the atmosphere parameters from the @ref UBOAtmosphereParameter uniform block.
 * @return A structure containing the atmosphere parameters.
 */
AtmosphereParameter GetAtmosphereParameter(void){
    AtmosphereParameter result;
    result.sunLightDirectionWorldSpace = ubo_atmosphere_pxyz_k.xyz;
    result.sunDiscIntensity = ubo_atmosphere_pxyz_k.w;
    result.sunLightDirection = ubo_atmosphere_dxyz_cr.xyz;
    result.sunColor = vec3(ubo_atmosphere_dxyz_cr.w, ubo_atmosphere_cgb_xrg.xy);
    result.skyColorMix = vec4(ubo_atmosphere_cgb_xrg.zw, ubo_atmosphere_xba_exy.xy);
    result.eyePosition = vec3(ubo_atmosphere_xba_exy.zw, ubo_atmosphere_ez_i_rp_ra.x);
    result.sunIntensity = ubo_atmosphere_ez_i_rp_ra.y;
    result.radiusPlanet = ubo_atmosphere_ez_i_rp_ra.z;
    result.radiusAtmosphere = ubo_atmosphere_ez_i_rp_ra.w;
    result.coefficientRayleigh = ubo_atmosphere_rrgb_m.xyz;
    result.coefficientMie = ubo_atmosphere_rrgb_m.w;
    result.scaleHeightRayleigh = ubo_atmosphere_hr_hm_g_e.x;
    result.scaleHeightMie = ubo_atmosphere_hr_hm_g_e.y;
    result.scatterAmountMie = ubo_atmosphere_hr_hm_g_e.z;
    result.atmosphereExposure = ubo_atmosphere_hr_hm_g_e.w;
    result.cosSunDiscInnerCutOff = ubo_atmosphere_ci_co_fm_fr.x;
    result.cosSunDiscOuterCutOff = ubo_atmosphere_ci_co_fm_fr.y;
    result.fogDepthBegin = ubo_atmosphere_ci_co_fm_fr.z;
    result.fogDepthEnd = ubo_atmosphere_ci_co_fm_fr.w;
    result.fogDensityBegin = ubo_atmosphere_fxyz_bb.x;
    result.fogDensityEnd = ubo_atmosphere_fxyz_bb.y;
    result.depthFogColor = vec3(ubo_atmosphere_fxyz_bb.zw, ubo_atmosphere_br.x);
    result.depthBlendOutDistanceMin = ubo_atmosphere_br.y;
    result.depthBlendOutDistanceRange = ubo_atmosphere_br.z;
    return result;
}

