#pragma once
#include <typeindex>

// MACROS


#define AUTOBODY(cls) \
public: \
    static std::type_index StaticClass() { return typeid(cls); };

// Macro for intermediate CRTP base classes
#define DECLARE_CRTP_INTERMEDIATE_CLASS(ClassName, BaseName) \
template <typename Derived> \
class ClassName : public BaseName<Derived>

// Macro for final leaf classes
#define DECLARE_CRTP_LEAF_CLASS(ClassName, BaseName) \
class ClassName : public BaseName<ClassName>

#define DECLARE_CLASS(ClassName, BaseClass) \
class ClassName : public BaseClass \
{\
public:\
AUTOBODY(ClassName)

#define END_CLASS };


class IObject
{
public:
	virtual std::type_index GetStaticClass() const { return typeid(*this); };

};

class Object : public IObject
{

};