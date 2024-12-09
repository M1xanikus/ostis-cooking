#pragma once

#include <sc-memory/sc_keynodes.hpp>

class CreateRecipeKeynodes : public ScKeynodes
{
public:
    static inline ScKeynode const action_create_recipe{"action_create_recipe", ScType::ConstNodeClass};
    static inline ScKeynode const lang_en{"lang_en",ScType::ConstNodeClass};
    static inline ScKeynode const concept_recipe{"concept_recipe",ScType::ConstNodeClass};
    static inline ScKeynode const rrel_dynamic_argument{"rrel_dynamic_argument",ScType::ConstNodeRole};
    static inline ScKeynode const nrel_goto{"nrel_goto",ScType::ConstNodeNonRole};
    static inline ScKeynode const nrel_tools{"nrel_tools",ScType::ConstNodeNonRole};
    static inline ScKeynode const nrel_ingredients{"nrel_ingredients",ScType::ConstNodeNonRole};
    static inline ScKeynode const nrel_decomposition_of_action{"nrel_decomposition_of_action",ScType::ConstNodeNonRole};
};
