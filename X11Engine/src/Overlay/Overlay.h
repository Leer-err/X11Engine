#pragma once

#include <concepts>
#include <functional>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace Overlay {
namespace OverlayElements {

template <typename T>
concept OverlayElement = requires(T el) {
    { el.draw() } -> std::same_as<void>;
};

class SliderFloat {
   public:
    SliderFloat(const std::string& label, std::function<void(float)> callback,
                float min, float max)
        : SliderFloat(label, callback, min, max, min) {}
    SliderFloat(const std::string& label, std::function<void(float)> callback,
                float min, float max, float initial);

    void draw();

   private:
    std::string label;

    float minimal;
    float maximum;
    float value;
    std::function<void(float)> callback;
};

class Header;

using AnyElement =
    std::variant<OverlayElements::Header, OverlayElements::SliderFloat>;

class Header {
   public:
    Header(const std::string& label);

    void draw();

    template <typename T, typename... ARGS>
    void add(const std::string& path, ARGS&&... args) {
        auto index = std::find(path.begin(), path.end(), '/');

        if (path.empty()) {
            elements.emplace_back(T(std::forward<ARGS>(args)...));
            return;
        }

        std::string header_name;
        if (index == path.end())
            header_name = path;
        else
            header_name = std::string(path.begin(), index);

        auto it = headers.find(header_name);
        if (it == headers.end()) {
            auto result =
                headers.emplace(path, OverlayElements::Header(header_name));
            it = result.first;
        }

        it->second.add<T>(std::string(index, path.end()),
                          std::forward<ARGS>(args)...);
    }

   private:
    std::string label;

    std::unordered_map<std::string, OverlayElements::Header> headers;
    std::vector<OverlayElements::AnyElement> elements;
};
};  // namespace OverlayElements

class Overlay {
   public:
    static Overlay& get() {
        static Overlay instance;
        return instance;
    }

    void draw();

    template <typename T, typename... ARGS>
    void add(const std::string& path, ARGS&&... args) {
        if (path.empty()) {
            elements.emplace_back(T(std::forward<ARGS>(args)...));
            return;
        }

        auto index = std::find(path.begin(), path.end(), '/');

        std::string header_name;
        if (index == path.end())
            header_name = path;
        else
            header_name = std::string(path.begin(), index);

        auto it = headers.find(header_name);
        if (it == headers.end()) {
            auto result =
                headers.emplace(path, OverlayElements::Header(header_name));
            it = result.first;
        }

        it->second.add<T>(std::string(index, path.end()),
                          std::forward<ARGS>(args)...);
    }

   private:
    Overlay();
    ~Overlay();

    std::unordered_map<std::string, OverlayElements::Header> headers;
    std::vector<OverlayElements::AnyElement> elements;
};

};  // namespace Overlay