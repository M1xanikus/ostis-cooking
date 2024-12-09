#include "create_ingredient_agent.hpp"
#include <string>
#include "keynodes/create_ingredient_keynodes.hpp"
ScAddr CreateIngredientAgent::GetActionClass() const
{
    return CreateIngredientKeynodes::action_create_ingredient;
};
ScAddr CreateIngredientAgent::CreateNamedNode(std::string nameOfNode_en, ScType const &nodeType, ScStructure & structure)
{
    ScAddr const & node = m_context.ResolveElementSystemIdentifier(nameOfNode_en, nodeType);
    
    structure << node;
    return node;
}

ScAddr CreateIngredientAgent::SearchUnitOfMeasurement(std::string nameOfUnitOfMeasurement)
{   
    
     ScTemplate searchLinksTemplate;
    searchLinksTemplate.Triple(
    CreateIngredientKeynodes::concept_unit_of_measurement,
    ScType::VarPermPosArc,
    ScType::VarNode >> "_parameter");
    
    searchLinksTemplate.Quintuple(
    "_parameter",
    ScType::VarCommonArc,
    ScType::VarNodeLink >> "_idtf_link",
    ScType::VarPermPosArc,
    ScKeynodes::nrel_main_idtf);

    searchLinksTemplate.Triple(
    CreateIngredientKeynodes::lang_en,
    ScType::VarPermPosArc,
    "_idtf_link");

    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchLinksTemplate, result);
    for (size_t i = 0; i < result.Size(); ++i)
    {
        auto const & construction = result[i];
        ScAddr const & idtfLink = construction["_idtf_link"];
        std::string idtf;
        m_context.GetLinkContent(idtfLink, idtf);

        if(idtf == nameOfUnitOfMeasurement)
        { 
            SC_AGENT_LOG_DEBUG("Found parameter: " << idtf);
            return construction["_parameter"];
        }
    }
    return ScAddr::Empty;
}
bool CreateIngredientAgent::CheckForFullIngr(ScAddr & ingr_node,ScStructure& structure)
{
    ScTemplate searchConstrTemplate;
    searchConstrTemplate.Quintuple(
        ingr_node,
        ScType::VarCommonArc,
        ScType::VarNode >>"_unit_of_measure",
        ScType::VarPermPosArc,
        CreateIngredientKeynodes::nrel_unit_of_measurement 
    );
    searchConstrTemplate.Quintuple(
        "_unit_of_measure",
        ScType::VarCommonArc,
        ScType::VarNodeLink >> "_idtf_link",
        ScType::VarPermPosArc,
        ScKeynodes::nrel_main_idtf
    );
    searchConstrTemplate.Triple(
    CreateIngredientKeynodes::lang_en,
    ScType::VarPermPosArc,
    "_idtf_link");

    ScTemplateSearchResult result;
    if( m_context.SearchByTemplate(searchConstrTemplate, result))
    {
        structure << result[0];
        return true;
    }
    else
        return false;
    
}

ScAddr CreateIngredientAgent::CreateUnitOfMeasurement(std::string nameOfUnitOfMeasurement)
{
    ScAddr unit_of_measurement = m_context.ResolveElementSystemIdentifier(nameOfUnitOfMeasurement,ScType::ConstNode);
    m_context.GenerateConnector(ScType::ConstPermPosArc,CreateIngredientKeynodes::concept_unit_of_measurement,unit_of_measurement);
    return unit_of_measurement;
}

ScAddr CreateIngredientAgent::SearchForIngredient(std::string nameOfIngredient)
{
     ScTemplate searchLinksTemplate;
    searchLinksTemplate.Triple(
    ScType::VarNodeClass >> "concept",
    ScType::VarPermPosArc,
    ScType::VarNode >> "_parameter");

     searchLinksTemplate.Quintuple(
    "_parameter",
    ScType::VarCommonArc,
    ScType::VarNodeLink >> "_idtf_link",
    ScType::VarPermPosArc,
    ScKeynodes::nrel_main_idtf);

    searchLinksTemplate.Triple(
    CreateIngredientKeynodes::lang_en,
    ScType::VarPermPosArc,
    "_idtf_link");

    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchLinksTemplate, result);
 
    for (size_t i= 0; i < result.Size(); ++i)
    {
        auto const & construction = result[i];
        ScAddr const & idtfLink = construction["_idtf_link"];
        std::string idtf;
        m_context.GetLinkContent(idtfLink, idtf);
        if (idtf == nameOfIngredient)
            return construction["_parameter"];
        
    }
    return ScAddr::Empty;
    

}

ScResult CreateIngredientAgent::DoProgram(ScAction &action)
{
    // Переменные для имени ингредиента и единицы измерения
    std::string ingredient_name_str, unit_of_measurement_str;
    ScStructure structure = m_context.GenerateStructure();

    // Запрос на ввод имени ингредиента
    std::cout << "\nEnter an ingredient name:";
    getline(std::cin, ingredient_name_str);

    // Поиск ингредиента по введенному имени
    ScAddr check_ingr = SearchForIngredient(ingredient_name_str);

    if (check_ingr != ScAddr::Empty)
    {
        // Если ингредиент найден, запрос на ввод единицы измерения
        std::cout << "\nEnter a unit of measurement:";
        getline(std::cin, unit_of_measurement_str);

        // Поиск указанной единицы измерения
        ScAddr const &unit_of_measurement = SearchUnitOfMeasurement(unit_of_measurement_str);

        if (unit_of_measurement != ScAddr::Empty)
        {
            // Проверка на существование полной информации об ингредиенте
            if (CheckForFullIngr(check_ingr, structure))
            {
                action.SetResult(structure);
                return action.FinishSuccessfully();
            }
            else
            {
                // Создание дуги между ингредиентом и найденной единицей измерения
                ScAddr const &arc_from_ingredient = m_context.GenerateConnector(
                    ScType::ConstCommonArc, check_ingr, unit_of_measurement);

                // Создание дуги между отношением и дугой от ингредиента
                ScAddr const &arc_for_arc_from_ingredient = m_context.GenerateConnector(
                    ScType::ConstPermPosArc, CreateIngredientKeynodes::nrel_unit_of_measurement, arc_from_ingredient);

                // Добавление узлов и дуг в структуру
                structure << check_ingr << unit_of_measurement << arc_from_ingredient
                          << CreateIngredientKeynodes::nrel_unit_of_measurement << arc_for_arc_from_ingredient;

                action.SetResult(structure);
                return action.FinishSuccessfully();
            }
        }
        else
        {
            // Если единица измерения не найдена, создаем новую
            ScAddr const &new_unit_of_measurement = CreateUnitOfMeasurement(unit_of_measurement_str);

            // Создание дуги между ингредиентом и новой единицей измерения
            ScAddr const &arc_from_ingredient = m_context.GenerateConnector(
                ScType::ConstCommonArc, check_ingr, new_unit_of_measurement);

            // Создание дуги между отношением и дугой от ингредиента
            ScAddr const &arc_for_arc_from_ingredient = m_context.GenerateConnector(
                ScType::ConstPermPosArc, CreateIngredientKeynodes::nrel_unit_of_measurement, arc_from_ingredient);

            // Добавление узлов и дуг в структуру
            structure << check_ingr << new_unit_of_measurement << arc_from_ingredient
                      << CreateIngredientKeynodes::nrel_unit_of_measurement << arc_for_arc_from_ingredient;

            action.SetResult(structure);
            return action.FinishSuccessfully();
        }
    }
    else
    {
        // Если ингредиент не найден, создаем новый узел для ингредиента
        ScAddr const &ingredient = CreateNamedNode(ingredient_name_str, ScType::ConstNode, structure);

        // Создание концепта ингредиента
        ScAddr const &concept_ingredient = CreateNamedNode("concept_" + ingredient_name_str, ScType::ConstNodeClass, structure);

        // Создание дуги между концептом и ингредиентом
        ScAddr const &arc_from_class = m_context.GenerateConnector(
            ScType::ConstPermPosArc, concept_ingredient, ingredient);

        // Запрос на ввод единицы измерения
        std::cout << "\nEnter a unit of measurement:";
        getline(std::cin, unit_of_measurement_str);

        // Поиск указанной единицы измерения
        ScAddr const &unit_of_measurement = SearchUnitOfMeasurement(unit_of_measurement_str);

        if (unit_of_measurement != ScAddr::Empty)
        {
            // Создание дуги между ингредиентом и найденной единицей измерения
            ScAddr const &arc_from_ingredient = m_context.GenerateConnector(
                ScType::ConstCommonArc, ingredient, unit_of_measurement);

            // Создание дуги между отношением и дугой от ингредиента
            ScAddr const &arc_for_arc_from_ingredient = m_context.GenerateConnector(
                ScType::ConstPermPosArc, CreateIngredientKeynodes::nrel_unit_of_measurement, arc_from_ingredient);

            // Добавление узлов и дуг в структуру
            structure << arc_from_class << unit_of_measurement << arc_from_ingredient
                      << CreateIngredientKeynodes::nrel_unit_of_measurement << arc_for_arc_from_ingredient;

            action.SetResult(structure);
            return action.FinishSuccessfully();
        }
        else
        {
            // Если единица измерения не найдена, создаем новую
            ScAddr const &new_unit_of_measurement = CreateUnitOfMeasurement(unit_of_measurement_str);

            // Создание дуги между ингредиентом и новой единицей измерения
            ScAddr const &arc_from_ingredient = m_context.GenerateConnector(
                ScType::ConstCommonArc, ingredient, new_unit_of_measurement);

            // Создание дуги между отношением и дугой от ингредиента
            ScAddr const &arc_for_arc_from_ingredient = m_context.GenerateConnector(
                ScType::ConstPermPosArc, CreateIngredientKeynodes::nrel_unit_of_measurement, arc_from_ingredient);

            // Добавление узлов и дуг в структуру
            structure << new_unit_of_measurement << arc_from_ingredient
                      << CreateIngredientKeynodes::nrel_unit_of_measurement << arc_for_arc_from_ingredient;

            action.SetResult(structure);
            return action.FinishSuccessfully();
        }
    }
}


