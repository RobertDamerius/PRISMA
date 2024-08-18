function quaternion = LookAtToQuaternion(eyePosition, lookAtPosition)
    %PRISMA.LookAtToQuaternion Calculate the unit quaternion for a non-rolling camera based on the eye position and the look-at position.
    % 
    % PARAMETER
    % eyePosition    ... A three-dimensional vector representing the eye position from where to look, e.g. the camera position.
    % lookAtPosition ... The three-dimensional position to which the camera is looking at.
    % 
    % RETURN
    % quaternion   ... A unit quaternion representing the orientation of the non-rolling camera, given as [qw; qx; qy; qz].
    assert(isa(eyePosition,'double') && (3 == numel(eyePosition)), 'Input "eyePosition" must be a 3-dimensional vector of type double!');
    assert(isa(lookAtPosition,'double') && (3 == numel(lookAtPosition)), 'Input "lookAtPosition" must be a 3-dimensional vector of type double!');

    % calculate view direction (x axis)
    xAxis = [lookAtPosition(1) - eyePosition(1); lookAtPosition(2) - eyePosition(2); lookAtPosition(3) - eyePosition(3)];
    L = norm(xAxis);
    if(L > 100*eps)
        xAxis = xAxis / L;
    else
        xAxis = [1;0;0];
    end

    % calculate y axis
    yAxis = cross([0;0;1], xAxis);
    L = norm(yAxis);
    if(L > 100*eps)
        yAxis = yAxis / L;
    else
        yAxis = [0;1;0];
    end

    % calculate z axis and normalize
    zAxis = cross(xAxis, yAxis);
    zAxis = zAxis / norm(zAxis);

    % create transformation matrix and convert to quaternion
    Rb2n = [xAxis, yAxis, zAxis];
    q = rotm2quat(Rb2n);
    quaternion = [q(1); q(2); q(3); q(4)];
end
