#pragma once
#include <cstdint>
#include <iostream>
#include <memory>

#include "Base/SClass.hpp"

#define AUTOBODY_IMPL(Base, Parent, EnableReflection) \
private: \
	using ThisClass = Base; \
public: \
	using Super = Parent; \
	virtual SClass* GetStaticClass() override { return StaticClass(); } \
	static SClass* StaticClass() { \
		static SClass instance(Parent::StaticClass(), #Base); \
		if constexpr (EnableReflection) \
		{ \
			ThisClass::RegisterReflection(); \
		} \
		return &instance; \
	} \
	static void RegisterReflection(); \
private:

#define AUTOBODY(Base, Parent) \
	AUTOBODY_IMPL(Base, Parent, false)

#define AUTOBODY_REFLECT(Base, Parent) \
	AUTOBODY_IMPL(Base, Parent, true)

#define ROOTBODY(Base) \
private: \
	using ThisClass = Base; \
public: \
	virtual SClass* GetStaticClass() { return StaticClass(); } \
	static SClass* StaticClass() { \
		static SClass instance(nullptr, #Base); \
		return &instance; \
	} \
private:

template<typename ClassType, typename MemberType>
struct PropertyRegistrar
{
	PropertyRegistrar(SClass* Class, const char* Name, MemberType ClassType::* MemberPtr)
	{
		const ClassType* dummy = nullptr;
		size_t offset = reinterpret_cast<size_t>(&(dummy->*MemberPtr));

		std::cout << "Registering: " << Name
			<< " offset: " << offset
			<< " sizeof(std::string): " << sizeof(std::string)
			<< " sizeof(ClassType): " << sizeof(ClassType)
			<< std::endl;

		Class->RegisterProperty(std::make_unique<SProperty<ClassType, MemberType>>(Name, offset));
	}
};

#define EXPOSE_FLOAT(Member) \
	static inline PropertyRegistrar<ThisClass, float> _reg_##Member{ThisClass::StaticClass(), #Member, &ThisClass::Member}

#define EXPOSE_INT(Member) \
	static inline PropertyRegistrar<ThisClass, int> _reg_##Member{ThisClass::StaticClass(), #Member, &ThisClass::Member}

#define EXPOSE_BOOL(Member) \
	static inline PropertyRegistrar<ThisClass, bool> _reg_##Member{ThisClass::StaticClass(), #Member, &ThisClass::Member}

#define EXPOSE_STRING(Member) \
	static inline PropertyRegistrar<ThisClass, std::string> _reg_##Member{ThisClass::StaticClass(), #Member, &ThisClass::Member}

#define EXPOSE_PROPERTY(Type, Member) \
	static inline PropertyRegistrar<ThisClass, Type> _reg_##Member{ThisClass::StaticClass(), #Member, &ThisClass::Member}

#define BEGIN_REFLECTION(Class) \
	void Class::RegisterReflection() { \
		using _Class = Class; \
		using _Concrete = Class; \
		static bool done = false; \
		if (done) return; \
			done = true; \
			_Concrete _concrete; \
			_Class* _tmp = static_cast<_Class*>(&_concrete); \
			uintptr_t _base = reinterpret_cast<uintptr_t>(_tmp);

#define REFLECT(Member, Type) \
		_Class::StaticClass()->RegisterProperty( \
			std::make_unique<SProperty<_Class, Type>>( \
				#Member, \
				static_cast<size_t>(reinterpret_cast<uintptr_t>(&_tmp->Member) - _base) \
			));

#define END_REFLECTION }

#define DECLARE_CRTP_INTERMEDIATE_CLASS(ClassName, BaseName) \
template <typename Derived> \
class ClassName : public BaseName<Derived>

#define DECLARE_CRTP_LEAF_CLASS(ClassName, BaseName) \
class ClassName : public BaseName<ClassName>

#define DECLARE_CLASS(ClassName, BaseClass) \
class ClassName : public BaseClass \
{ \
public: \
AUTOBODY(ClassName, BaseClass)

#define END_CLASS };

#if defined(_WIN32) && defined(SONARENGINE_SHARED)
#if defined(SONARENGINE_BUILD)
#define SONARENGINE_API __declspec(dllexport)
#else
#define SONARENGINE_API __declspec(dllimport)
#endif
#else
#define SONARENGINE_API
#endif