#pragma once

#include <sc-memory/sc_keynodes.hpp>

class CreateDishKeynodes : public ScKeynodes
{
public:
    static inline ScKeynode const rrel_example{"rrel_example", ScType::ConstNodeRole};
    static inline ScKeynode const nrel_sc_text_translation{"nrel_sc_text_translation", ScType::ConstNodeNonRole};
    static inline ScKeynode const action_create_dish{"action_create_dish", ScType::ConstNodeClass};
    static inline ScKeynode const nrel_dish{"nrel_dish", ScType::ConstNodeNonRole};
    static inline ScKeynode const lang_en{"lang_en", ScType::ConstNodeClass};
    static inline ScKeynode const nrel_portion_number{"nrel_portion_number", ScType::ConstNodeNonRole};
    static inline ScKeynode const nrel_proteins{"nrel_protein", ScType::ConstNodeNonRole};
    static inline ScKeynode const nrel_carbohydrates{"nrel_carbohydrates", ScType::ConstNodeNonRole};
    static inline ScKeynode const nrel_fats{"nrel_fats", ScType::ConstNodeNonRole};
    static inline ScKeynode const nrel_calorie_content{"nrel_calorie_content", ScType::ConstNodeNonRole};
    static inline ScKeynode const nrel_cooking_time{"nrel_cooking_time", ScType::ConstNodeNonRole};
    static inline ScKeynode const nrel_recipe{"nrel_recipe", ScType::ConstNodeNonRole};
    
};