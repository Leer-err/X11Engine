#ifndef INPUT_CONTEXT_H
#define INPUT_CONTEXT_H

#include <map>
#include <memory>
#include <vector>

#include "PhysicalInput.h"

template <typename ActionType, typename AxisType>
class InputContext {
    class IButtonBinding {
       public:
        virtual ButtonState evaluate(const PhysicalInput& input) const = 0;
        virtual ~IButtonBinding() {};
    };

    class IAxisBinding {
       public:
        virtual float evaluate(const PhysicalInput& input) const = 0;
        virtual ~IAxisBinding() {};
    };

    class ButtonBinding : public IButtonBinding {
       public:
        ButtonBinding(Button button) : key(button) {}

        ButtonState evaluate(const PhysicalInput& input) const override {
            return input.getButtonState(key);
        }

       private:
        Button key;
    };

    class AxisButtonBinding : public IButtonBinding {
       public:
        AxisButtonBinding(Axis axis, float threshold)
            : axis(axis), threshold(threshold) {}

        ButtonState evaluate(const PhysicalInput& input) const override {
            float value = input.getAxisState(axis);
            if (value >= threshold) return ButtonState::Held;

            return ButtonState::None;
        }

       private:
        Axis axis;
        float threshold;
    };

    class AxisBinding : public IAxisBinding {
       public:
        AxisBinding(Axis axis) : axis(axis) {}

        float evaluate(const PhysicalInput& input) const override {
            return input.getAxisState(axis);
        }

       private:
        Axis axis;
    };

    class ButtonAxisBinding : public IAxisBinding {
       public:
        ButtonAxisBinding() {}

        void addBinding(Button button, float scale) {
            buttons.push_back(button);
            scalings.push_back(scale);
        }

        float evaluate(const PhysicalInput& input) const override {
            float value = 0;

            for (int i = 0; i < buttons.size(); i++) {
                ButtonState state = input.getButtonState(buttons[i]);

                if (state == ButtonState::Pressed ||
                    state == ButtonState::Held) {
                    value += scalings[i];
                }
            }

            return value;
        }

       private:
        std::vector<Button> buttons;
        std::vector<float> scalings;
    };

   public:
    bool addBinding(ActionType action, Button button) {
        auto binding = std::make_unique<ButtonBinding>(button);

        size_t index = button_bindings.size();
        button_indices.emplace(action, index);

        button_bindings.emplace_back(std::move(binding));

        return true;
    }

    bool addBinding(AxisType axis, Axis physical_axis) {
        auto binding = std::make_unique<AxisBinding>(physical_axis);

        size_t index = axis_bindings.size();
        axis_indices.emplace(axis, index);

        axis_bindings.emplace_back(std::move(binding));

        return true;
    }

    bool addBinding(AxisType axis, Button button, float scale) {
        auto binding_it = axis_indices.find(axis);

        ButtonAxisBinding* binding_ptr;

        if (binding_it == axis_indices.end()) {
            auto binding = std::make_unique<ButtonAxisBinding>();

            binding_ptr = binding.get();

            size_t index = axis_bindings.size();
            axis_indices.emplace(axis, index);

            axis_bindings.emplace_back(std::move(binding));
        } else {
            binding_ptr =
                (ButtonAxisBinding*)axis_bindings[binding_it->second].get();
        }

        binding_ptr->addBinding(button, scale);

        return true;
    }

    float getAxis(AxisType action) {
        auto index_it = axis_indices.find(action);

        if (index_it == axis_indices.end()) return 0.f;

        return axis_bindings[index_it->second]->evaluate(PhysicalInput::get());
    }

    ButtonState getButton(ActionType action) {
        auto index_it = button_indices.find(action);

        if (index_it == button_indices.end()) return ButtonState::None;

        return button_bindings[index_it->second]->evaluate(
            PhysicalInput::get());
    }

   private:
    std::vector<std::unique_ptr<IButtonBinding>> button_bindings;
    std::map<ActionType, size_t> button_indices;

    std::vector<std::unique_ptr<IAxisBinding>> axis_bindings;
    std::map<AxisType, size_t> axis_indices;

    std::shared_ptr<PhysicalInput> input;
};

#endif  // INPUT_CONTEXT_H