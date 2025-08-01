#pragma once
#include <memory>
#include"Component.h"
#include "Renderer.h"

class Component;

class Object {
	
private:
	std::vector<std::unique_ptr<Component>> m_Components;

public:
	template<typename T, typename... Args>
	T* AddComponent(Args&&... args)
	{
		static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

		auto comp = std::make_unique<T>(std::forward<Args>(args)...);

		comp->SetOwner(this); // SetOwner ȣ��

		T* ptr = comp.get();

		m_Components.emplace_back(std::move(comp));

		return ptr;
	}

	template<typename T>
	T* GetComponent() const //const Object������ ȣ�� ����
	{
		static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

		for (const auto& compPtr : m_Components) { 
			if (T* typedComp = dynamic_cast<T*>(compPtr.get())) {
				return typedComp;
			}
		}
		return nullptr; // �ش� Ÿ���� ������Ʈ�� ã�� ����
	}

	virtual void Update(double deltaTime) = 0;

	//�ʼ��� �ƴ�
	virtual void FixedUpdate(double deltaTime);
	virtual void LateUpdate(double deltaTime);

	virtual void Render(Renderer& renderer) {};

	void _SendMessage(const mycore::MessageID msg, void* data = nullptr);

	void SendEvent(const std::string& ev);
};