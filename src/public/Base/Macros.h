#pragma once
#include "Base/SClass.hpp"

template<typename ClassType, typename MemberType>
struct PropertyRegistrar
{
    PropertyRegistrar(SClass* Class, const char* Name, MemberType ClassType::* MemberPtr)
    {
        const ClassType* dummy = nullptr;
        size_t offset = reinterpret_cast<size_t>(&(dummy->*MemberPtr));

        // Print exactly where this registration is happening
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

