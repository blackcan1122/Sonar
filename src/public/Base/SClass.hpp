#pragma once
#include <string>
#include <unordered_map>
#include <iostream>
#include <type_traits>

enum class EPropertyType
{
    Int,
    Float,
    Bool,
    Unknown
};

class IProperty
{
public:
    IProperty(const char* Name, EPropertyType Type)
        : PropertyName(Name), Type(Type) {
    }

    virtual ~IProperty() = default;

    virtual void* GetValuePtr(void* Object) const = 0;

    template<typename T>
    T& GetValue(void* Object) const {
        return *static_cast<T*>(GetValuePtr(Object));
    }

    template<typename T>
    void SetValue(void* Object, const T& Value) const {
        *static_cast<T*>(GetValuePtr(Object)) = Value;
    }

    const std::string PropertyName;
    EPropertyType Type;
};

class SClass
{
public:
    explicit SClass(const SClass* Parent, const char* Name)
        : ParentClass(Parent), ClassName(Name)
    {
        GetClassRegistry()[Name] = this;
    }

    void RegisterProperty(std::unique_ptr<IProperty> Prop)
    {
        Properties.push_back(std::move(Prop));
    }

    IProperty* FindProperty(const std::string& Name) const
    {
        for (const auto& Prop : Properties)
        {
            if (Prop->PropertyName == Name)
                return Prop.get();
        }
        if (ParentClass)
            return ParentClass->FindProperty(Name);
        return nullptr;
    }

    std::vector<IProperty*> GetAllProperties() const
    {
        std::vector<IProperty*> Result;
        if (ParentClass)
            Result = ParentClass->GetAllProperties();
        for (const auto& Prop : Properties)
            Result.push_back(Prop.get());
        return Result;
    }

    template<typename T>
    T* GetPropertyValue(const std::string& Name, void* Object) const
    {
        if (auto prop = FindProperty(Name))
            return static_cast<T*>(prop->GetValuePtr(Object));
        return nullptr;
    }

    template<typename T>
    bool SetPropertyValue(const std::string& Name, void* Object, const T& Value) const
    {
        if (auto prop = FindProperty(Name))
        {
            *static_cast<T*>(prop->GetValuePtr(Object)) = Value;
            return true;
        }
        return false;
    }

    bool operator<<(SClass* Other) const
    {
        const SClass* current = this;
        while (current)
        {
            if (current == Other) return true;
            current = current->ParentClass;
        }
        return false;
    }

    static SClass* FindClass(const std::string& Name)
    {
        auto& Registry = GetClassRegistry();
        auto it = Registry.find(Name);
        return (it != Registry.end()) ? it->second : nullptr;
    }

    const std::string ClassName;

    static std::unordered_map<std::string, SClass*>& GetClassRegistry()
    {
        static std::unordered_map<std::string, SClass*> Registry;
        return Registry;
    }

protected:
    const SClass* ParentClass;
    std::vector<std::unique_ptr<IProperty>> Properties;
};



template<typename ClassType, typename MemberType>
class SProperty : public IProperty
{
public:
    SProperty(const char* Name, MemberType ClassType::* InMemberPtr)
        : IProperty(Name, DeduceType()), MemberPtr(InMemberPtr)
    {
    }

    void* GetValuePtr(void* Object) const override
    {
        return &(static_cast<ClassType*>(Object)->*MemberPtr);
    }

    
    MemberType GetValue(void* Object) const
    {
        return static_cast<ClassType*>(Object)->*MemberPtr;
    }

    void SetValue(void* Object, const MemberType& Value) const
    {
        static_cast<ClassType*>(Object)->*MemberPtr = Value;
    }

private:
    MemberType ClassType::* MemberPtr;

    static constexpr EPropertyType DeduceType()
    {
        if constexpr (std::is_same_v<MemberType, int>) return EPropertyType::Int;
        else if constexpr (std::is_same_v<MemberType, float>) return EPropertyType::Float;
        else if constexpr (std::is_same_v<MemberType, bool>) return EPropertyType::Bool;
        else return EPropertyType::Unknown;
    }
};