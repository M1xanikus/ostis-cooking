#pragma once

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_agent.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "json.hpp"


class TranslationInJSONAgent : public ScActionInitiatedAgent
{
public:
    struct Ingredient;

    ScAddr GetActionClass() const override;

    ScResult DoProgram(ScAction & action) override;

    std::string Replace_WithSpaces(std::string str);

    std::string RemoveRedundant(std::string str);

    int ChangeToDigit(std::string str);

    ScAddr FindRecipe(std::string recipe_name);

    ScAddr FindFirstStep(ScAddr & structure);

    bool IsNextStep(ScAddr & structure);

    ScAddr SetNextStep(ScAddr & structure);

    std::string GetUnit(ScAddr & structure);

    std::string GetQuantity(ScAddr & structure);

    nlohmann::json GetStep(ScAddr & structure, const std::vector<TranslationInJSONAgent::Ingredient>& ingredients);

    std::vector<Ingredient> GetListOfIngredients(ScAddr & structure);

    std::vector<std::string> GetListOfCookwares(ScAddr & structure);

    std::string GetProteins(ScAddr & structure);

    std::string GetFats(ScAddr & structure);

    std::string GetCarbohydrates(ScAddr & structure);

    std::string GetCalories(ScAddr & structure);

    std::string GetServings(ScAddr & structure);

    std::string GetTotalTime(ScAddr & structure);

    std::string GetAction(ScAddr & structure);

    std::string GetCondition(ScAddr & structure);

    std::string GetCookware(ScAddr & structure);
};

struct TranslationInJSONAgent::Ingredient {
    public:
        std::string name;
        double quantity;
        std::string unit;
    };