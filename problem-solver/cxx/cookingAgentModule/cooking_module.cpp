#include "cooking_module.hpp"
#include "agents/edit_recipe_agent.hpp"

SC_MODULE_REGISTER(CookingAgentModule)
   ->Agent<EditRecipeAgent>();