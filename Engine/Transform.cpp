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
	// ���⿡�� �޽��� �ڵ鷯�� ����մϴ�.
	RegisterMessageHandler(mycore::MessageID::Move, std::bind(&Transform::OnMoveMessage, this, std::placeholders::_1));
	RegisterMessageHandler(mycore::MessageID::Rotate, std::bind(&Transform::OnRotateMessage, this, std::placeholders::_1));
	RegisterMessageHandler(mycore::MessageID::Scale, std::bind(&Transform::OnScaleMessage, this, std::placeholders::_1));
}

void Transform::Update(double dt)
{
	// ��ġ/ȸ��/ũ�� ������ �ַ� Set �Լ��� Translate/Rotate/Scale �Լ�
}

void Transform::OnMoveMessage(void* data)
{
	if (data) {
		// void* �����͸� DirectX::XMVECTOR* Ÿ������ ĳ�����Ͽ� ���
		DirectX::XMVECTOR* deltaPos = static_cast<DirectX::XMVECTOR*>(data);
		Translate(*deltaPos);
	}
}

void Transform::OnRotateMessage(void* data)
{
	if (data) {
		// void* �����͸� double* Ÿ������ ĳ�����Ͽ� ��� (���� ����)
		float* deltaAngleRad = static_cast<float*>(data);
		Rotate(*deltaAngleRad);
	}
}

void Transform::OnScaleMessage(void* data)
{
	if (data) {
		// void* �����͸� DirectX::XMVECTOR* Ÿ������ ĳ�����Ͽ� ���
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
	// �ʿ��ϴٸ� ȸ�� ���� �޽��� �߼�
}

void Transform::SetScale(DirectX::XMVECTOR scl)
{
	m_scale = scl;
	// �ʿ��ϴٸ� ũ�� ���� �޽��� �߼�
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

	// ��� ���� ����: ������ -> ȸ�� -> �̵� (SRT ����)
	XMMATRIX localMatrix = XMMatrixMultiply(scaleMatrix, rotationMatrix);
	localMatrix = XMMatrixMultiply(localMatrix, translationMatrix);
	if (m_parent)
	{
		return XMMatrixMultiply(localMatrix, m_parent->GetWorldMatrix());
	}
	else
	{
		return localMatrix; // �θ� ������ ���� ����� �� ���� ���
	}
}