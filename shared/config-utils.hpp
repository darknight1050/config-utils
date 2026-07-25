#pragma once

#include "beatsaber-hook/shared/utils.hpp"
#include "paper2_scotland2/shared/logger.hpp"
#include "reflectcpp/include/rfl.hpp"
#include "reflectcpp/include/rfl/json.hpp"

#include <mutex>

#define DECLARE_CONFIG(name)                                 \
    struct name##_t;                                         \
    inline name##_t& get##name() {                           \
        return ConfigUtils::Parent<name##_t>::GetInstance(); \
    }                                                        \
    struct name##_t : ConfigUtils::Parent<name##_t>

#define CONFIG_VALUE(name, type, jsonName, def, ...) \
    ConfigUtils::Value<type, ConfigType, &Save, #name, jsonName __VA_OPT__(,) __VA_ARGS__> name = type(def)

namespace ConfigUtils {
    static constexpr auto Logger = Paper::ConstLoggerContext("config-utils");

    struct RenameProcessor;

    template <typename T>
    struct Parent {
       private:
        static inline std::string __config_path = "";
        static inline T* __self_instance = nullptr;

       public:
        static void Init(modloader::ModInfo const& info) {
            __config_path = get_config_path(info);
            if (!fileexists(__config_path)) {
                ConfigUtils::Logger.info("Config for {} {} does not exist at {}", info.id, info.version, __config_path);
                Save();
                return;
            }
            auto result = rfl::json::read<T, rfl::DefaultIfMissing, RenameProcessor>(readfile(__config_path));
            if (!result) {
                ConfigUtils::Logger.error("Error reading config: {} (from {})", result.error().what(), __config_path);
            } else {
                rfl::to_view(*result).apply([]<typename F>(F const& field) {
                    rfl::to_view(GetInstance()).template get<F>()->value = std::move(field.value()->value);
                });
            }
            Save();
        }
        static inline T& GetInstance() {
            if (__self_instance == nullptr)
                __self_instance = new T();
            return *__self_instance;
        }
        static void Save() {
            if (__config_path.empty()) {
                ConfigUtils::Logger.error("Config was not initialized!");
                return;
            }
            try {
                std::string json = rfl::json::write<RenameProcessor>(GetInstance());
                writefile(__config_path, json);
            } catch (std::exception const& err) {
                ConfigUtils::Logger.error("Error saving config: {} (to {})", err.what(), __config_path);
            }
        }
        using ConfigType = T;
    };

    template <
        typename T,
        typename C,
        auto Save,
        rfl::internal::StringLiteral Field,
        rfl::internal::StringLiteral Json,
        rfl::internal::StringLiteral Hint = "">
    class Value {
       private:
        T value;
        std::vector<std::function<void(T)>> changeEvents;
        std::unique_ptr<std::mutex> changeEventsMutex = std::make_unique<std::mutex>();

        friend Parent<C>;

       public:
        using ReflectionType = T;
        T reflection() const { return value; };
        static constexpr rfl::internal::StringLiteral Rename = Json;

        Value() = default;
        Value(T const& init) : value(init) {}

        bool operator==(T const& value) const { return this->value == value; }

        T const& GetValue() { return value; }
        void SetValue(T const& value, bool save = true) {
            this->value = value;
            if (save)
                Save();
            std::lock_guard lock(*changeEventsMutex);
            for (auto& event : changeEvents)
                event(value);
        }

        std::string GetName() { return Json; }
        std::string GetHoverHint() { return Hint; }
        T GetDefaultValue() {
            static T def = []() {
                C config;
                return rfl::to_view(config).template get<Field>()->GetValue();
            }();
            return def;
        }

        void AddChangeEvent(std::function<void(T)> event) {
            std::lock_guard lock(*changeEventsMutex);
            changeEvents.push_back(event);
        }
    };

    struct RenameProcessor {
        template <typename StructType>
        static auto process(auto&& tuple) {
            if constexpr (std::is_base_of_v<ConfigUtils::Parent<StructType>, StructType>) {
                return tuple.transform([]<typename F>(F const& field) {
                    using T = std::remove_pointer_t<typename F::Type>;
                    if constexpr (requires { T::Rename; }) {
                        return rfl::Field<T::Rename, typename F::Type>(field.value());
                    } else {
                        return field;
                    }
                });
            } else {
                return tuple;
            }
        }
    };
}

#pragma region BSML_LITE
#if __has_include("bsml/shared/BSML-Lite.hpp")
#include "bsml/shared/BSML-Lite.hpp"

#include "UnityEngine/Color.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Vector4.hpp"

inline BSML::ToggleSetting* AddConfigValueToggle(BSML::Lite::TransformWrapper const& parent, ConfigUtils::Value<bool>& configValue) {
    auto object =
        BSML::Lite::CreateToggle(parent, configValue.GetName(), configValue.GetValue(), [&configValue](bool value) { configValue.SetValue(value); });
    if (!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return object;
}

inline ::UnityEngine::UI::Toggle* AddConfigValueModifierButton(BSML::Lite::TransformWrapper const& parent, ConfigUtils::Value<bool>& configValue) {
    auto object = BSML::Lite::CreateModifierButton(parent, configValue.GetName(), configValue.GetValue(), [&configValue](bool value) {
        configValue.SetValue(value);
    });
    if (!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return object;
}

inline void SetButtons(BSML::IncrementSetting* increment) {
    auto child = increment->get_gameObject()->get_transform()->GetChild(1);
    auto decButton = child->GetComponentsInChildren<UnityEngine::UI::Button*>()->First();
    auto incButton = child->GetComponentsInChildren<UnityEngine::UI::Button*>()->Last();
    increment->onChange = [oldFunc = std::move(increment->onChange), increment, decButton, incButton](float value) {
        oldFunc(value);
        decButton->set_interactable(value > increment->minValue);
        incButton->set_interactable(value < increment->maxValue);
    };
    decButton->set_interactable(increment->currentValue > increment->minValue);
    incButton->set_interactable(increment->currentValue < increment->maxValue);
}

inline BSML::IncrementSetting*
AddConfigValueIncrementInt(BSML::Lite::TransformWrapper const& parent, ConfigUtils::Value<int>& configValue, int increment, int min, int max) {
    auto object = BSML::Lite::CreateIncrementSetting(
        parent, configValue.GetName(), 0, increment, configValue.GetValue(), min, max, [&configValue](float value) {
            configValue.SetValue((int) value);
        }
    );
    SetButtons(object);
    if (!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return object;
}

inline BSML::IncrementSetting* AddConfigValueIncrementFloat(
    BSML::Lite::TransformWrapper const& parent, ConfigUtils::Value<float>& configValue, int decimals, float increment, float min, float max
) {
    auto object = BSML::Lite::CreateIncrementSetting(
        parent, configValue.GetName(), decimals, increment, configValue.GetValue(), min, max, [&configValue](float value) {
            configValue.SetValue(value);
        }
    );
    SetButtons(object);
    if (!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return object;
}

inline BSML::IncrementSetting* AddConfigValueIncrementDouble(
    BSML::Lite::TransformWrapper const& parent, ConfigUtils::Value<double>& configValue, int decimals, double increment, double min, double max
) {
    auto object = BSML::Lite::CreateIncrementSetting(
        parent, configValue.GetName(), decimals, increment, configValue.GetValue(), min, max, [&configValue](float value) {
            configValue.SetValue(value);
        }
    );
    SetButtons(object);
    if (!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return object;
}

inline BSML::IncrementSetting* AddConfigValueIncrementEnum(
    BSML::Lite::TransformWrapper const& parent, ConfigUtils::Value<int>& configValue, std::vector<std::string> const enumStrings
) {
    auto object = BSML::Lite::CreateIncrementSetting(parent, configValue.GetName(), 0, 1, configValue.GetValue(), 0, enumStrings.size() - 1);
    object->onChange = [&configValue, object, enumStrings](float value) {
        configValue.SetValue((int) value);
        object->text->set_text(enumStrings.at(value));
    };
    object->text->set_text(enumStrings.at(object->currentValue));
    SetButtons(object);
    if (!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return object;
}

template <class V>
requires(std::is_convertible_v<V, float>)
inline BSML::SliderSetting* AddConfigValueSlider(
    BSML::Lite::TransformWrapper const& parent, ConfigUtils::Value<V>& configValue, int decimals, float increment, float min, float max
) {
    auto object = BSML::Lite::CreateSliderSetting(
        parent, configValue.GetName(), increment, configValue.GetValue(), min, max, [&configValue](float value) { configValue.SetValue(value); }
    );
    object->get_transform().template cast<UnityEngine::RectTransform>()->set_sizeDelta({0, 8});
    if (!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return object;
}

template <class V>
requires(std::is_convertible_v<V, float>)
inline BSML::SliderSetting*
AddConfigValueSliderIncrement(BSML::Lite::TransformWrapper const& parent, ConfigUtils::Value<V>& configValue, float increment, float min, float max) {
    auto object = BSML::Lite::CreateSliderSetting(
        parent, configValue.GetName(), increment, configValue.GetValue(), min, max, 1, true, {}, [&configValue](float value) {
            configValue.SetValue(value);
        }
    );
    if (!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return object;
}

inline ::HMUI::InputFieldView* AddConfigValueInputString(BSML::Lite::TransformWrapper const& parent, ConfigUtils::Value<std::string>& configValue) {
    auto object = BSML::Lite::CreateStringSetting(parent, configValue.GetName(), configValue.GetValue(), [&configValue](StringW value) {
        configValue.SetValue(static_cast<std::string>(value));
    });
    if (!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return object;
}

inline BSML::DropdownListSetting* AddConfigValueDropdownString(
    BSML::Lite::TransformWrapper const& parent, ConfigUtils::Value<std::string>& configValue, std::span<std::string_view> const dropdownStrings
) {
    int currentIndex = 0;
    std::string_view currentValue = "";
    for (int i = 0; i < dropdownStrings.size(); i++) {
        if (configValue.GetValue() == dropdownStrings[i]) {
            currentIndex = i;
            currentValue = dropdownStrings[i];
            break;
        }
    }

    auto object = BSML::Lite::CreateDropdown(parent, configValue.GetName(), currentValue, dropdownStrings, [&configValue](StringW value) {
        configValue.SetValue(static_cast<std::string>(value));
    });
    object->get_transform()->GetParent()->template GetComponent<::UnityEngine::UI::LayoutElement*>()->set_preferredHeight(7);
    if (!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return object;
}

inline BSML::DropdownListSetting* AddConfigValueDropdownEnum(
    BSML::Lite::TransformWrapper const& parent, ConfigUtils::Value<int>& configValue, std::span<std::string_view> const dropdownStrings
) {
    int value = configValue.GetValue();
    std::string_view stringValue = value < dropdownStrings.size() ? dropdownStrings[value] : "";
    auto object = BSML::Lite::CreateDropdown(
        parent,
        configValue.GetName(),
        stringValue,
        dropdownStrings,
        [&configValue, dropdownStrings = std::vector<std::string>(dropdownStrings.begin(), dropdownStrings.end())](StringW value) {
            for (int i = 0; i < dropdownStrings.size(); i++) {
                if (value == dropdownStrings.at(i)) {
                    configValue.SetValue(i);
                    break;
                }
            }
        }
    );
    object->get_transform()->GetParent()->template GetComponent<::UnityEngine::UI::LayoutElement*>()->set_preferredHeight(7);
    if (!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return object;
}

inline BSML::ColorSetting*
AddConfigValueColorPicker(BSML::Lite::TransformWrapper const& parent, ConfigUtils::Value<::UnityEngine::Color>& configValue) {
    auto object = BSML::Lite::CreateColorPicker(
        parent, configValue.GetName(), configValue.GetValue(), nullptr, nullptr, [&configValue](::UnityEngine::Color value) {
            configValue.SetValue(value);
        }
    );
    if (!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return object;
}

template <class T>
requires std::is_same_v<T, ::UnityEngine::Vector2> || std::is_same_v<T, ::UnityEngine::Vector3> || std::is_same_v<T, ::UnityEngine::Vector4>
inline std::array<BSML::IncrementSetting*, 2>
AddConfigValueIncrementVector2(BSML::Lite::TransformWrapper const& parent, ConfigUtils::Value<T>& configValue, int decimals, double increment) {
    auto object1 = BSML::Lite::CreateIncrementSetting(
        parent, configValue.GetName() + " X", decimals, increment, configValue.GetValue().x, [&configValue](float value) {
            auto newValue = configValue.GetValue();
            newValue.x = value;
            configValue.SetValue(newValue);
        }
    );
    SetButtons(object1);
    if (!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object1, configValue.GetHoverHint());
    auto object2 = BSML::Lite::CreateIncrementSetting(
        parent, configValue.GetName() + " Y", decimals, increment, configValue.GetValue().y, [&configValue](float value) {
            auto newValue = configValue.GetValue();
            newValue.y = value;
            configValue.SetValue(newValue);
        }
    );
    SetButtons(object2);
    if (!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object2, configValue.GetHoverHint());
    return {object1, object2};
}

template <class T>
requires std::is_same_v<T, ::UnityEngine::Vector3> || std::is_same_v<T, ::UnityEngine::Vector4>
inline std::array<BSML::IncrementSetting*, 3>
AddConfigValueIncrementVector3(BSML::Lite::TransformWrapper const& parent, ConfigUtils::Value<T>& configValue, int decimals, double increment) {
    auto objects = AddConfigValueIncrementVector2(parent, configValue, decimals, increment);
    auto object = BSML::Lite::CreateIncrementSetting(
        parent, configValue.GetName() + " Z", decimals, increment, configValue.GetValue().z, [&configValue](float value) {
            auto newValue = configValue.GetValue();
            newValue.z = value;
            configValue.SetValue(newValue);
        }
    );
    SetButtons(object);
    if (!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return {objects[0], objects[1], object};
}

inline std::array<::BSML::IncrementSetting*, 4> AddConfigValueIncrementVector4(
    BSML::Lite::TransformWrapper const& parent, ConfigUtils::Value<::UnityEngine::Vector4>& configValue, int decimals, double increment
) {
    auto objects = AddConfigValueIncrementVector3(parent, configValue, decimals, increment);
    auto object = BSML::Lite::CreateIncrementSetting(
        parent, configValue.GetName() + " W", decimals, increment, configValue.GetValue().w, [&configValue](float value) {
            auto newValue = configValue.GetValue();
            newValue.w = value;
            configValue.SetValue(newValue);
        }
    );
    SetButtons(object);
    if (!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return {objects[0], objects[1], objects[2], object};
}

#endif
#pragma endregion
