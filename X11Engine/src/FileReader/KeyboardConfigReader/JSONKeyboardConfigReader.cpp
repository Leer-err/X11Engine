// #include "JSONKeyboardMouseConfigReader.h"

// #include <fstream>
// #include <nlohmann/json.hpp>
// #include <optional>
// #include <string>

// #include "IKeyboardMouseConfigReader.h"
// #include "Input.h"

// using namespace nlohmann;

// JSONKeyboardMouseConfigReader::JSONKeyboardMouseConfigReader(
//     const std::string& filename) {
//     std::ifstream file(filename);
//     if (!file) return;

//     json data = json::parse(file);

//     for (auto it = data.begin(); it != data.end(); ++it) {
//         auto action_optional = getActionFromName(it.key());
//         if (!action_optional) continue;

//         auto key = it.value().get<std::string>()[0];

//         Binding binding = {};
//         binding.key = key;
//         binding.action = action_optional.value();

//         bindings.emplace_back(binding);
//     }
// }

// std::optional<Binding> JSONKeyboardMouseConfigReader::getBinding() {
//     if (bindings.size() == 0) return std::nullopt;

//     auto binding = bindings.back();
//     bindings.pop_back();

//     return binding;
// }

// std::optional<Action> JSONKeyboardMouseConfigReader::getActionFromName(
//     const std::string& name) const {
//     if (name == "move_forward") return Action::MOVE_FORWARD;
//     if (name == "move_backward") return Action::MOVE_BACKWARD;
//     if (name == "move_left") return Action::MOVE_LEFT;
//     if (name == "move_right") return Action::MOVE_RIGHT;
//     if (name == "move_up") return Action::MOVE_UP;
//     if (name == "move_down") return Action::MOVE_DOWN;
//     if (name == "attack") return Action::ATTACK;
//     if (name == "toggle_dev_overlay") return Action::TOGGLE_DEV_OVERLAY;

//     return std::nullopt;
// }
