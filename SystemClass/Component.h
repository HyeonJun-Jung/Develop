#pragma once
class Component
{
public:
	virtual ~Component() {};
	virtual bool Update() = 0;
};

