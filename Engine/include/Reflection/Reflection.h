#pragma once

#include "Reflection/Property.h"
#include "Reflection/ComponentReflection.h"
#include "Reflection/ReflectionRegistry.h"
#include "Reflection/PropertyAttributes.h"

#define CONCAT_IMPL(a, b) a##b
#define CONCAT(a, b) CONCAT_IMPL(a, b)

#define INIT_REFLECTION(Component) \
    using COMPONENT = Component; \
    static inline std::vector<IProperty*> props; \
	static inline std::unordered_map<size_t, const char*> headers


#define HEADER(Header) \
	static inline bool CONCAT(_reg_header_, __COUNTER__) = []() { \
        headers[props.size()] = Header; \
        return true; \
    }()

#define PROPERTY(Member, ...) \
    static inline Property<COMPONENT, &COMPONENT::Member> prop_##Member{#Member, {__VA_ARGS__}}; \
    static inline bool _reg_##Member = []() { \
        props.push_back(&prop_##Member); \
        return true; \
    }()

#define REFLECT(Component) \
    static inline bool _reg_##Component = []() { \
        ReflectionRegistry::Get().push_back(std::make_shared<ComponentReflection<Component>>(#Component, props, headers)); \
        return true; \
    }()