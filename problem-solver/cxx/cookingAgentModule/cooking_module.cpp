#include "cooking_module.hpp"
#include "agents/create_dish_agent.hpp"

SC_MODULE_REGISTER(CookingAgentModule)
   ->Agent<CreateDishAgent>();
