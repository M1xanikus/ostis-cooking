#include "cooking_module.hpp"
#include "agents/translation_in_JSON_agent.hpp"

SC_MODULE_REGISTER(CookingAgentModule)
   ->Agent<TranslationInJSONAgent>();