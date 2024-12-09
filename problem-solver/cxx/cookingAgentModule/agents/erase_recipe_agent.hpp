#pragma once
#include <iostream>
#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_agent.hpp>
#include <keynodes/erase_recipe_keynodes.hpp>
#include <keynodes/create_recipe_keynodes.hpp>
class EraseRecipeAgent : public ScActionInitiatedAgent
{
public:
   ScAddr GetActionClass() const override;
   ScResult DoProgram(ScAction & action) override;  
   ScAddr CreateNamedNode(std::string nameOfNode, ScType const & nodeType);
   ScAddr EraseRecipe(std::string recipe_name, ScStructure & structure);
};