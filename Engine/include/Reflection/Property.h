#pragma once

#include <any> // <-- Not great, find a better alternative

struct IComponentReflection;

struct IProperty
{
    virtual ~IProperty() = default;
    virtual const char* Name() const = 0;
    virtual std::any Get(entt::registry& r, entt::entity e) const = 0;
    virtual void Set(const std::any& value, entt::registry& r, entt::entity e) const = 0;
    virtual void SetReflectionTarget(IComponentReflection* owner) = 0;
};

template<typename Target, auto Member>
struct Property : public IProperty
{
    Property(const char* n) : name(n) {}

    const char* name;
    using ValueType = std::remove_reference_t<
        decltype(std::declval<Target>().*Member)
    >;

    ValueType& Get(Target* o) const
    {
        return o->*Member;
    }

    void Set(Target* o, const ValueType& v) const
    {
        o->*Member = v;
    }

    const char* Name() const override
    {
        return name;
    }

    std::any Get(entt::registry& r, entt::entity e) const override
    {
        return Get(reflectionTarget->template GetTarget<Target>(r, e));
    }

    void Set(const std::any& value, entt::registry& r, entt::entity e) const override
    {
        Set(reflectionTarget->template GetTarget<Target>(r, e), std::any_cast<ValueType>(value));
    }

    void SetReflectionTarget(IComponentReflection* target) override
    {
        reflectionTarget = target;
    }

private:
    IComponentReflection* reflectionTarget = nullptr;
};