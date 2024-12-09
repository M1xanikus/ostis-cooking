#include "cooking_module.hpp"

#include "agents/create_ingredient_agent.hpp"
#include "agents/create_recipe_agent.hpp"
SC_MODULE_REGISTER(CookingAgentModule)
   ->Agent<CreateRecipeAgent>()
   ->Agent<CreateIngredientAgent>();