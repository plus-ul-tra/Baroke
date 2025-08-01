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

		comp->SetOwner(this); // SetOwner 호출

		T* ptr = comp.get();

		m_Components.emplace_back(std::move(comp));

		return ptr;
	}

	template<typename T>
	T* GetComponent() const //const Object에서도 호출 가능
	{
		static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

		for (const auto& compPtr : m_Components) { 
			if (T* typedComp = dynamic_cast<T*>(compPtr.get())) {
				return typedComp;
			}
		}
		return nullptr; // 해당 타입의 컴포넌트를 찾지 못함
	}

	virtual void Update(double deltaTime) = 0;

	//필수는 아님
	virtual void FixedUpdate(double deltaTime);
	virtual void LateUpdate(double deltaTime);

	virtual void Render(Renderer& renderer) {};

	void _SendMessage(const mycore::MessageID msg, void* data = nullptr);

	void SendEvent(const std::string& ev);
};