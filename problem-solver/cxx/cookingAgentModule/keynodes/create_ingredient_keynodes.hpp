#pragma once

#include <sc-memory/sc_keynodes.hpp>

class CreateIngredientKeynodes : public ScKeynodes
{
public:
    static inline ScKeynode const action_create_ingredient{"action_create_ingredient", ScType::ConstNodeClass};
    static inline ScKeynode const action_finish_creating_because_of_existing_ingr{"action_finish_creating_because_of_existing_ingr", ScType::ConstNodeClass};
    static inline ScKeynode const nrel_unit_of_measurement{"nrel_unit_of_measurement",ScType::ConstNodeNonRole};
    static inline ScKeynode const lang_ru{"lang_ru",ScType::ConstNodeClass};
    static inline ScKeynode const lang_en{"lang_en",ScType::ConstNodeClass};
    static inline ScKeynode const concept_unit_of_measurement{"concept_unit_of_measurement",ScType::ConstNodeClass};
};
