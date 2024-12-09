#pragma once
#include<iostream>
#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_agent.hpp>
#include "keynodes/create_recipe_keynodes.hpp"
class CreateRecipeAgent : public ScActionInitiatedAgent
{
public:
    ScAddr GetActionClass() const override;
    ScResult DoProgram(ScAction & action) override;
   // ScAddr SearchIngredient(std::string ingredient_name);
    ScAddr CreateNamedNode(std::string nameOfNode, ScType const & nodeType);
    void FillList(std::vector<std::string> & list);
    void AddConceptsIngrORTool(std::vector<std::string> & list, ScAddr const & nrel_keynode,ScStructure & main_structure, ScStructure & recipe_structure );
    std::string ReplaceSpacesWith_(std::string str);
    ScAddr CreateStep(ScStructure &recipe_structure,std::vector<std::string> & ingr_list, std::vector<std::string> & tools_list);
    
};
