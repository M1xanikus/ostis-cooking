#pragma once

#include <sc-memory/sc_keynodes.hpp>


class TranslationKeynodes : public ScKeynodes
{
public:
  static inline ScKeynode const action_translation_in_JSON{"action_translation_in_JSON", ScType::NodeConstClass};
  static inline ScKeynode const nrel_translating{"nrel_translating", ScType::NodeConstNoRole};
  static inline ScKeynode const action_edit_recipe{"action_edit_recipe", ScType::ConstNodeClass};
  static inline ScKeynode const lang_en{"lang_en", ScType::ConstNodeClass};
  static inline ScKeynode const nrel_ingredients{"nrel_ingredients", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_tools{"nrel_tools", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_quantity{"nrel_quantity", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_recipe{"nrel_recipe", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_portion_number{"nrel_portion_number", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_proteins{"nrel_protein", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_carbohydrates{"nrel_carbohydrates", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_fats{"nrel_fats", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_calorie_content{"nrel_calorie_content", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_cooking_time{"nrel_cooking_time", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_goto{"nrel_goto", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_unit_of_measurement{"nrel_unit_of_measurement", ScType::ConstNodeNonRole};
};