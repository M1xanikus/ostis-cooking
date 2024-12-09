#include "edit_recipe_agent.hpp"
#include <string>
#include "keynodes/edit_recipe_keynodes.hpp"
#include <iostream>
#include <vector>

ScAddr EditRecipeAgent::GetActionClass() const
{
    return EditRecipeKeynodes::action_edit_recipe;
};

// Метод для создания именованного узла
ScAddr EditRecipeAgent::CreateNamedNode(std::string nameOfNode, ScType const &nodeType)
{
    // Создание или получение узла с указанным системным идентификатором
    ScAddr const & node = m_context.ResolveElementSystemIdentifier(nameOfNode, nodeType);
    return node;
}

// Метод для поиска рецепта по имени
ScAddr EditRecipeAgent::FindRecipe(std::string recipe_name)
{
    // Создание шаблона для поиска рецепта
    ScTemplate searchlinksTemplate;
    searchlinksTemplate.Triple(
        EditRecipeKeynodes::lang_en,
        ScType::PermPosArc,
        ScType::NodeLink >> "_link");

    searchlinksTemplate.Quintuple(
        ScType::NodeStructure >> "_struct",
        ScType::CommonArc,
        "_link",
        ScType::PermPosArc,
        ScKeynodes::nrel_main_idtf);
    
    // Поиск рецепта с помощью шаблона
    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchlinksTemplate, result);
    // Поиск совпадений идентификатора рецепта с введенным пользователем
    for (size_t i = 0; i < result.Size(); ++i)
    {
        auto const & construction = result[i];
        ScAddr const & idtfLink = construction["_link"];
        std::string recipe_idtf;
        m_context.GetLinkContent(idtfLink, recipe_idtf);
        if(recipe_idtf == recipe_name)
        {
            return construction["_struct"];
        }
    }
    // Возврат пустого адреса, если рецепт не найден
    return ScAddr::Empty;
}

// Метод для получения списка ингредиентов рецепта
 std::vector<std::string> EditRecipeAgent::ListOfIngredients(ScAddr & structure)
 {
    std::vector<std::string> ingredients;
    
    // Создание шаблона для поиска ингредиентов
    ScTemplate searchingredientsTemplate;
    searchingredientsTemplate.Quintuple(
        structure,
        ScType::CommonArc,
        ScType::NodeTuple >> "_tuple",
        ScType::PermPosArc,
        EditRecipeKeynodes::nrel_ingredients
    );

    searchingredientsTemplate.Triple(
        "_tuple",
        ScType::PermPosArc,
        ScType::Node >> "_ingredient"
    );

    // Поиск ингредиентов с помощью шаблона    
    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchingredientsTemplate, result);

    // Извлечение имен ингредиентов
    for (size_t i = 0; i < result.Size(); ++i)
    {
        auto const & construction = result[i];
        ScAddr const & idtfLink = construction["_ingredient"];
        std::string name_of_ingr;
        name_of_ingr = m_context.GetElementSystemIdentifier(idtfLink);
        name_of_ingr = name_of_ingr.substr(name_of_ingr.find('_'));
        ingredients.push_back(name_of_ingr);
    }

    // Возврат списка ингредиентов
    return ingredients;
}

// Метод для вывода содержимого вектора на экран
void EditRecipeAgent::ViewVector(std::vector<std::string> vector)
{
    for (size_t i = 0; i < vector.size(); ++i)
    {
        std::cout << vector[i];
        if (i != vector.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
}

// Метод для получения списка инструментов рецепта
std::vector<std::string> EditRecipeAgent::ListOfTools(ScAddr & structure)
{
    std::vector<std::string> tools;
    
    // Создание шаблона для поиска инструментов
    ScTemplate searchtoolsTemplate;
    
    searchtoolsTemplate.Quintuple(
        structure,
        ScType::CommonArc,
        ScType::NodeTuple >> "_tuple",
        ScType::PermPosArc,
        EditRecipeKeynodes::nrel_tools
    );

    searchtoolsTemplate.Triple(
        "_tuple",
        ScType::PermPosArc,
        ScType::NodeClass >> "_tool"
    );
    
    // Поиск инструментов с помощью шаблона
    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchtoolsTemplate, result);

    // Извлечение имен инструментов
    for (size_t i = 0; i < result.Size(); ++i)
    {
        auto const & construction = result[i];
        ScAddr const & idtfLink = construction["_tool"];
        std::string name_of_tool;
        name_of_tool = m_context.GetElementSystemIdentifier(idtfLink);
        name_of_tool = name_of_tool.substr(name_of_tool.find('_'));
        tools.push_back(name_of_tool);
    }

    // Возврат списка инструментов
    return tools;
}

// Метод для поиска первого шага в рецепте
ScAddr EditRecipeAgent::FindFirstStep(ScAddr & structure)
{
    // Создание шаблона для поиска первого шага
    ScTemplate searchnodeTemplate;
    searchnodeTemplate.Triple(
        structure,
        ScType::PermPosArc,
        ScType::NodeTuple >> "_tuple"
    );

    searchnodeTemplate.Quintuple(
        "_tuple",
        ScType::PermPosArc,
        ScType::Node >> "_first_node",
        ScType::PermPosArc,
        ScKeynodes::rrel_1
    );

    // Поиск первого шага с помощью шаблона
    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchnodeTemplate, result);

    // Проверка наличия шагов в рецепте
    if (result.Size() == 0) 
    {
        std::cout << "In this recipe there are no steps!";
        return ScAddr::Empty;
    }
    // Возврат адреса первого шага
    return result[0]["_first_node"];
}

// Метод для проверки наличия следующего шага
bool EditRecipeAgent::isNextStep(ScAddr & structure)
{
    // Создание шаблона для поиска следующего шага
    ScTemplate searchnextstepTemplate;
    searchnextstepTemplate.Quintuple(
        structure,
        ScType::CommonArc,
        ScType::Node,
        ScType::PermPosArc,
        EditRecipeKeynodes::nrel_goto
    );

    // Поиск следующего шага с помощью шаблона
    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchnextstepTemplate, result);

    // Возврат результата проверки
    if (result.Size() == 0) return false;
    return true; 
}

bool EditRecipeAgent::isPreviousStep(ScAddr & structure)
{
    ScTemplate searchpreviousstepTemplate;
    searchpreviousstepTemplate.Quintuple(
        ScType::Node,
        ScType::CommonArc,
        structure,
        ScType::PermPosArc,
        EditRecipeKeynodes::nrel_goto
    );
    
    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchpreviousstepTemplate, result);

    if (result.Size() == 0) return false;
    return true;
}

// Метод для удаления шага из рецепта
void EditRecipeAgent::DeleteStep(ScAddr & structure)
{
    // Создание шаблонов для удаления связей шага
    ScTemplate searchtoolTemplate;
    searchtoolTemplate.Quintuple(
        structure,
        ScType::PermPosArc >> "_second",
        ScType::Node,
        ScType::PermPosArc,
        ScKeynodes::rrel_1
    );

    ScTemplate searchingrTemplate;
    searchingrTemplate.Quintuple(
        structure,
        ScType::PermPosArc >> "_third",
        ScType::Node,
        ScType::PermPosArc,
        ScKeynodes::rrel_2
    );
    
    ScTemplate searchadditTemplate;
    searchadditTemplate.Quintuple(
        structure,
        ScType::PermPosArc >> "_fifth",
        ScType::NodeClass,
        ScType::PermPosArc,
        ScKeynodes::rrel_3
    );

    ScTemplate searchactionTemplate;
    searchactionTemplate.Triple(
        ScType::NodeClass >> "_action",
        ScType::PermPosArc,
        structure
    );

    // Удаление найденных связей
    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchtoolTemplate, result);

    ScTemplateSearchResult result2;
    m_context.SearchByTemplate(searchingrTemplate, result2);

    ScTemplateSearchResult result3;
    m_context.SearchByTemplate(searchadditTemplate, result3);

    ScTemplateSearchResult result4;
    m_context.SearchByTemplate(searchactionTemplate, result4);
    if (result4.Size() != 0) bool const isArcErased1 = m_context.EraseElement(result4[0]["_action"]);
    if (result.Size() != 0) bool const isArcErased2 = m_context.EraseElement(result[0]["_second"]);
    if (result2.Size() != 0) bool const isArcErased3 = m_context.EraseElement(result2[0]["_third"]);
    if (result3.Size() != 0) bool const isArcErased4 = m_context.EraseElement(result3[0]["_fifth"]);
}

// Метод для замены пробелов в строке на символ "_"
std::string EditRecipeAgent::ReplaceSpacesWith_(std::string str) 
{
    for (size_t i = 0; i < str.length(); i++)
    {
        if(str[i] == ' ')
            str[i] = '_';
    }
    return str;
}

// Метод для создания нового шага в рецепте
void EditRecipeAgent::CreateStep(ScAddr & structure, ScAddr & structure2, std::vector<std::string> ingredients, std::vector<std::string> tools)
{   
    std::string action_name_str, c1, c2, c4;
    int choice = 0;
    int choice2 = 0;

    // Ввод имени действия для нового шага
    std::cout << "\nEnter the name of cooking action:(input with _ insted of 'spaces')";
    std::cin >> action_name_str;

    // Создание узла действия
    ScAddr const &action_name = CreateNamedNode("action_" + ReplaceSpacesWith_(action_name_str), ScType::ConstNodeClass);
    ScAddr const & var_to_act = m_context.GenerateConnector(ScType::VarPermPosArc, action_name, structure);
    m_context.GenerateConnector(ScType::ConstPermPosArc, structure2, var_to_act);
    m_context.GenerateConnector(ScType::ConstPermPosArc, structure2, action_name);
    // Выбор инструмента для действия
    std::cout << "\nWould you like to choose your tool?(Y/N)";
    std::cin >> c1;
    if (c1 == "Y" || c1 == "y")
    {
        ViewVector(tools);
        while (true)
        {
            std::cout << "Choose one of the tools (" << 0 << "-" << tools.size() - 1 << ")" << std::endl;
            std::cin >> choice;
            if (choice >= 0 && choice < tools.size()) break;
        }
        ScAddr const &tool = CreateNamedNode(tools[choice], ScType::VarNode);
        ScAddr const & arc_to_tool = m_context.GenerateConnector(ScType::VarPermPosArc, structure, tool);
        m_context.GenerateConnector(ScType::ConstPermPosArc, structure2, arc_to_tool);
        //m_context.GenerateConnector(ScType::ConstPermPosArc, structure2, tool);
        ScAddr const & arc_to_rel = m_context.GenerateConnector(ScType::VarPermPosArc, ScKeynodes::rrel_1, arc_to_tool);
        m_context.GenerateConnector(ScType::ConstPermPosArc, structure2, arc_to_rel);
        //ScAddr const & concept_tool = CreateNamedNode("concept" + tools[choice], ScType::NodeClass);
        //m_context.GenerateConnector(ScType::ConstPermPosArc, structure2, concept_tool);
        //ScAddr const & arc_to_conc = m_context.GenerateConnector(ScType::VarPermPosArc, concept_tool, tool);
        //m_context.GenerateConnector(ScType::ConstPermPosArc, structure2, arc_to_conc);
    }

    // Выбор ингредиента для действия
    std::cout << "\nWould you like to choose your ingredient?(Y/N)";
    std::cin >> c2;
    if (c2 == "Y" || c2 == "y")
    {
    
        ViewVector(ingredients);
        while (true)
        {
            std::cout << "Choose one of the ingredients (" << 0 << "-" << ingredients.size() - 1 << ")" << std::endl;
            std::cin >> choice2;
            if (choice2 >= 0 && choice2 <= ingredients.size()) break;
        }
        ScAddr const &ingredient = CreateNamedNode(ingredients[choice2], ScType::VarNode);
        ScAddr const & arc_to_ingr = m_context.GenerateConnector(ScType::VarPermPosArc, structure, ingredient);
        m_context.GenerateConnector(ScType::ConstPermPosArc, structure2, arc_to_ingr);
        //m_context.GenerateConnector(ScType::ConstPermPosArc, structure2, tool);
        ScAddr const & arc_to_rel2 = m_context.GenerateConnector(ScType::VarPermPosArc, ScKeynodes::rrel_2, arc_to_ingr);
        m_context.GenerateConnector(ScType::ConstPermPosArc, structure2, arc_to_rel2);
        //ScAddr const &ingredienta = CreateNamedNode("concept" + ingredients[choice2], ScType::VarNode);
        //m_context.GenerateConnector(ScType::VarPermPosArc, ingredienta, ingredient);
        //m_context.GenerateConnector(ScType::VarPermPosArc, structure2, ingredienta);
    }
    // Указание дополнительных условий для ингредиента
    std::cout << "\nDoes your ingredient have additional conditions?(Y/N)";
    std::cin >> c4;
    if (c4 == "Y" || c4 == "y")
    {
        std::string condit;
        std::cout << "\n Please, input your additional conditions:(input with _ insted of 'spaces')";
        std::cin >> condit;
        ScAddr const &add_cond = CreateNamedNode(condit, ScType::ConstNodeClass);
        m_context.GenerateConnector(ScType::VarPermPosArc, structure, add_cond);

        ScAddr const & arc_to_cond = m_context.GenerateConnector(ScType::VarPermPosArc, structure, add_cond);
        m_context.GenerateConnector(ScType::ConstPermPosArc, structure2, arc_to_cond);
        //m_context.GenerateConnector(ScType::ConstPermPosArc, structure2, tool);
        ScAddr const & arc_to_rel3 = m_context.GenerateConnector(ScType::VarPermPosArc, ScKeynodes::rrel_3, arc_to_cond);
        m_context.GenerateConnector(ScType::ConstPermPosArc, structure2, arc_to_rel3);
        m_context.GenerateConnector(ScType::ConstPermPosArc, structure2, add_cond);         
    }
    std::cout << "\nChanging step is finished!" << std::endl;
}
/*void EditRecipeAgent::AdfafadStep(ScAddr & structure)
{
    int choice = 0;

    ScTemplate searchtoolTemplate;
    searchtoolTemplate.Quintuple(
        structure,
        ScType::PermPosArc,
        ScType::Node >> "_tool",
        ScType::PermPosArc,
        ScKeynodes::rrel_1
    );

    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchtoolTemplate, result);

    if (result.Size() == 0) 
    {
        std::cout << "There is no tool at this step!";
        return;
    }

    std::cout << "What tool you want to change to?(" << 0 << "-" << tools.size() - 1 << ")" << std::endl;
    ViewVector(tools);
    while (true)
    {
        std::cout << "Select: ";  std::cin >> choice;
        if (choice >= tools.size() || choice <= 0) std::cout << "Incorrect imput!";
        break;
    }
    std::string new_tool = tools[choice];

    ScAddr const & idtfLink = result[0]["_tool"];
    m_context.SetElementSystemIdentifier(new_tool, idtfLink);
}

void EditRecipeAgent::ChangeIngredient(ScAddr & structure, std::vector<std::string> ingredients)
{
    int choice = 0;
    ScAddr ingredient;

    ScTemplate searchingredientTemplate;
    searchingredientTemplate.Quintuple(
        structure,
        ScType::PermPosArc,
        ScType::Node >> "_ingredient",
        ScType::PermPosArc,
        ScKeynodes::rrel_2
    );

    ScTemplate searchnodeTemplate;
    searchnodeTemplate.Triple(
        ScType::NodeStructure >> "_structure",
        ScType::PermPosArc,
        structure
    );

    searchnodeTemplate.Triple(
        "_structure",
        ScType::PermPosArc,
        ScType::Node >> "_node"
    );

    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchingredientTemplate, result);

    if (result.Size() == 0) 
    {
        std::cout << "There is no ingredient at this step!";
        return;
    }

    std::cout << "What ingredient you want to change to?(" << 0 << "-" << ingredients.size() - 1 << ")" << std::endl;
    ViewVector(ingredients);
    while (true)
    {
        std::cout << "Select: ";  std::cin >> choice;
        if (choice >= ingredients.size() || choice <= 0) 
        {
            std::cout << "Incorrect imput!";
            continue;
        }
        break;
    }
    std::string new_ingredient = ingredients[choice];

    ScAddr const & idtfLink = result[0]["_ingredient"];
    ScTemplateSearchResult result2;
    m_context.SearchByTemplate(searchnodeTemplate, result2);
    for (size_t i = 0; i < result2.Size(); ++i)
    {
        auto const & construction = result[i];
        ScAddr const & idtfLink = construction["_node"];
        std::string name_of_ingr;
        name_of_ingr = m_context.GetElementSystemIdentifier(idtfLink);
        if(name_of_ingr == ingredients[choice])
        {
            ingredient = construction["_node"];
        }
    }
    //ScAddr const & node = m_context.ResolveElementSystemIdentifier(new_ingredient, ScType::VarNode);
    ScAddr const & arc_from_node = m_context.GenerateConnector(ScType::VarPermPosArc, structure, ingredient);
}*/

/*ScAddr EditRecipeAgent::EditRecipe(ScAddr & structure, std::vector<std::string> tools, std::vector<std::string> ingredients)
{
    ScAddr & first_node = structure;
    char ch = 'n';
    int choice = 0;

    //do
    //{
        std::cout << "Do you want to change something in this step?(Y/N)";
        std::cin >> ch;
        switch (ch)
        {
        case 'Y':
        case 'y': 
            std::cout << "What do you want to change?" << std::endl;
            std::cout << "1.Tool" << std::endl;
            std::cout << "2.Ingredient" << std::endl;
            std::cout << "3.Addition" << std::endl;
            std::cout << "Select: "; std::cin >> choice;
            while (true)
            {
                switch (choice)
                {
                case 1:
                    ChangeTool(first_node, tools);
                    break;
                case 2:
                    ChangeIngredient(first_node, ingredients);
                    break;
                case 3:
                    //ChangeAddition();
                    break;
                default:
                    std::cout << "Incorrect input!" << std::endl;
                    break;
                }
                break;
            }
        default:
            break;
        }
    //} while (isNextStep(first_node));
    return first_node;
}*/

// Метод для установки следующего шага в рецепте
void EditRecipeAgent::SetNextStep(ScAddr & structure)
{
     // Создание шаблона для поиска следующего шага
    ScTemplate searchnextstepTemplate;
    searchnextstepTemplate.Quintuple(
        structure,
        ScType::CommonArc,
        ScType::Node >> "_next_node",
        ScType::PermPosArc,
        EditRecipeKeynodes::nrel_goto
    );

    // Поиск следующего шага с помощью шаблона
    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchnextstepTemplate, result);

    // Поиск следующего шага с помощью шаблона
    structure = result[0]["_next_node"];
}

// Метод для получения действия текущего шага
std::string EditRecipeAgent::GetAction(ScAddr & structure)
{
    // Создание шаблона для поиска следующего шага
    ScTemplate searchactionTemplate;
    searchactionTemplate.Triple(
        ScType::NodeClass >> "_action",
        ScType::PermPosArc,
        structure
    );

    // Поиск действия с помощью шаблона
    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchactionTemplate, result);
    // Возврат системного идентификатора действия
    return m_context.GetElementSystemIdentifier(result[0]["_action"]);
}

// Основной метод выполнения программы редактирования рецепта
ScResult EditRecipeAgent::DoProgram(ScAction & action)
{
    std::string recipe_name, act;
    std::vector<std::string> ingredients;
    std::vector<std::string> tools;
    char ch;
    // Запрос на ввод названия рецепта
    std::cout << "Write the name of the recipe: ";
    getline(std::cin, recipe_name);
    // Поиск рецепта по названию
    ScAddr structure = FindRecipe(recipe_name);
    if (structure == ScAddr::Empty) return action.FinishSuccessfully();
    // Получение списка ингредиентов и инструментов
    ingredients = ListOfIngredients(structure);
    tools = ListOfTools(structure);
    // Поиск первого шага рецепта
    ScAddr first_step = FindFirstStep(structure);
    // Цикл для прохождения и редактирования шагов
    while(isNextStep(first_step)){
        act = GetAction(first_step);
        std::cout << "Current step is: " << act << std::endl;
        std::cout << "Do you want to change this action?(Y/N)";
        std::cin >> ch;
        // Удаление и создание нового шага, если пользователь решил изменить шаг
        if (ch == 'Y' || ch == 'y')
        {
            DeleteStep(first_step);
            CreateStep(first_step, structure, ingredients, tools);
        }
        // Переход к следующему шагу
        SetNextStep(first_step);
    }
    // Работа с текущим шагом
    act = GetAction(first_step);
    std::cout << "Current step is: " << act << std::endl;
    std::cout << "Do you want to change this action?(Y/N)";
    std::cin >> ch;
    if (ch == 'Y' || ch == 'y')
    {
        DeleteStep(first_step);
        CreateStep(first_step, structure, ingredients, tools);
    }
     // Установка результата действия
    action.SetResult(structure);
    return action.FinishSuccessfully();
}


