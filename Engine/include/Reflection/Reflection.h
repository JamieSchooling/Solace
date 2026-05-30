#pragma once

#include "Reflection/Property.h"
#include "Reflection/ComponentReflection.h"
#include "Reflection/ReflectionModule.h"

#include <iostream>

#define INIT_REFLECTION(Component) \
    using COMPONENT = Component; \
    static inline std::vector<IProperty*> props;

#define PROPERTY(Member) \
    static inline Property<COMPONENT, &COMPONENT::Member> prop_##Member{#Member}; \
    static inline bool _reg_##Member = []() { \
        props.push_back(&prop_##Member); \
        return true; \
    }();

#define REFLECT(Component) \
    static inline bool _reg_##Component = []() { \
		std::cout << "Registering: " << #Component << std::endl; \
        GetReflectionModule().Components.push_back(std::make_shared<ComponentReflection<Component>>(#Component, props)); \
        return true; \
    }();