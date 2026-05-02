#pragma once

#include <any> // <-- Not great, find a better 

struct IProperty
{
    virtual ~IProperty() = default;
    virtual const char* Name() const = 0;
    virtual std::any Get(void* owner) const = 0;
    virtual void Set(void* owner, const std::any& value) const = 0;
};

template<typename Owner, auto Member>
struct Property : public IProperty
{
    Property(const char* n) : name(n) {}

    const char* name;
    using ValueType = std::remove_reference_t<
        decltype(std::declval<Owner>().*Member)
    >;

    ValueType& Get(Owner& o) const
    {
        return o.*Member;
    }

    void Set(Owner& o, const ValueType& v) const
    {
        o.*Member = v;
    }

    const char* Name() const override
    {
        return name;
    }

    std::any Get(void* owner) const override
    {
        return Get(*reinterpret_cast<Owner*>(owner));
    }

    void Set(void* owner, const std::any& value) const override
    {
        Set(*reinterpret_cast<Owner*>(owner), std::any_cast<ValueType>(value));
    }
};