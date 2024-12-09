#pragma once 

#include <sc-memory/sc_keynodes.hpp>

class EraseRecipeKeynodes : public ScKeynodes
{
public:
    static inline ScKeynode const action_erase_recipe{"action_erase_recipe", ScType::ConstNodeClass};
    static inline ScKeynode const lang_en{"lang_en",ScType::ConstNodeClass};
  
};