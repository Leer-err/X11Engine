#pragma once

#include <cmath>
#include <random>
#include <type_traits>
#include <variant>
#include <vector>

#include "Vector3.h"
#include "Vector4.h"

template <typename T>
class Property {
   public:
    struct State {
        float time;
        T value;
    };

   private:
    struct ConstantProperty {
        T value;

        T evaluate(float lifetime) const;
    };

    struct RandomBetweenPointsProperty {
        T min;
        T max;

        T evaluate(float lifetime) const;
    };

    struct OverLifetimeProperty {
        std::vector<State> states;

        T evaluate(float lifetime) const;
    };

   public:
    Property() : property(ConstantProperty()) {}

    static Property constant(const T& value);
    static Property random(const T& min, const T& max);
    static Property overLifetime(const std::vector<State>& states);

    T evaluate(float lifetime);

   private:
    explicit Property(
        const std::variant<ConstantProperty, RandomBetweenPointsProperty,
                           OverLifetimeProperty>& value)
        : property(value) {}

    std::variant<ConstantProperty, RandomBetweenPointsProperty,
                 OverLifetimeProperty>
        property;
};

template <typename T>
Property<T> Property<T>::constant(const T& value) {
    return Property(ConstantProperty(value));
}

template <typename T>
Property<T> Property<T>::random(const T& min, const T& max) {
    return Property(RandomBetweenPointsProperty(min, max));
}

template <typename T>
Property<T> Property<T>::overLifetime(const std::vector<State>& states) {
    return Property(OverLifetimeProperty(states));
}

template <typename T>
T Property<T>::evaluate(float lifetime) {
    auto evaluator = [lifetime](auto& value) {
        return value.evaluate(lifetime);
    };

    return std::visit(evaluator, property);
}

template <typename T>
T Property<T>::ConstantProperty::evaluate(float lifetime) const {
    return value;
}

template <typename T>
T Property<T>::RandomBetweenPointsProperty::evaluate(float lifetime) const {
    static std::random_device rd;
    static std::mt19937 gen(rd());

    if constexpr (std::is_integral_v<T>) {
        std::uniform_int_distribution<T> dist(min, max);

        return dist(gen);
    } else if constexpr (std::is_floating_point_v<T>) {
        std::uniform_real_distribution<T> dist(min, max);

        return dist(gen);
    } else if constexpr (std::is_same_v<T, Vector3>) {
        std::uniform_real_distribution<float> x_dist(min.x, max.x);
        std::uniform_real_distribution<float> y_dist(min.y, max.y);
        std::uniform_real_distribution<float> z_dist(min.z, max.z);

        return Vector3(x_dist(gen), y_dist(gen), z_dist(gen));
    } else if constexpr (std::is_same_v<T, Vector4>) {
        std::uniform_real_distribution<float> x_dist(min.x, max.x);
        std::uniform_real_distribution<float> y_dist(min.y, max.y);
        std::uniform_real_distribution<float> z_dist(min.z, max.z);
        std::uniform_real_distribution<float> w_dist(min.z, max.z);

        return Vector4(x_dist(gen), y_dist(gen), z_dist(gen), w_dist(gen));
    }
}

template <typename T>
T Property<T>::OverLifetimeProperty::evaluate(float lifetime) const {
    auto next = std::ranges::upper_bound(
        states, State{lifetime, T()},
        [](const State& a, const State& b) { return a.time < b.time; });

    auto prev = next - 1;

    auto lifetime_ratio = (lifetime - prev->time) / (next->time - prev->time);

    if constexpr (std::is_arithmetic_v<T>)
        return std::lerp(prev->value, next->value, lifetime_ratio);
    else {
        return T::lerp(prev->value, next->value, lifetime_ratio);
    }
}