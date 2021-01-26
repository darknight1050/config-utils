#pragma once
#include "beatsaber-hook/shared/utils/utils.h"
#include "beatsaber-hook/shared/config/config-utils.hpp"

#define DEFINE_CONFIG(name) \
name##_t& get##name() { \
    static name##_t name; \
    return name; \
} \

#define DECLARE_CONFIG(name, impl) \
struct name##_t { \
    Configuration* config = nullptr; \
    impl \
}; \
name##_t& get##name();

#define DECLARE_VALUE(name, type, ...) \
ConfigUtils::ConfigValue<type> name = ConfigUtils::ConfigValue<type>(__VA_ARGS__);

#define INIT_FUNCTION(impl) \
void Init(const ModInfo info) { \
    if(config) \
        delete config; \
    config = new Configuration(info); \
    config->Load(); \
    impl \
}
#define INIT_VALUE(name) \
name.Init(config);

#ifdef HAS_CODEGEN
#define AddConfigValueToggle(parent, boolConfigValue) \
QuestUI::BeatSaberUI::CreateToggle(parent, boolConfigValue.GetName(), boolConfigValue.GetValue(), \
    il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction_1<bool>*>(classof(UnityEngine::Events::UnityAction_1<bool>*), (void*)nullptr, \
    +[](bool toggle) { \
        boolConfigValue.SetValue(toggle); \
    }))

#define AddConfigValueIncrementInt(parent, intConfigValue, increment, min, max) \
BeatSaberUI::CreateIncrementSetting(parent, intConfigValue.GetName(), 0, increment, intConfigValue.GetValue(), min, max, \
    il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction_1<float>*>(classof(UnityEngine::Events::UnityAction_1<float>*), (void*)nullptr, \
    +[](float value) { \
        intConfigValue.SetValue((int)value); \
    }))

#define AddConfigValueIncrementFloat(parent, floatConfigValue, decimal, increment, min, max) \
BeatSaberUI::CreateIncrementSetting(parent, floatConfigValue.GetName(), decimal, increment, floatConfigValue.GetValue(), min, max, \
    il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction_1<float>*>(classof(UnityEngine::Events::UnityAction_1<float>*), (void*)nullptr, \
    +[](float value) { \
        floatConfigValue.SetValue(value); \
    }))

#define AddConfigValueStringSetting(parent, stringConfigValue) \
BeatSaberUI::CreateStringSetting(parent, stringConfigValue.GetName(), stringConfigValue.GetValue(), \
    il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction_1<Il2CppString*>*>(classof(UnityEngine::Events::UnityAction_1<Il2CppString*>*), (void*)nullptr, \
    +[](Il2CppString* value) { \
        stringConfigValue.SetValue(to_utf8(csstrtostr(value))); \
    }))

#endif

namespace ConfigUtils {

    inline Logger& getLogger() {
        static auto logger = new Logger(ModInfo{"config-utils", "0.2.0"});
        return *logger;
    }
    
    template <typename ValueType> 
    class ConfigValue {
        private:
            std::string name;
            ValueType value;
            ValueType defaultValue;
            bool autoSave;
            Configuration* config;

        public:
            ConfigValue(std::string name, ValueType defaultValue, bool autoSave) {
                this->name = name;
                this->defaultValue = defaultValue;
                this->autoSave = autoSave;
                this->config = nullptr;
            }

            ConfigValue(std::string name, ValueType defaultValue) : ConfigValue(name, defaultValue, true) {}

            void Init(Configuration* cfg) {
                this->config = cfg;
                if(config->config.HasMember(name)) {
                    getLogger().info("ConfigValue: Loading %s", name.c_str());
                    LoadValue();
                } else {
                    getLogger().info("ConfigValue: Creating %s", name.c_str());
                    if(config->config.ObjectEmpty()) 
                        config->config.SetObject();
                    auto& allocator = config->config.GetAllocator();
                    config->config.AddMember(rapidjson::Value(name.c_str(), allocator).Move(), rapidjson::Value(), allocator);
                    SetValue(defaultValue);
                    if(!autoSave)
                        SaveValue();
                }
                GetValue();
            }

            void LoadValue();
            void SaveValue();

            ValueType GetValue() {
                return value;
            }

            void SetValue(ValueType value) {
                this->value = value;
                if(autoSave)
                    SaveValue();
            }

            ValueType GetDefaultValue() {
                return defaultValue;
            }

            std::string GetName() {
                return name;
            }
    };
   
}

#pragma region SimpleMacrosDefine
#define SIMPLE_SAVE(type, typeName) \
template<> \
inline void ConfigUtils::ConfigValue<type>::SaveValue() { \
    config->config[name].Set##typeName(value); \
    config->Write(); \
}

#define SIMPLE_LOAD(type, typeName) \
template<> \
inline void ConfigUtils::ConfigValue<type>::LoadValue() { \
    if(config->config[name].Is##typeName()){ \
        value = config->config[name].Get##typeName(); \
    } else { \
        SetValue(defaultValue); \
        if(!autoSave) \
            SaveValue(); \
    } \
}

#define SIMPLE_VALUE(type, typeName) \
SIMPLE_SAVE(type, typeName) \
SIMPLE_LOAD(type, typeName)

#pragma endregion

SIMPLE_VALUE(bool, Bool)

SIMPLE_VALUE(int, Int)

SIMPLE_VALUE(float, Float)

SIMPLE_VALUE(double, Double)

#pragma region StringValues
template<>
inline void ConfigUtils::ConfigValue<char*>::SaveValue() {
    config->config[name].SetString(value, config->config.GetAllocator());
    config->Write();
}

template<>
inline void ConfigUtils::ConfigValue<char*>::LoadValue() {
    if(config->config[name].IsString()){
        value = (char*)config->config[name].GetString();
    } else {
        SetValue(defaultValue);
        if(!autoSave)
            SaveValue();
    }
    
}

template<>
inline void ConfigUtils::ConfigValue<const char*>::SaveValue() {
    config->config[name].SetString(value, config->config.GetAllocator());
    config->Write();
}

SIMPLE_LOAD(const char*, String)

template<>
inline void ConfigUtils::ConfigValue<std::string>::SaveValue() {
    config->config[name].SetString(value, config->config.GetAllocator());
    config->Write();
}

SIMPLE_LOAD(std::string, String)
#pragma endregion

#pragma region CodegenValues
#ifdef HAS_CODEGEN

#pragma region VectorValues

#pragma region VectorMacrosDefine
#define VECTOR_SAVE(type, coords) \
template<> \
inline void ConfigUtils::ConfigValue<type>::SaveValue() { \
    auto& allocator = config->config.GetAllocator(); \
    auto& object = config->config[name]; \
    if(object.ObjectEmpty()) \
        object.SetObject(); \
    coords \
    config->Write(); \
} \

#define VECTOR_LOAD(type, coords) \
template<> \
inline void ConfigUtils::ConfigValue<type>::LoadValue() { \
    value = defaultValue; \
    auto& allocator = config->config.GetAllocator(); \
    auto& object = config->config[name]; \
    if(object.ObjectEmpty()) object.SetObject(); \
    bool write = false; \
    coords \
    if(write) \
        config->Write(); \
} \

#define VECTOR_COORD_SAVE(name) \
if(object.HasMember(#name)) { \
    object[#name].SetFloat(value.name); \
} else { \
    object.AddMember(#name, value.name, allocator); \
}

#define VECTOR_COORD_LOAD(name) \
if(object.HasMember(#name)) { \
    value.name = object[#name].GetFloat(); \
} else { \
    object.AddMember(#name, value.name, allocator); \
    write = true; \
}
#pragma endregion

#pragma region Vector2Value
#include "UnityEngine/Vector2.hpp"
VECTOR_SAVE(UnityEngine::Vector2, 
    VECTOR_COORD_SAVE(x)
    VECTOR_COORD_SAVE(y)
)

VECTOR_LOAD(UnityEngine::Vector2, 
    VECTOR_COORD_LOAD(x)
    VECTOR_COORD_LOAD(y)
)
#pragma endregion

#pragma region Vector3Value
#include "UnityEngine/Vector3.hpp"
VECTOR_SAVE(UnityEngine::Vector3, 
    VECTOR_COORD_SAVE(x)
    VECTOR_COORD_SAVE(y)
    VECTOR_COORD_SAVE(z)
)

VECTOR_LOAD(UnityEngine::Vector3, 
    VECTOR_COORD_LOAD(x)
    VECTOR_COORD_LOAD(y)
    VECTOR_COORD_LOAD(z)
)
#pragma endregion

#pragma region Vector4Value
#include "UnityEngine/Vector4.hpp"
VECTOR_SAVE(UnityEngine::Vector4, 
    VECTOR_COORD_SAVE(x)
    VECTOR_COORD_SAVE(y)
    VECTOR_COORD_SAVE(z)
    VECTOR_COORD_SAVE(w)
)

VECTOR_LOAD(UnityEngine::Vector4, 
    VECTOR_COORD_LOAD(x)
    VECTOR_COORD_LOAD(y)
    VECTOR_COORD_LOAD(z)
    VECTOR_COORD_LOAD(w)
)
#pragma endregion

#pragma region VectorMacrosUndefine
#undef VECTOR_SAVE
#undef VECTOR_LOAD
#undef VECTOR_COORD_SAVE
#undef VECTOR_COORD_LOAD
#pragma endregion

#pragma endregion

#endif
#pragma endregion

#pragma region SimpleMacrosUndefine
#undef SIMPLE_SAVE
#undef SIMPLE_LOAD
#undef SIMPLE_VALUE
#pragma endregion
