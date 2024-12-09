#pragma once

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_agent.hpp>

class EditRecipeAgent : public ScActionInitiatedAgent
{
public:
    ScAddr GetActionClass() const override;

    ScResult DoProgram(ScAction & action) override;
    
    ScAddr FindRecipe(std::string recipe_name);

    std::vector<std::string> ListOfIngredients(ScAddr & structure);

    void ViewVector(std::vector<std::string> vector);

    std::vector<std::string> ListOfTools(ScAddr & structure);

    void CreateStep(ScAddr & structure, ScAddr & structure2, std::vector<std::string> ingredients, std::vector<std::string> tools);

    ScAddr FindFirstStep(ScAddr & structure);

    bool isNextStep(ScAddr & structure);

    bool isPreviousStep(ScAddr & structure);

    void DeleteStep(ScAddr & structure);
    
    std::string ReplaceSpacesWith_(std::string str);

    std::string GetAction(ScAddr & structure);

    //void ChangeIngredient(ScAddr & structure, std::vector<std::string> ingredients);

    void SetNextStep(ScAddr & structure);

    //ScAddr EditRecipe(ScAddr & structure, std::vector<std::string> tools, std::vector<std::string> ingredients);

    ScAddr CreateNamedNode(std::string nameOfNode, ScType const &nodeType);
};