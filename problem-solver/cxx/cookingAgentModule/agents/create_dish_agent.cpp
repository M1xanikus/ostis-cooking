#include "create_dish_agent.hpp"
#include <string>
#include "keynodes/create_dish_keynodes.hpp"
#include <iostream>


ScAddr CreateDishAgent::GetActionClass() const
{
    return CreateDishKeynodes::action_create_dish;
};


ScAddr CreateDishAgent::CreateNamedNode(std::string nameOfNode_en, ScType const &nodeType)
{
    ScAddr const & node = m_context.ResolveElementSystemIdentifier(nameOfNode_en, nodeType);
    
    return node;
}

std::string CreateDishAgent::ReplaceSpacesWith_(std::string str) 
{
    for (size_t i = 0; i < str.length(); i++)
    {
        if(str[i] == ' ')
            str[i] = '_';
    }
    return str;
}

ScAddr CreateDishAgent::SearchForDish(std::string nameOfDish)
{
     ScTemplate searchLinksTemplate;

    searchLinksTemplate.Triple(
    CreateDishKeynodes::lang_en,
        ScType::PermPosArc,
        ScType::NodeLink >> "_link");

     searchLinksTemplate.Quintuple(
    ScType::NodeClass >> "concept",
    ScType::CommonArc,
    "_link",
    ScType::PermPosArc,
    ScKeynodes::nrel_main_idtf);

    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchLinksTemplate, result);
 
    for (size_t i= 0; i < result.Size(); ++i)
    {
        auto const & construction = result[i];
        ScAddr const & idtfLink = construction["_link"];
        std::string idtf;
        m_context.GetLinkContent(idtfLink, idtf);
        if (idtf == nameOfDish)
            return construction["concept"];
        
    }
    return ScAddr::Empty;
    

}

ScAddr CreateDishAgent::FindRecipe(std::string name_of_recipe){
    ScTemplate searchlinksTemplate;
    searchlinksTemplate.Triple(
        CreateDishKeynodes::lang_en,
        ScType::PermPosArc,
        ScType::NodeLink >> "_link");

    searchlinksTemplate.Quintuple(
        ScType::NodeStructure >> "_struct",
        ScType::CommonArc,
        "_link",
        ScType::PermPosArc,
        ScKeynodes::nrel_main_idtf);
    
    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchlinksTemplate, result);
    std::cout << result.Size();
    for (size_t i = 0; i < result.Size(); ++i)
    {
        auto const & construction = result[i];
        ScAddr const & idtfLink = construction["_link"];
        std::string recipe_idtf;
        m_context.GetLinkContent(idtfLink, recipe_idtf);
        if(recipe_idtf == name_of_recipe)
        { 
            return construction["_struct"];
        }
    }
    std::cout << "er3";
    return ScAddr::Empty;
}

ScResult CreateDishAgent::DoProgram(ScAction &action)
{ 
    // Ввод данных о блюде
    int amount_proteins, amount_fats, amount_carbohydrates, cooking_time, portion_number, calorie_content;
    std::string dish_name;
    std::cout << "\nEnter the dish name in English:";
    getline(std::cin, dish_name);
    std::cout << dish_name;

    // Проверка существования блюда
    ScAddr check_dish = SearchForDish(dish_name);

    ScStructure structure = m_context.GenerateStructure();

    if (check_dish != ScAddr::Empty){
        //Если блюдо уже существует
        ScAddr const & message = m_context.GenerateLink(ScType::ConstNodeLink);
        m_context.SetLinkContent(message, "the dish " + dish_name + " already exists");
        structure << message;
        action.SetResult(structure);
        // Завершение программы с не успешным результатом
        return action.FinishUnsuccessfully();
    }
    else{
    // Сбор информации о блюде
    std::cout << "\nEnter the amount of protein in gram:";
    std::cin >> amount_proteins;

    std::cout << "\nEnter the amount of fat in gram:";
    std::cin >> amount_fats;

    std::cout << "\nEnter the amount of carbohydrates in gram:";
    std::cin >> amount_carbohydrates;

    std::cout << "\nEnter cooking time in minutes:";
    std::cin >> cooking_time;

    std::cout << "\nEnter the number of servings:";
    std::cin >> portion_number;

    std::cout << "\nEnter the calorie content in kilocalories:";
    std::cin >> calorie_content;

    //Создание узла блюда
    ScAddr const & concept_dish = CreateNamedNode("concept_"+ ReplaceSpacesWith_(dish_name), ScType:: ConstNodeClass);
    structure << concept_dish;

    // Создание узлов для атрибутов блюда
    ScAddr const & proteins_g = CreateNamedNode(std::to_string(amount_proteins)+"_g", ScType:: ConstNode);
    ScAddr const & fats_g = CreateNamedNode(std::to_string(amount_fats)+"_g", ScType:: ConstNode);
    ScAddr const & carbohydrates_g = CreateNamedNode(std::to_string(amount_carbohydrates)+"_g", ScType:: ConstNode);
    ScAddr const & cooking_time_min = CreateNamedNode(std::to_string(cooking_time)+"_min", ScType:: ConstNode);
    ScAddr const & portion_num = CreateNamedNode(std::to_string(portion_number)+"_portion", ScType:: ConstNode);
    ScAddr const & calorie_cont = CreateNamedNode(std::to_string(calorie_content)+"_kcal", ScType:: ConstNode);
    structure << proteins_g << fats_g << carbohydrates_g << cooking_time_min << portion_num << calorie_cont;

    // Добавление дуг между узлами
    ScAddr const &arc_for_proteins = m_context.GenerateConnector(ScType::ConstCommonArc, concept_dish, proteins_g);   
    ScAddr const &arc_for_fats = m_context.GenerateConnector(ScType::ConstCommonArc, concept_dish, fats_g);   
    ScAddr const &arc_for_carbohydrates = m_context.GenerateConnector(ScType::ConstCommonArc, concept_dish, carbohydrates_g);   
    ScAddr const &arc_for_cooking_time = m_context.GenerateConnector(ScType::ConstCommonArc, concept_dish, cooking_time_min);   
    ScAddr const &arc_for_portion_number = m_context.GenerateConnector(ScType::ConstCommonArc, concept_dish, portion_num);   
    ScAddr const &arc_for_calorie_content = m_context.GenerateConnector(ScType::ConstCommonArc, concept_dish, calorie_cont);  
    structure << arc_for_proteins << arc_for_fats << arc_for_carbohydrates << arc_for_cooking_time << arc_for_portion_number << arc_for_calorie_content;

    // Добавление дуг связывающих соответствующие дуги и ключевые узлы
    ScAddr const &arc_for_arc_from_protein = m_context.GenerateConnector(
        ScType::ConstPermPosArc, CreateDishKeynodes::nrel_proteins, arc_for_proteins);
    ScAddr const &arc_for_arc_from_fat = m_context.GenerateConnector(
        ScType::ConstPermPosArc, CreateDishKeynodes::nrel_fats, arc_for_fats);
    ScAddr const &arc_for_arc_from_carbohydrates = m_context.GenerateConnector(
        ScType::ConstPermPosArc, CreateDishKeynodes::nrel_carbohydrates, arc_for_carbohydrates);
    ScAddr const &arc_for_arc_from_cooking_time = m_context.GenerateConnector(
        ScType::ConstPermPosArc, CreateDishKeynodes::nrel_cooking_time, arc_for_cooking_time);
    ScAddr const &arc_for_arc_from_portion_number = m_context.GenerateConnector(
        ScType::ConstPermPosArc, CreateDishKeynodes::nrel_portion_number, arc_for_portion_number);
    ScAddr const &arc_for_arc_from_calorie_content = m_context.GenerateConnector(
        ScType::ConstPermPosArc, CreateDishKeynodes::nrel_calorie_content, arc_for_calorie_content);
    structure << arc_for_arc_from_protein << arc_for_arc_from_fat << arc_for_arc_from_carbohydrates << arc_for_arc_from_cooking_time 
        << arc_for_arc_from_portion_number << arc_for_arc_from_calorie_content;

    ScAddr const &name_of_the_system_dish_link = m_context.GenerateLink(ScType::ConstNodeLink);
        m_context.SetLinkContent(name_of_the_system_dish_link, "concept_"+ ReplaceSpacesWith_(dish_name));
    ScAddr const &arc_for_syst_link_from_dish = m_context.GenerateConnector(
        ScType::ConstCommonArc, concept_dish, name_of_the_system_dish_link);
    ScAddr const &arc_for_arc_Sfrom_syst_idtf = m_context.GenerateConnector(
        ScType::ConstPermPosArc, ScKeynodes::nrel_system_identifier, arc_for_syst_link_from_dish);
    structure << name_of_the_system_dish_link << arc_for_syst_link_from_dish << arc_for_arc_from_syst_idtf;

    // Добавление системного идентификатора и основного идентификатора
    ScAddr const &name_of_the_main_idtf_dish_link = m_context.GenerateLink(ScType::ConstNodeLink);
        m_context.SetLinkContent(name_of_the_main_idtf_dish_link, dish_name);
    ScAddr const &arc_for_main_link_from_lang = m_context.GenerateConnector(
        ScType::ConstPermPosArc, CreateDishKeynodes::lang_en, name_of_the_main_idtf_dish_link);
    ScAddr const &arc_for_main_link_from_dish = m_context.GenerateConnector(
        ScType::ConstCommonArc, concept_dish, name_of_the_main_idtf_dish_link);
    ScAddr const &arc_for_arc_from_main_identifier = m_context.GenerateConnector(
        ScType::ConstPermPosArc, ScKeynodes::nrel_main_idtf, arc_for_main_link_from_dish);
    structure << name_of_the_main_idtf_dish_link << arc_for_main_link_from_lang << arc_for_main_link_from_dish << arc_for_arc_from_main_identifier;
    
    // Проверка на существование рецепта
    ScAddr const &recipe = FindRecipe(dish_name);
    if(recipe != ScAddr::Empty){
        ScAddr const &arc_for_recipe = m_context.GenerateConnector(ScType::ConstCommonArc, concept_dish, recipe);
        ScAddr const &arc_for_arc_from_recipe = m_context.GenerateConnector(ScType::ConstPermPosArc, CreateDishKeynodes::nrel_recipe, arc_for_recipe);
        structure << arc_for_recipe << arc_for_arc_from_recipe;
    }
    else
        // Завершение программы с не успешным результатом
        return action.FinishUnsuccessfully();
    
    // Завершение программы с успешным результатом
    action.SetResult(structure);
    return action.FinishSuccessfully();
    }
}