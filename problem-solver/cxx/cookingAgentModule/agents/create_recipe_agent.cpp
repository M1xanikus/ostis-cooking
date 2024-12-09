#include "create_recipe_agent.hpp"
#include <string>
#include <vector>
#include "keynodes/create_ingredient_keynodes.hpp"
ScAddr CreateRecipeAgent::GetActionClass() const
{
    return CreateRecipeKeynodes::action_create_recipe;
}
/*ScAddr CreateStepAgent::SearchIngredient(std::string ingredient_name)
{   
    ScAddr const & concept_ingredient = CreateNamedNode("concept_"+ingredient_name,ScType::ConstNodeClass);
     ScTemplate searchLinksTemplate;
//template for all ingredient structure
    searchLinksTemplate.Triple(
    concept_ingredient >> "concept_name_of_ingr",
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

    searchLinksTemplate.Quintuple(
    "_parameter",
    ScType::VarCommonArc,
    ScType::VarNode >> "_unit_of_measure",
    ScType::VarPermPosArc,
    CreateIngredientKeynodes::nrel_unit_of_measurement);

    searchLinksTemplate.Triple(
    CreateIngredientKeynodes::concept_unit_of_measurement,
    ScType::VarPermPosArc,
    "_unit_of_measure");

    searchLinksTemplate.Quintuple(
    "_unit_of_measure",
    ScType::VarCommonArc,
    ScType::VarNodeLink >> "_idtf_link_two",
    ScType::VarPermPosArc,
    ScKeynodes::nrel_main_idtf);

    searchLinksTemplate.Triple(
    CreateIngredientKeynodes::lang_en,
    ScType::VarPermPosArc,
    "_idtf_link_two");
    

    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchLinksTemplate, result);
    for (size_t i = 0; i < result.Size(); ++i)
    {
        auto const & construction = result[i];
        ScAddr const & idtfLink = construction["_idtf_link"];
        std::string idtf;
        m_context.GetLinkContent(idtfLink, idtf);

        if(idtf == ingredient_name)
        { 
            SC_AGENT_LOG_DEBUG("Found parameter: " << idtf);
            return construction["concept_name_of_ingr"];
        }
    }
    return ScAddr::Empty;
}*/
ScAddr CreateRecipeAgent::CreateNamedNode(std::string nameOfNode, ScType const &nodeType)
{
    ScAddr const & node = m_context.ResolveElementSystemIdentifier(nameOfNode, nodeType);
    return node;
}

void CreateRecipeAgent::FillList(std::vector<std::string> &list)
{
    int i = 0;
    while(true)
    {   
        std::string buffer = "",c = "";
        std::cout << "\nInput your value:";
        getline(std::cin, buffer);
        list.push_back(buffer);
        std::cout << "\nWould you like to continue?(Y/N)";
        getline(std::cin, c);
        if (c == "N")
            break;       
    }
}

void CreateRecipeAgent::AddConceptsIngrORTool(std::vector<std::string> &list, ScAddr const & nrel_keynode, ScStructure &main_structure, ScStructure &recipe_structure)
{
    ScAddr const & tuple = m_context.GenerateNode(ScType::ConstNodeTuple);
    main_structure << nrel_keynode << tuple;
    ScAddr const & commonArc = m_context.GenerateConnector(ScType::ConstCommonArc,recipe_structure,tuple);
    main_structure << commonArc;
    main_structure << m_context.GenerateConnector(ScType::ConstPermPosArc, nrel_keynode, commonArc);

    for (size_t i = 0; i < list.size(); ++i)
    {
        ScAddr const & temp = CreateNamedNode("concept_"+list[i],ScType::ConstNodeClass);
        if(nrel_keynode == CreateRecipeKeynodes::nrel_ingredients)
        {   
            std::string buf1;
            std::cout << std::endl <<list[i];
            std::cout << "\nWould you like to choose an amount of this ingredient?(Y/N)";
            getline(std::cin, buf1);
            if (buf1 == "Y")
            {
                std::string buf2;
                std::cout << "\nInput your number of this ingredient:";
                getline(std::cin, buf2);
                ScAddr const & num = m_context.ResolveElementSystemIdentifier(buf2,ScType::ConstNode);
                ScAddr const & common_arc_to_num = m_context.GenerateConnector(ScType::ConstCommonArc, temp, num);
                main_structure << num << common_arc_to_num << CreateRecipeKeynodes::nrel_quantity 
                << m_context.GenerateConnector(ScType::ConstPermPosArc,CreateRecipeKeynodes::nrel_quantity,common_arc_to_num );
            }
        }
        main_structure << temp << m_context.GenerateConnector(ScType::ConstPermPosArc, tuple, temp);
    }
    
}

std::string CreateRecipeAgent::ReplaceSpacesWith_(std::string str) 
{
    for (size_t i = 0; i < str.length(); i++)
    {
        if(str[i] == ' ')
            str[i] = '_';
    }
    return str;
}

ScAddr CreateRecipeAgent::CreateStep(ScStructure &recipe_structure, std::vector<std::string> &ingr_list, std::vector<std::string> &tools_list)
{   
    // Инициализация переменных
    std::string action_name_str, c1, c2, c3, c4;
    
    // Запрос на ввод названия действия
    std::cout << "\nEnter the name of cooking action:";
    getline(std::cin, action_name_str);

    // Создание узлов для действия
    ScAddr const &variable = m_context.GenerateNode(ScType::VarNode);
    ScAddr const &action_name = CreateNamedNode("action_" + ReplaceSpacesWith_(action_name_str), ScType::ConstNodeClass);
    recipe_structure << variable << action_name << m_context.GenerateConnector(ScType::VarPermPosArc, action_name, variable);

    // Запрос на выбор инструмента
    std::cout << "\nWould you like to choose your tool?(Y/N)";
    getline(std::cin, c1);
    if (c1 == "Y")
    {
        // Выбор инструмента из списка
        std::string choosing_tool;
        std::cout << "\nChoose tool:";
        for (size_t i = 0; i < tools_list.size(); i++)
            std::cout << "\n" << i << " " << tools_list[i];
        getline(std::cin, choosing_tool);

        // Создание связи между выбранным инструментом и действием
        ScAddr const &var_tool = CreateNamedNode("_" + tools_list[stoi(choosing_tool)], ScType::VarNode);
        ScAddr const &arc_to_var_tool = m_context.GenerateConnector(ScType::VarPermPosArc, variable, var_tool);
        recipe_structure 
            << arc_to_var_tool << var_tool << ScKeynodes::rrel_1 
            << m_context.GenerateConnector(ScType::VarPermPosArc, ScKeynodes::rrel_1, arc_to_var_tool);
    }

    // Запрос на выбор ингредиента
    std::cout << "\nWould you like to choose your ingredient?(Y/N)";
    getline(std::cin, c2);
    if (c2 == "Y")
    {
        
        // Выбор ингредиента из списка
        std::string choosing_ingr;
        std::cout << "\nChoose ingredient:";
        for (size_t i = 0; i < ingr_list.size(); i++)
            std::cout << "\n" << i << " " << ingr_list[i];
        getline(std::cin, choosing_ingr);

        // Создание узлов для выбранного ингредиента
        ScAddr const &var_ingr = CreateNamedNode("_" + ingr_list[stoi(choosing_ingr)], ScType::VarNode);
        ScAddr const &concept_name_ingr = CreateNamedNode("concept_" + ingr_list[stoi(choosing_ingr)], ScType::ConstNodeClass);
        ScAddr const &arc_concept_to_var_ingr = m_context.GenerateConnector(ScType::VarPermPosArc, concept_name_ingr, var_ingr);
        ScAddr const &arc_to_var_ingr = m_context.GenerateConnector(ScType::VarPermPosArc, variable, var_ingr);

        // Запрос на проверку, является ли ингредиент динамическим аргументом
        std::cout << "\nIs your ingredient a dynamic argument ?(Y/N)";
        getline(std::cin, c3);
        if (c3 == "Y")
        {
            // Связывание динамического ингредиента
            recipe_structure << var_ingr << arc_to_var_ingr << concept_name_ingr << arc_concept_to_var_ingr << ScKeynodes::rrel_2 
                << m_context.GenerateConnector(ScType::VarPermPosArc, ScKeynodes::rrel_2, arc_to_var_ingr) << CreateRecipeKeynodes::rrel_dynamic_argument
                << m_context.GenerateConnector(ScType::VarPermPosArc, CreateRecipeKeynodes::rrel_dynamic_argument, arc_to_var_ingr);
        }  
        else
        {
            // Связывание статического ингредиента
            recipe_structure << var_ingr << arc_to_var_ingr << concept_name_ingr << arc_concept_to_var_ingr << ScKeynodes::rrel_2 
                << m_context.GenerateConnector(ScType::VarPermPosArc, ScKeynodes::rrel_2, arc_to_var_ingr);
        }
    }

    // Запрос на дополнительные условия для ингредиента
    std::cout << "\nDoes your ingredient have additional conditions?(Y/N)";
    getline(std::cin, c4);
    if (c4 == "Y")
    {   
        // Запрос на ввод дополнительных условий
        std::string condit;
        std::cout << "\n Please, input your additional conditions:";
        getline(std::cin, condit);

        // Создание узла и соединения для дополнительных условий
        ScAddr const &add_cond = CreateNamedNode(ReplaceSpacesWith_(condit), ScType::ConstNodeClass);
        ScAddr const &arc_cond = m_context.GenerateConnector(ScType::VarPermPosArc, variable, add_cond);
        recipe_structure << add_cond << arc_cond << ScKeynodes::rrel_3
            << m_context.GenerateConnector(ScType::VarPermPosArc, ScKeynodes::rrel_3, arc_cond);
    }

    // Завершение добавления шага
    std::cout << "\nAdding step is finished!";
    return variable;
}


ScResult CreateRecipeAgent::DoProgram(ScAction &action)
{
    // Инициализация переменных
    std::string name_of_the_cooking_recipe, main_ingr, main_tool;
    std::vector<std::string> ingredients_list, tools_list;

    // Создание структуры для рецепта и основной структуры
    ScStructure main_structure = m_context.GenerateStructure();
    ScStructure recipe_structure = m_context.GenerateStructure();
    
    // Запрос на ввод названия рецепта
    std::cout << "\nInput your name of the cooking recipe:";
    getline(std::cin, name_of_the_cooking_recipe);

    // Генерация и установка связи для названия рецепта
    ScAddr const &name_of_the_recipe_link = m_context.GenerateLink(ScType::ConstNodeLink);
    m_context.SetLinkContent(name_of_the_recipe_link, name_of_the_cooking_recipe);

    // Связывание структур
    main_structure << recipe_structure;
    main_structure << name_of_the_recipe_link << CreateRecipeKeynodes::lang_en
        << m_context.GenerateConnector(ScType::ConstPermPosArc, CreateRecipeKeynodes::lang_en, name_of_the_recipe_link);

    // Создание соединений для связи между названием рецепта и структурой
    ScAddr const &arc_between_name_and_struct = m_context.GenerateConnector(ScType::ConstCommonArc, recipe_structure, name_of_the_recipe_link);
    main_structure << arc_between_name_and_struct;
    main_structure << ScKeynodes::nrel_main_idtf;
    main_structure << m_context.GenerateConnector(ScType::ConstPermPosArc, ScKeynodes::nrel_main_idtf, arc_between_name_and_struct);

    // Связь с концептом рецепта
    main_structure << CreateRecipeKeynodes::concept_recipe;
    main_structure << m_context.GenerateConnector(ScType::ConstPermPosArc, CreateRecipeKeynodes::concept_recipe, recipe_structure);

    // Запрос на ввод списка ингредиентов
    std::cout << "\nEnter your ingredient list:";
    FillList(ingredients_list);
    AddConceptsIngrORTool(ingredients_list, CreateRecipeKeynodes::nrel_ingredients, main_structure, recipe_structure);

    // Запрос на ввод списка инструментов
    std::cout << "\nEnter your tools list:";
    FillList(tools_list);
    AddConceptsIngrORTool(tools_list, CreateRecipeKeynodes::nrel_tools, main_structure, recipe_structure);

    // Запрос на ввод основного ингредиента
    std::cout << "\nInput main ingredient:";
    getline(std::cin, main_ingr);

    // Запрос на ввод основного инструмента
    std::cout << "\nInput main tool:";
    getline(std::cin, main_tool);

    // Создание и связывание узлов для действия приготовления
    ScAddr const &action_cooking = CreateNamedNode("action_cooking_" + ReplaceSpacesWith_(name_of_the_cooking_recipe), ScType::ConstNodeClass);
    recipe_structure << action_cooking;
    
    // Создание переменной для основного шага
    ScAddr const &main_var = m_context.GenerateNode(ScType::VarNode);
    recipe_structure << main_var << m_context.GenerateConnector(ScType::VarPermPosArc, action_cooking, main_var);

    // Создание концептов для основного ингредиента и инструмента
    ScAddr const &concept_main_ingr = CreateNamedNode("concept_" + main_ingr, ScType::ConstNodeClass);
    ScAddr const &concept_main_tool = CreateNamedNode("concept_" + main_tool, ScType::ConstNodeClass);
    
    // Создание переменных для основного ингредиента и инструмента
    ScAddr const &var_main_ingr = CreateNamedNode("_" + main_ingr, ScType::VarNode);
    ScAddr const &var_main_tool = CreateNamedNode("_" + main_tool, ScType::VarNode);

    // Связывание концептов и переменных
    recipe_structure << concept_main_ingr << concept_main_tool << var_main_ingr << var_main_tool
        << m_context.GenerateConnector(ScType::VarPermPosArc, concept_main_ingr, var_main_ingr)
        << m_context.GenerateConnector(ScType::VarPermPosArc, concept_main_tool, var_main_tool);

    // Создание связей для ингредиента и инструмента
    ScAddr const &VarArcIngr = m_context.GenerateConnector(ScType::VarPermPosArc, main_var, var_main_ingr);
    ScAddr const &VarArcTool = m_context.GenerateConnector(ScType::VarPermPosArc, main_var, var_main_tool);

    // Добавление rrel и связей для основного шага
    recipe_structure << ScKeynodes::rrel_1 << ScKeynodes::rrel_2 << VarArcIngr << VarArcTool
        << m_context.GenerateConnector(ScType::VarPermPosArc, ScKeynodes::rrel_1, VarArcIngr)
        << m_context.GenerateConnector(ScType::VarPermPosArc, ScKeynodes::rrel_2, VarArcTool);

    // Связывание с нодой для декомпозиции действия
    recipe_structure << CreateRecipeKeynodes::nrel_decomposition_of_action;
    ScAddr const &var_step_tuple = m_context.GenerateNode(ScType::VarNodeTuple);
    ScAddr const &var_common_arc = m_context.GenerateConnector(ScType::VarCommonArc, var_step_tuple, main_var);
    recipe_structure << var_step_tuple << var_common_arc
        << m_context.GenerateConnector(ScType::VarPermPosArc, CreateRecipeKeynodes::nrel_decomposition_of_action, var_common_arc);

    // Создание первого шага рецепта
    ScAddr var_first_step = CreateStep(recipe_structure, ingredients_list, tools_list);
    ScAddr const &var_first_step_arc = m_context.GenerateConnector(ScType::VarPermPosArc, var_step_tuple, var_first_step);
    recipe_structure << var_first_step_arc << ScKeynodes::rrel_1
        << m_context.GenerateConnector(ScType::VarPermPosArc, ScKeynodes::rrel_1, var_first_step_arc);

    // Добавление шагов, если пользователь желает
    while (true)
    {
        std::cout << "\nWould you like to add another step?";
        std::string temp;
        getline(std::cin, temp);
        if (temp == "Y")
        {
            // Создание следующего шага
            ScAddr const &step = CreateStep(recipe_structure, ingredients_list, tools_list);
            ScAddr const &step_arc = m_context.GenerateConnector(ScType::VarPermPosArc, var_step_tuple, step);
            ScAddr const &arc_to_next_step = m_context.GenerateConnector(ScType::VarCommonArc, var_first_step, step);

            // Связывание шага с предыдущим
            recipe_structure << step_arc << CreateRecipeKeynodes::nrel_goto
                << arc_to_next_step
                << m_context.GenerateConnector(ScType::VarPermPosArc, CreateRecipeKeynodes::nrel_goto, arc_to_next_step);

            // Обновление ссылки на первый шаг
            var_first_step = step;
        }
        else
            break;
    }

    // Завершение действия и возврат результата
    action.SetResult(main_structure);
    return action.FinishSuccessfully();
}
