#pragma once
#include<iostream>
#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_agent.hpp>

class CreateIngredientAgent : public ScActionInitiatedAgent
{
public:
    ScAddr GetActionClass() const override;

    ScResult DoProgram(ScAction & action) override;

    ScAddr CreateNamedNode(std::string nameOfNode_en, ScType const & nodeType, ScStructure & structure);

    ScAddr SearchUnitOfMeasurement(std::string nameOfUnitOfMeasurement);

    bool CheckForFullIngr(ScAddr & ingr_node,ScStructure& structure);
    
    ScAddr CreateUnitOfMeasurement(std::string nameOfUnitOfMeasurement);

    ScAddr SearchForIngredient(std::string nameOfIngredient);
};


