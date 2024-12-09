#include "cooking_module.hpp"

#include "agents/erase_recipe_agent.hpp"


SC_MODULE_REGISTER(CookingAgentModule)
->Agent<EraseRecipeAgent>();
 

