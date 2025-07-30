#pragma once

class Joker;

class JokerFunctionRegistry
{
public:
	static JokerFunctionRegistry& Get()
	{
		static JokerFunctionRegistry instance;
		return instance;
	}

	void Register(const std::string& name, std::function<void(Joker&)> func)
	{
		m_functions[name] = func;
	}

	void Invoke(const std::string& name, Joker& joker)
	{
		if (auto it = m_functions.find(name); it != m_functions.end())
			it->second(joker);
		else
			std::cerr << "[Joker] 기능 없음: " << name << std::endl;
	}

private:
	std::unordered_map<std::string, std::function<void(Joker&)>> m_functions;
};
