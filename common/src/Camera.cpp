#include "Camera.h"
#include <cmath>

using namespace DirectX;

Camera::Camera() : mEye(), mTarget(), mUp(), mButtonType(-1)
{
    mMtxView = XMMatrixIdentity();
    mMtxProj = XMMatrixIdentity();
}

void Camera::SetLookAt(XMFLOAT3 vPos, XMFLOAT3 vTarget, XMFLOAT3 vUp)
{
    mEye = XMLoadFloat3(&vPos);
    mTarget = XMLoadFloat3(&vTarget);
    mUp = XMLoadFloat3(&vUp);
    mMtxView = XMMatrixLookAtRH(mEye, mTarget, mUp);
}

void Camera::SetPerspective(f32 fovY, f32 aspect, f32 znear, f32 zfar)
{
    mMtxProj = XMMatrixPerspectiveFovRH(fovY, aspect, znear, zfar);
}

void Camera::OnMouseButtonDown(s32 buttonType, f32, f32 )
{
    mButtonType = buttonType;
}

void Camera::OnMouseMove(f32 dx, f32 dy) 
{
    if (mButtonType < 0)
    {
        return;
    }
    if (mButtonType == 0)
    {
        TrackingRotation(dx, dy);
    }
    if (mButtonType == 1)
    {
        ForwardBackward(dy);
    }
}
void Camera::OnMouseButtonUp()
{
    mButtonType = -1;
}

void Camera::OnMouseWheel(f32 rotate)
{
    SwingRightLeft(rotate);
}

void Camera::TrackingRotation(f32 dx, f32 dy)
{
    auto EyeVec = mEye - mTarget;
    auto EyeVecLength = XMVectorGetX(XMVector3Length(EyeVec));
    auto NormalizedEyeVec = XMVector3Normalize(EyeVec);

    auto phi = std::atan2(XMVectorGetX(NormalizedEyeVec), XMVectorGetZ(NormalizedEyeVec));//Azimuth
    auto theta = std::acos(XMVectorGetY(NormalizedEyeVec));//Elevation Angle
        
    auto x = (XM_PI + phi) / XM_2PI;
    auto y = theta / XM_PI;

    x += dx;
    y = std::fmax(0.01f, std::fmin(y - dy, 0.99f));

    phi = x * XM_2PI;
    theta = y * XM_PI;

    auto NewEyeVec = XMVector3Normalize(XMVectorSet(-sin(theta) * sin(phi), cos(theta), -sin(theta) * cos(phi), 0.0f));
    NewEyeVec *= EyeVecLength;
    mEye = mTarget + NewEyeVec;
    mMtxView = XMMatrixLookAtRH(mEye, mTarget, mUp);
}

void Camera::ForwardBackward(f32 d)
{
    auto EyeVec = mEye - mTarget;
    auto toTargetLength = XMVectorGetX(XMVector3Length(EyeVec));
    if (toTargetLength < FLT_EPSILON) {
        return;
    }
    EyeVec = XMVector3Normalize(-EyeVec);//reverse

    mEye += EyeVec * (toTargetLength * d);

    mMtxView = XMMatrixLookAtRH(mEye, mTarget, mUp);
}

void Camera::SwingRightLeft(f32 rotate)
{
    mTarget += rotate * XMVector3Cross(mEye - mTarget, mUp);
    TrackingRotation(0, 0);
}
