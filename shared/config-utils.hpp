#pragma once

#ifdef CONFIG_UTILS_GCC_TEST
#include "bs.hpp"
#include "macros.hpp"
#include "paper.hpp"
#else
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "paper2_scotland2/shared/logger.hpp"
#include "rapidjson-macros/shared/macros.hpp"
#endif

#include <mutex>

#define DECLARE_CONFIG(name) \
struct name##_t; \
inline name##_t& get##name() { \
    return ConfigUtils::ConfigParent<name##_t>::GetInstance(); \
} \
DECLARE_JSON_STRUCT(name##_t), ConfigUtils::ConfigParent<name##_t>

#define CONFIG_VALUE(name, type, jsonName, def, ...) \
private: \
NAMED_VALUE_DEFAULT(ConfigUtils::Specialization<type>::JSONType, __##name, def, jsonName); \
public: \
ConfigUtils::ConfigValue<type> name = {&this->Save, &__##name, jsonName, def __VA_OPT__(,) __VA_ARGS__};

namespace ConfigUtils {
    static constexpr auto Logger = Paper::ConstLoggerContext("config-utils");

    template <typename ValueType>
    struct Specialization {
        using JSONType = ValueType;
    };

    template <typename T>
    struct ConfigParent {
       private:
        static inline std::string __config_path = "";
        static inline T* __self_instance = nullptr;

       public:
        static void Init(modloader::ModInfo const info) {
            __config_path = Configuration::getConfigFilePath(info);
            try {
                ReadFromFile(__config_path, GetInstance());
            } catch (std::exception const& err) {
                ConfigUtils::Logger.error("Error reading config: {} (from {})", err.what(), __config_path);
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
                WriteToFile(__config_path, GetInstance());
            } catch (std::exception const& err) {
                ConfigUtils::Logger.error("Error saving config: {} (to {})", err.what(), __config_path);
            }
        }
    };

    template <typename ValueType>
    class ConfigValue {
       private:
        using JSONType = typename ConfigUtils::Specialization<ValueType>::JSONType;
        std::string name;
        JSONType* value;
        ValueType defaultValue;
        std::string hoverHint;
        void (*saveFunc)();
        std::vector<std::function<void(ValueType)>> changeEvents;
        std::mutex changeEventsMutex;

       public:
        bool operator==(ConfigValue<ValueType> const& other) const {
            return *value == other.GetValue() && defaultValue == other.GetDefaultValue() && name == other.GetName() &&
                   hoverHint == other.GetHoverHint();
        }

        ConfigValue(void (*save)(), JSONType* ref, std::string name, ValueType defaultValue) {
            this->saveFunc = save;
            this->value = ref;
            this->name = name;
            this->defaultValue = defaultValue;
        }

        ConfigValue(void (*save)(), JSONType* ref, std::string name, ValueType defaultValue, std::string hoverHint) :
            ConfigValue(save, ref, name, defaultValue) {
            this->hoverHint = hoverHint;
        }

        void SaveValue() { saveFunc(); }

        ValueType GetValue() { return (ValueType) *value; }

        void SetValue(ValueType value, bool save = true) {
            *this->value = value;
            if (save)
                SaveValue();
            std::lock_guard<std::mutex> lock(changeEventsMutex);
            for (auto& event : changeEvents) {
                event(value);
            }
        }

        ValueType GetDefaultValue() { return defaultValue; }

        std::string GetName() { return name; }

        std::string GetHoverHint() { return hoverHint; }

        void AddChangeEvent(std::function<void(ValueType)> event) {
            std::lock_guard<std::mutex> lock(changeEventsMutex);
            changeEvents.push_back(event);
        }
    };
}

#pragma region CodegenValues
#ifdef CONFIG_UTILS_GCC_TEST
#include "unity.hpp"
#endif

#if __has_include("UnityEngine/Vector2.hpp")

#include "UnityEngine/Color.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Vector4.hpp"

namespace ConfigUtils {

#pragma region JSONClasses

#define CONVERSION(clazz, construct, convert) \
clazz(const UnityEngine::clazz& other) { construct; } \
clazz& operator=(const UnityEngine::clazz& other) { construct; return *this; } \
operator UnityEngine::clazz() const { return UnityEngine::clazz convert; }

    DECLARE_JSON_STRUCT(Vector2) {
        NAMED_VALUE(float, x, NAME_OPTS("x", "X"));
        NAMED_VALUE(float, y, NAME_OPTS("y", "Y"));
        CONVERSION(Vector2, x = other.x; y = other.y;, (x, y));
        Vector2() = default;
        Vector2(float x, float y) : x(x), y(y) {}
    };

    DECLARE_JSON_STRUCT(Vector3) {
        NAMED_VALUE(float, x, NAME_OPTS("x", "X"));
        NAMED_VALUE(float, y, NAME_OPTS("y", "Y"));
        NAMED_VALUE(float, z, NAME_OPTS("z", "Z"));
        CONVERSION(Vector3, x = other.x; y = other.y; z = other.z;, (x, y, z));
        Vector3() = default;
        Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    };

    DECLARE_JSON_STRUCT(Vector4) {
        NAMED_VALUE(float, x, NAME_OPTS("x", "X"));
        NAMED_VALUE(float, y, NAME_OPTS("y", "Y"));
        NAMED_VALUE(float, z, NAME_OPTS("z", "Z"));
        NAMED_VALUE(float, w, NAME_OPTS("w", "W"));
        CONVERSION(Vector4, x = other.x; y = other.y; z = other.z; w = other.w;, (x, y, z, w));
        Vector4() = default;
        Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    };

    DECLARE_JSON_STRUCT(Color) {
        NAMED_VALUE(float, r, NAME_OPTS("r", "R"));
        NAMED_VALUE(float, g, NAME_OPTS("g", "G"));
        NAMED_VALUE(float, b, NAME_OPTS("b", "B"));
        NAMED_VALUE(float, a, NAME_OPTS("a", "A"));
        CONVERSION(Color, r = other.r; g = other.g; b = other.b; a = other.a;, (r, g, b, a));
        Color() = default;
        Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
    };

#pragma endregion

#pragma region Specializations

#define SPECIALIZATION(type) \
template <> \
struct Specialization<UnityEngine::type> { \
    using JSONType = type; \
}; \
template <> \
struct Specialization<std::vector<UnityEngine::type>> { \
    using JSONType = std::vector<type>; \
};

    SPECIALIZATION(Vector2)
    SPECIALIZATION(Vector3)
    SPECIALIZATION(Vector4)
    SPECIALIZATION(Color)

#undef SPECIALIZATION

#pragma endregion

}

#endif
#pragma endregion

#pragma region BSML_LITE
#if __has_include("bsml/shared/BSML-Lite.hpp")
#include "UnityEngine/UI/LayoutElement.hpp"
#include "bsml/shared/BSML-Lite.hpp"

inline BSML::ToggleSetting* AddConfigValueToggle(const BSML::Lite::TransformWrapper& parent, ConfigUtils::ConfigValue<bool>& configValue) {
    auto object =
        BSML::Lite::CreateToggle(parent, configValue.GetName(), configValue.GetValue(), [&configValue](bool value) { configValue.SetValue(value); });
    if (!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return object;
}

inline ::UnityEngine::UI::Toggle*
AddConfigValueModifierButton(const BSML::Lite::TransformWrapper& parent, ConfigUtils::ConfigValue<bool>& configValue) {
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
AddConfigValueIncrementInt(const BSML::Lite::TransformWrapper& parent, ConfigUtils::ConfigValue<int>& configValue, int increment, int min, int max) {
    auto object =
        BSML::Lite::CreateIncrementSetting(parent, configValue.GetName(), 0, increment, configValue.GetValue(), min, max, [&configValue](float value) {
            configValue.SetValue((int) value);
        });
    SetButtons(object);
    if (!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return object;
}

inline BSML::IncrementSetting* AddConfigValueIncrementFloat(
    const BSML::Lite::TransformWrapper& parent, ConfigUtils::ConfigValue<float>& configValue, int decimals, float increment, float min, float max
) {
    auto object = BSML::Lite::CreateIncrementSetting(
        parent,
        configValue.GetName(),
        decimals,
        increment,
        configValue.GetValue(),
        min,
        max,
        [&configValue](float value) { configValue.SetValue(value); }
    );
    SetButtons(object);
    if (!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return object;
}

inline BSML::IncrementSetting* AddConfigValueIncrementDouble(
    const BSML::Lite::TransformWrapper& parent, ConfigUtils::ConfigValue<double>& configValue, int decimals, double increment, double min, double max
) {
    auto object = BSML::Lite::CreateIncrementSetting(
        parent,
        configValue.GetName(),
        decimals,
        increment,
        configValue.GetValue(),
        min,
        max,
        [&configValue](float value) { configValue.SetValue(value); }
    );
    SetButtons(object);
    if (!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return object;
}

inline BSML::IncrementSetting* AddConfigValueIncrementEnum(
    const BSML::Lite::TransformWrapper& parent, ConfigUtils::ConfigValue<int>& configValue, std::vector<std::string> const enumStrings
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
    const BSML::Lite::TransformWrapper& parent, ConfigUtils::ConfigValue<V>& configValue, int decimals, float increment, float min, float max
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
inline BSML::SliderSetting* AddConfigValueSliderIncrement(
    const BSML::Lite::TransformWrapper& parent, ConfigUtils::ConfigValue<V>& configValue, float increment, float min, float max
) {
    auto object = BSML::Lite::CreateSliderSetting(
        parent,
        configValue.GetName(),
        increment,
        configValue.GetValue(),
        min,
        max,
        1,
        true,
        {},
        [&configValue](float value) { configValue.SetValue(value); }
    );
    if (!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return object;
}

inline ::HMUI::InputFieldView*
AddConfigValueInputString(const BSML::Lite::TransformWrapper& parent, ConfigUtils::ConfigValue<std::string>& configValue) {
    auto object = BSML::Lite::CreateStringSetting(parent, configValue.GetName(), configValue.GetValue(), [&configValue](StringW value) {
        configValue.SetValue(static_cast<std::string>(value));
    });
    if (!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return object;
}

inline BSML::DropdownListSetting* AddConfigValueDropdownString(
    const BSML::Lite::TransformWrapper& parent, ConfigUtils::ConfigValue<std::string>& configValue, std::span<std::string_view> const dropdownStrings
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
    const BSML::Lite::TransformWrapper& parent, ConfigUtils::ConfigValue<int>& configValue, std::span<std::string_view> const dropdownStrings
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
AddConfigValueColorPicker(const BSML::Lite::TransformWrapper& parent, ConfigUtils::ConfigValue<::UnityEngine::Color>& configValue) {
    auto object = BSML::Lite::CreateColorPicker(
        parent,
        configValue.GetName(),
        configValue.GetValue(),
        nullptr,
        nullptr,
        [&configValue](::UnityEngine::Color value) { configValue.SetValue(value); }
    );
    if (!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return object;
}

template <class T>
requires std::is_same_v<T, ::UnityEngine::Vector2> || std::is_same_v<T, ::UnityEngine::Vector3> || std::is_same_v<T, ::UnityEngine::Vector4>
inline std::array<BSML::IncrementSetting*, 2>
AddConfigValueIncrementVector2(const BSML::Lite::TransformWrapper& parent, ConfigUtils::ConfigValue<T>& configValue, int decimals, double increment) {
    auto object1 = BSML::Lite::CreateIncrementSetting(
        parent,
        configValue.GetName() + " X",
        decimals,
        increment,
        configValue.GetValue().x,
        [&configValue](float value) {
            auto newValue = configValue.GetValue();
            newValue.x = value;
            configValue.SetValue(newValue);
        }
    );
    SetButtons(object1);
    if (!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object1, configValue.GetHoverHint());
    auto object2 = BSML::Lite::CreateIncrementSetting(
        parent,
        configValue.GetName() + " Y",
        decimals,
        increment,
        configValue.GetValue().y,
        [&configValue](float value) {
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
AddConfigValueIncrementVector3(const BSML::Lite::TransformWrapper& parent, ConfigUtils::ConfigValue<T>& configValue, int decimals, double increment) {
    auto objects = AddConfigValueIncrementVector2(parent, configValue, decimals, increment);
    auto object = BSML::Lite::CreateIncrementSetting(
        parent,
        configValue.GetName() + " Z",
        decimals,
        increment,
        configValue.GetValue().z,
        [&configValue](float value) {
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
    const BSML::Lite::TransformWrapper& parent, ConfigUtils::ConfigValue<::UnityEngine::Vector4>& configValue, int decimals, double increment
) {
    auto objects = AddConfigValueIncrementVector3(parent, configValue, decimals, increment);
    auto object = BSML::Lite::CreateIncrementSetting(
        parent,
        configValue.GetName() + " W",
        decimals,
        increment,
        configValue.GetValue().w,
        [&configValue](float value) {
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
