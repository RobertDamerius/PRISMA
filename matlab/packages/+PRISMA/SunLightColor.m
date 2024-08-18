function lightColor = SunLightColor(eyePosition, sunLightDirection, radiusPlanet, radiusAtmosphere, scaleHeightRayleigh, coefficientRayleigh, sunIntensity)
    %PRISMA.SunLightColor Calculate the light color of a white sun light sources that passes through the atmosphere of a planet.
    % 
    % PARAMETER
    % eyePosition         ... The location of the camera, given as three-dimensional vector.
    % sunLightDirection   ... A three-dimensional vector indicating the light direction, e.g. the vector from the sun to the camera.
    % radiusPlanet        ... The radius of the planet in meters, e.g. 6371000.
    % radiusAtmosphere    ... The radius of the atmosphere in meters, e.g. the sum of the planets radius and the thickness of the atmosphere, for example 6471000.
    % scaleHeightRayleigh ... Scale height for Rayleigh scattering in meters, e.g. 8000.
    % coefficientRayleigh ... Precomputed Rayleigh scattering coefficients for red, green and blue wavelengths in 1/m, e.g. [5.5e-6; 13.0e-6; 22.4e-6].
    % sunIntensity        ... Intensity of the sun for atmospheric scattering, e.g. 20.
    % 
    % RETURN
    % lightColor ... A 3-by-1 vector of type double representing the resulting sun light color.
    assert(isa(eyePosition,'double') && (3 == numel(eyePosition)), 'Input "eyePosition" must be a 3-dimensional vector of type double!');
    assert(isa(sunLightDirection,'double') && (3 == numel(sunLightDirection)), 'Input "sunLightDirection" must be a 3-dimensional vector of type double!');
    assert(isa(radiusPlanet,'double') && isscalar(radiusPlanet), 'Input "radiusPlanet" must be a scalar of type double!');
    assert(isa(radiusAtmosphere,'double') && isscalar(radiusAtmosphere), 'Input "radiusPlanet" must be a scalar of type double!');
    assert(isa(scaleHeightRayleigh,'double') && isscalar(scaleHeightRayleigh), 'Input "radiusPlanet" must be a scalar of type double!');
    assert(isa(coefficientRayleigh,'double') && (3 == numel(coefficientRayleigh)), 'Input "coefficientRayleigh" must be a 3-dimensional vector of type double!');
    assert(isa(sunIntensity,'double') && isscalar(sunIntensity), 'Input "radiusPlanet" must be a scalar of type double!');

    % use correct vector dimension
    directionToSun = -[sunLightDirection(1); sunLightDirection(2); sunLightDirection(3)];
    rayOrigin = [eyePosition(1); eyePosition(2); eyePosition(3)];
    coeffRayleigh = [coefficientRayleigh(1); coefficientRayleigh(2); coefficientRayleigh(3)];

    % find intersection
    intersectAtmosphere = RaySphereIntersection(rayOrigin, directionToSun, radiusAtmosphere);
    lightColor = ones(3,1) * (3.0/(8.0*pi) * sunIntensity);
    if(intersectAtmosphere(2) <= 0.0)
        return;
    end
    intersectBegin = max(0, intersectAtmosphere(1));
    intersectEnd = intersectAtmosphere(2);

    % calculate step size
    numSteps = 20;
    stepSize = (intersectEnd - intersectBegin) / numSteps;

    % ray tracing through atmosphere
    rayTime = 0;
    opticalDepth = 0;
    for k=1:numSteps
        p = rayOrigin + directionToSun * (rayTime + stepSize * 0.5);
        altitude = norm(p) - radiusPlanet;
        opticalDepth = opticalDepth + exp(-altitude / scaleHeightRayleigh) * stepSize;
        rayTime = rayTime + stepSize;
    end
    attenuation = exp(-coeffRayleigh * opticalDepth);
    lightColor = (3.0/(8.0*pi) * sunIntensity) * attenuation;
end

function result = RaySphereIntersection(rayOrigin, viewDirection, radius)
    b = rayOrigin' * viewDirection;
    d = radius * radius + b * b - rayOrigin' * rayOrigin;
    result = [1e5; -1e5];
    if(d >= 0.0)
        ds = sqrt(d);
        result = [-ds - b; ds - b];
    end
end

