#pragma once
#include "paper/shared/logger.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "rapidjson-macros/shared/macros.hpp"

#define DECLARE_CONFIG(name, ...) \
DECLARE_JSON_CLASS(name##_t, \
    DISCARD_EXTRA_FIELDS \
    __VA_ARGS__ \
    private: \
    static inline std::string __config_path = ""; \
    static inline name##_t* __self_instance = nullptr; \
    public: \
    static void Init(const modloader::ModInfo info) { \
        __config_path = Configuration::getConfigFilePath(info); \
        if(!__self_instance) \
            __self_instance = new name##_t(); \
        try { \
            ReadFromFile(__config_path, GetInstance()); \
        } catch (const std::exception& err) { \
            ConfigUtils::Logger.error("Error reading config: {} (from {})", err.what(), __config_path); \
        } \
		Save(); \
    } \
    static inline name##_t& GetInstance() { \
        return *__self_instance; \
    } \
    static void Save() { \
        if (__config_path.empty()) { \
            ConfigUtils::Logger.error("Config was not initialized!"); \
            return; \
        } \
        try { \
            WriteToFile(__config_path, GetInstance()); \
        } catch (const std::exception& err) { \
            ConfigUtils::Logger.error("Error saving config: {} (to {})", err.what(), __config_path); \
        } \
    } \
) \
inline name##_t& get##name() { \
    return name##_t::GetInstance(); \
}

#define CONFIG_VALUE(name, type, jsonName, def, ...) \
private: \
NAMED_VALUE_DEFAULT(ConfigUtils::Specialization<type>::JSONType, __##name, def, jsonName) \
public: \
ConfigUtils::ConfigValue<type> name = {&this->Save, &__##name, jsonName, def __VA_OPT__(,) __VA_ARGS__};

namespace ConfigUtils {
    auto Logger = Paper::ConstLoggerContext("config-utils");

    template <typename ValueType>
    struct Specialization {
        using JSONType = ValueType;
    };

    template <typename ValueType>
    class ConfigValue {
    private:
        using JSONType = typename ConfigUtils::Specialization<ValueType>::JSONType;
        std::string name;
        JSONType* value;
        ValueType defaultValue;
        std::string hoverHint;
        void(*saveFunc)();
        std::vector<std::function<void(ValueType)>> changeEvents;
        std::mutex changeEventsMutex;

    public:
        bool operator==(const ConfigValue<ValueType>& other) const {
            return *value == other.GetValue()
                   && defaultValue == other.GetDefaultValue()
                   && name == other.GetName()
                   && hoverHint == other.GetHoverHint();
        }

        ConfigValue(void(*save)(), JSONType* ref, std::string name, ValueType defaultValue) {
            this->saveFunc = save;
            this->value = ref;
            this->name = name;
            this->defaultValue = defaultValue;
        }

        ConfigValue(void(*save)(), JSONType* ref, std::string name, ValueType defaultValue, std::string hoverHint) : ConfigValue(save, ref, name, defaultValue) {
            this->hoverHint = hoverHint;
        }

        void SaveValue() {
            saveFunc();
        }

        ValueType GetValue() {
            return (ValueType) *value;
        }

        void SetValue(ValueType value, bool save = true) {
            *this->value = value;
            if(save)
                SaveValue();
            std::lock_guard<std::mutex> lock(changeEventsMutex);
            for (auto& event : changeEvents) {
                event(value);
            }
        }

        ValueType GetDefaultValue() {
            return defaultValue;
        }

        std::string GetName() {
            return name;
        }

        std::string GetHoverHint() {
            return hoverHint;
        }

        void AddChangeEvent(std::function<void(ValueType)> event) {
            std::lock_guard<std::mutex> lock(changeEventsMutex);
            changeEvents.push_back(event);
        }
    };
}

#pragma region CodegenValues
#if __has_include("UnityEngine/Vector2.hpp")

#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Vector4.hpp"
#include "UnityEngine/Color.hpp"

namespace ConfigUtils {

#pragma region JSONClasses

#define CONVERSION(clazz, construct, convert) \
clazz(const UnityEngine::clazz& other) { construct; } \
clazz& operator=(const UnityEngine::clazz& other) { construct; return *this; } \
operator UnityEngine::clazz() const { return UnityEngine::clazz convert; }

DECLARE_JSON_CLASS(Vector2,
    VALUE(float, x)
    VALUE(float, y)
    CONVERSION(Vector2,
        x = other.x; y = other.y;,
        (x, y)
    )
    Vector2() = default;
    Vector2(float x, float y) : x(x), y(y) {}
)

DECLARE_JSON_CLASS(Vector3,
    VALUE(float, x)
    VALUE(float, y)
    VALUE(float, z)
    CONVERSION(Vector3,
        x = other.x; y = other.y; z = other.z;,
        (x, y, z)
    )
    Vector3() = default;
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
)

DECLARE_JSON_CLASS(Vector4,
    VALUE(float, x)
    VALUE(float, y)
    VALUE(float, z)
    VALUE(float, w)
    CONVERSION(Vector4,
        x = other.x; y = other.y; z = other.z; w = other.w;,
        (x, y, z, w)
    )
    Vector4() = default;
    Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
)

DECLARE_JSON_CLASS(Color,
    VALUE(float, r)
    VALUE(float, g)
    VALUE(float, b)
    VALUE(float, a)
    CONVERSION(Color,
        r = other.r; g = other.g; b = other.b; a = other.a;,
        (r, g, b, a)
    )
    Color() = default;
    Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
)

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
#if __has_include("BSML/shared/BSML-Lite.hpp")
#include "BSML/shared/BSML-Lite.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"

inline BSML::ToggleSetting* AddConfigValueToggle(const BSML::Lite::TransformWrapper& parent, ConfigUtils::ConfigValue<bool>& configValue) {
    auto object = BSML::Lite::CreateToggle(parent, configValue.GetName(), configValue.GetValue(),
        [&configValue](bool value) {
            configValue.SetValue(value);
        }
    );
    if(!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return object;
}

inline ::UnityEngine::UI::Toggle* AddConfigValueModifierButton(const BSML::Lite::TransformWrapper& parent, ConfigUtils::ConfigValue<bool>& configValue) {
    auto object = BSML::Lite::CreateModifierButton(parent, configValue.GetName(), configValue.GetValue(),
        [&configValue](bool value) {
            configValue.SetValue(value);
        }
    );
    if(!configValue.GetHoverHint().empty())
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

inline BSML::IncrementSetting* AddConfigValueIncrementInt(const BSML::Lite::TransformWrapper& parent, ConfigUtils::ConfigValue<int>& configValue, int increment, int min, int max) {
    auto object = BSML::Lite::CreateIncrementSetting(parent, configValue.GetName(), 0, increment, configValue.GetValue(), min, max,
        [&configValue](float value) {
            configValue.SetValue((int)value);
        }
    );
    SetButtons(object);
    if(!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return object;
}

inline BSML::IncrementSetting* AddConfigValueIncrementFloat(const BSML::Lite::TransformWrapper& parent, ConfigUtils::ConfigValue<float>& configValue, int decimals, float increment, float min, float max) {
    auto object = BSML::Lite::CreateIncrementSetting(parent, configValue.GetName(), decimals, increment, configValue.GetValue(), min, max,
        [&configValue](float value) {
            configValue.SetValue(value);
        }
    );
    SetButtons(object);
    if(!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return object;
}

inline BSML::IncrementSetting* AddConfigValueIncrementDouble(const BSML::Lite::TransformWrapper& parent, ConfigUtils::ConfigValue<double>& configValue, int decimals, double increment, double min, double max) {
    auto object = BSML::Lite::CreateIncrementSetting(parent, configValue.GetName(), decimals, increment, configValue.GetValue(), min, max,
        [&configValue](float value) {
            configValue.SetValue(value);
        }
    );
    SetButtons(object);
    if(!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return object;
}

inline BSML::IncrementSetting* AddConfigValueIncrementEnum(const BSML::Lite::TransformWrapper& parent, ConfigUtils::ConfigValue<int>& configValue, const std::vector<std::string> enumStrings) {
    auto object = BSML::Lite::CreateIncrementSetting(parent, configValue.GetName(), 0, 1, configValue.GetValue(), 0, enumStrings.size() - 1);
    object->onChange = [&configValue, object, enumStrings](float value) {
        configValue.SetValue((int) value);
        object->text->set_text(enumStrings[value]);
    };
    object->text->set_text(enumStrings[object->currentValue]);
    SetButtons(object);
    if(!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return object;
}

template<class V>
requires (std::is_convertible_v<V, float>)
inline BSML::SliderSetting* AddConfigValueSlider(const BSML::Lite::TransformWrapper& parent, ConfigUtils::ConfigValue<V>& configValue, int decimals, float increment, float min, float max) {
    auto object = BSML::Lite::CreateSliderSetting(parent, configValue.GetName(), increment, configValue.GetValue(), min, max,
        [&configValue](float value) {
            configValue.SetValue(value);
        }
    );
    object->get_transform().template cast<UnityEngine::RectTransform>()->set_sizeDelta({0, 8});
    if(!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return object;
}

template<class V>
requires (std::is_convertible_v<V, float>)
inline BSML::SliderSetting* AddConfigValueSliderIncrement(const BSML::Lite::TransformWrapper& parent, ConfigUtils::ConfigValue<V>& configValue, float increment, float min, float max) {
    auto object = BSML::Lite::CreateSliderSetting(parent, configValue.GetName(), increment, configValue.GetValue(), min, max, 1, true, {},
          [&configValue](float value) {
              configValue.SetValue(value);
          }
    );
    if(!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return object;
}

inline ::HMUI::InputFieldView* AddConfigValueInputString(const BSML::Lite::TransformWrapper& parent, ConfigUtils::ConfigValue<std::string>& configValue) {
    auto object = BSML::Lite::CreateStringSetting(parent, configValue.GetName(), configValue.GetValue(),
        [&configValue](StringW value) {
            configValue.SetValue(static_cast<std::string>(value));
        }
    );
    if(!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return object;
}

inline BSML::DropdownListSetting* AddConfigValueDropdownString(const BSML::Lite::TransformWrapper& parent, ConfigUtils::ConfigValue<std::string>& configValue, const std::span<std::string_view> dropdownStrings) {
    int currentIndex = 0;
    for(int i = 0; i < dropdownStrings.size(); i++) {
        if(configValue.GetValue() == dropdownStrings[i]) {
            currentIndex = i;
            break;
        }
    }

    auto object = BSML::Lite::CreateDropdown(parent, configValue.GetName(), dropdownStrings[currentIndex], dropdownStrings,
        [&configValue](StringW value) {
            configValue.SetValue(static_cast<std::string>(value));
        }
    );
    object->get_transform()->GetParent()->template GetComponent<::UnityEngine::UI::LayoutElement*>()->set_preferredHeight(7);
    if(!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return object;
}

inline BSML::DropdownListSetting* AddConfigValueDropdownEnum(const BSML::Lite::TransformWrapper& parent, ConfigUtils::ConfigValue<int>& configValue, const std::span<std::string_view> dropdownStrings) {
    auto object = BSML::Lite::CreateDropdown(parent, configValue.GetName(), dropdownStrings[configValue.GetValue()], dropdownStrings,
        [&configValue, dropdownStrings = std::vector<std::string>(dropdownStrings.begin(), dropdownStrings.end())](StringW value) {
            for(int i = 0; i < dropdownStrings.size(); i++) {
                if(value == dropdownStrings[i]) {
                    configValue.SetValue(i);
                    break;
                }
            }
        }
    );
    object->get_transform()->GetParent()->template GetComponent<::UnityEngine::UI::LayoutElement*>()->set_preferredHeight(7);
    if(!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return object;
}

inline BSML::ColorSetting* AddConfigValueColorPicker(const BSML::Lite::TransformWrapper& parent, ConfigUtils::ConfigValue<::UnityEngine::Color>& configValue) {
    auto object = BSML::Lite::CreateColorPicker(parent, configValue.GetName(), configValue.GetValue(), nullptr, nullptr,
        [&configValue](::UnityEngine::Color value) {
            configValue.SetValue(value);
        }
    );
    if(!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return object;
}

template<class T>
requires std::is_same_v<T, ::UnityEngine::Vector2> || std::is_same_v<T, ::UnityEngine::Vector3> || std::is_same_v<T, ::UnityEngine::Vector4>
inline std::array<BSML::IncrementSetting*, 2> AddConfigValueIncrementVector2(const BSML::Lite::TransformWrapper& parent, ConfigUtils::ConfigValue<T>& configValue, int decimals, double increment) {
    auto object1 = BSML::Lite::CreateIncrementSetting(parent, configValue.GetName() + " X", decimals, increment, configValue.GetValue().x,
        [&configValue](float value) {
            auto newValue = configValue.GetValue();
            newValue.x = value;
            configValue.SetValue(newValue);
        }
    );
    SetButtons(object1);
    if(!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object1, configValue.GetHoverHint());
    auto object2 = BSML::Lite::CreateIncrementSetting(parent, configValue.GetName() + " Y", decimals, increment, configValue.GetValue().y,
        [&configValue](float value) {
            auto newValue = configValue.GetValue();
            newValue.y = value;
            configValue.SetValue(newValue);
        }
    );
    SetButtons(object2);
    if(!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object2, configValue.GetHoverHint());
    return {object1, object2};
}

template<class T>
requires std::is_same_v<T, ::UnityEngine::Vector3> || std::is_same_v<T, ::UnityEngine::Vector4>
inline std::array<BSML::IncrementSetting*, 3> AddConfigValueIncrementVector3(const BSML::Lite::TransformWrapper& parent, ConfigUtils::ConfigValue<T>& configValue, int decimals, double increment) {
    auto objects = AddConfigValueIncrementVector2(parent, configValue, decimals, increment);
    auto object = BSML::Lite::CreateIncrementSetting(parent, configValue.GetName() + " Z", decimals, increment, configValue.GetValue().z,
        [&configValue](float value) { \
            auto newValue = configValue.GetValue(); \
            newValue.z = value; \
            configValue.SetValue(newValue); \
        }
    );
    SetButtons(object);
    if(!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return {objects[0], objects[1], object};
}

inline std::array<::BSML::IncrementSetting*, 4> AddConfigValueIncrementVector4(const BSML::Lite::TransformWrapper& parent, ConfigUtils::ConfigValue<::UnityEngine::Vector4>& configValue, int decimals, double increment) {
    auto objects = AddConfigValueIncrementVector3(parent, configValue, decimals, increment);
    auto object = BSML::Lite::CreateIncrementSetting(parent, configValue.GetName() + " W", decimals, increment, configValue.GetValue().w,
        [&configValue](float value) { \
            auto newValue = configValue.GetValue(); \
            newValue.w = value; \
            configValue.SetValue(newValue); \
        }
    );
    SetButtons(object);
    if(!configValue.GetHoverHint().empty())
        BSML::Lite::AddHoverHint(object, configValue.GetHoverHint());
    return {objects[0], objects[1], objects[2], object};
}

#endif
#pragma endregion
