#pragma once
#include "pch.h"
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include "CoreTypes.h"

class Object;

class Component {
private:

public:
	virtual ~Component() = default;

	virtual void Update(double deltaTime) = 0;
	virtual void FixedUpdate(double deltaTime) {};
	virtual void LateUpdate(double deltaTime) {};

	virtual void OnEvent(const std::string& ev)=0;

	void HandleMessage(mycore::MessageID msg, void* data)
	{
		auto it = m_MessageHandlers.find(msg);
		if (it != m_MessageHandlers.end())
		{
			for (auto& handler : it->second)
				handler(data);
		}
	}

	using HandlerType = std::function<void(void*)>;

	void RegisterMessageHandler(mycore::MessageID msg, HandlerType handler)
	{
		m_MessageHandlers[msg].push_back(std::move(handler));
	}

	virtual void SetOwner(Object* owner) { m_owner = owner; }


protected:
	Object* m_owner = nullptr;
	std::unordered_map<mycore::MessageID, std::vector<HandlerType>> m_MessageHandlers;

};