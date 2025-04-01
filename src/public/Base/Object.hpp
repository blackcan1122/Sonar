#pragma once
#include <typeindex>


class IObject
{
public:
	virtual std::type_index GetStaticClass() const { return typeid(*this); };
	virtual void Tick(float DeltaTime) = 0;

};

class Object : public IObject
{

};