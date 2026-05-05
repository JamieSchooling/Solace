#pragma once

#include "Reflection/Property.h"
#include "Reflection/ComponentReflection.h"
#include "Reflection/ReflectionRegistry.h"

#define INIT_REFLECTION(Component) \
    using COMPONENT = Component; \
    static inline std::vector<IProperty*> props;

#define PROPERTY(Member, Type) \
    static inline Property<COMPONENT, &COMPONENT::Member> prop_##Member{#Member, Type}; \
    static inline bool _reg_##Member = []() { \
        props.push_back(&prop_##Member); \
        return true; \
    }();

#define REFLECT(Component) \
    static inline bool _reg_##Component = []() { \
        ReflectionRegistry::Get().push_back(std::make_shared<ComponentReflection<Component>>(#Component, props)); \
        return true; \
    }();