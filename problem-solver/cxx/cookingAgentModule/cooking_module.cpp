#include "cooking_module.hpp"
#include "agents/create_dish_agent.hpp"
#include "agents/create_ingredient_agent.hpp"
#include "agents/create_recipe_agent.hpp"
#include "agents/translation_in_JSON_agent.hpp"
#include "agents/edit_recipe_agent.hpp"
#include "agents/erase_recipe_agent.hpp"
SC_MODULE_REGISTER(CookingAgentModule)
   ->Agent<CreateRecipeAgent>()
   ->Agent<CreateIngredientAgent>()
   ->Agent<CreateDishAgent>()
   ->Agent<TranslationInJSONAgent>()
   ->Agent<EraseRecipeAgent>()
   ->Agent<EditRecipeAgent>();
