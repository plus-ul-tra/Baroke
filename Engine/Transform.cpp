#pragma once
#include "pch.h"
#include "Transform.h"
#include "CoreTypes.h"
using namespace DirectX;
Transform::Transform() :m_position(XMVectorSet(0.0f, 0.0f, 0.0f, 0.1f)),
						m_rotationRad(0.0f),
						m_scale(XMVectorSet(1.0f,1.0f,1.0f,0.0f))
{

}

Transform::Transform(XMVECTOR pos, float rotRad, XMVECTOR scale) 
	: m_position(pos),															  
	  m_rotationRad(rotRad), 
	  m_scale(scale)
{

}

void Transform::Initialize()
{
	// 여기에서 메시지 핸들러를 등록합니다.
	RegisterMessageHandler(mycore::MessageID::Move, std::bind(&Transform::OnMoveMessage, this, std::placeholders::_1));
	RegisterMessageHandler(mycore::MessageID::Rotate, std::bind(&Transform::OnRotateMessage, this, std::placeholders::_1));
	RegisterMessageHandler(mycore::MessageID::Scale, std::bind(&Transform::OnScaleMessage, this, std::placeholders::_1));
}

void Transform::Update(double dt)
{
	// 위치/회전/크기 변경은 주로 Set 함수나 Translate/Rotate/Scale 함수
}

void Transform::OnMoveMessage(void* data)
{
	if (data) {
		// void* 데이터를 DirectX::XMVECTOR* 타입으로 캐스팅하여 사용
		DirectX::XMVECTOR* deltaPos = static_cast<DirectX::XMVECTOR*>(data);
		Translate(*deltaPos);
	}
}

void Transform::OnRotateMessage(void* data)
{
	if (data) {
		// void* 데이터를 double* 타입으로 캐스팅하여 사용 (라디안 각도)
		float* deltaAngleRad = static_cast<float*>(data);
		Rotate(*deltaAngleRad);
	}
}

void Transform::OnScaleMessage(void* data)
{
	if (data) {
		// void* 데이터를 DirectX::XMVECTOR* 타입으로 캐스팅하여 사용
		DirectX::XMVECTOR* deltaScale = static_cast<XMVECTOR*>(data);
		Scale(*deltaScale);
	}
}

void Transform::SetPosition(XMVECTOR pos)
{
	m_position = pos;
	
}

void Transform::SetRotation(float rad)
{
	m_rotationRad = rad;
	// 필요하다면 회전 변경 메시지 발송
}

void Transform::SetScale(DirectX::XMVECTOR scl)
{
	m_scale = scl;
	// 필요하다면 크기 변경 메시지 발송
}



void Transform::Translate(DirectX::XMVECTOR delta)
{
	
	m_position = XMVectorAdd(m_position, delta);
	
}

void Transform::Rotate(float deltaAngleRad)
{
	m_rotationRad += deltaAngleRad;
	
}

void Transform::Scale(XMVECTOR deltaScale)
{
	
	m_scale = XMVectorMultiply(m_scale, deltaScale);
}

XMMATRIX Transform::GetWorldMatrix() const
{ 
	XMMATRIX scaleMatrix = XMMatrixScalingFromVector(m_scale);
	XMMATRIX rotationMatrix = XMMatrixRotationZ(m_rotationRad); 
	XMMATRIX translationMatrix = XMMatrixTranslationFromVector(m_position);

	// 행렬 곱셈 순서: 스케일 -> 회전 -> 이동 (SRT 순서)
	XMMATRIX localMatrix = XMMatrixMultiply(scaleMatrix, rotationMatrix);
	localMatrix = XMMatrixMultiply(localMatrix, translationMatrix);
	if (m_parent)
	{
		return XMMatrixMultiply(localMatrix, m_parent->GetWorldMatrix());
	}
	else
	{
		return localMatrix; // 부모가 없으면 로컬 행렬이 곧 월드 행렬
	}
}