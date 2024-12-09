#pragma once
#include<iostream>
#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_agent.hpp>

class CreateDishAgent : public ScActionInitiatedAgent
{
public:
    ScAddr GetActionClass() const override;

    ScResult DoProgram(ScAction & action) override;

    std::string ReplaceSpacesWith_(std::string str);

    ScAddr CreateNamedNode(std::string nameOfNode_en, ScType const &nodeType);
    ScAddr FindRecipe(std::string name_of_recipe);
    ScAddr SearchForDish(std::string nameOfDish);

};