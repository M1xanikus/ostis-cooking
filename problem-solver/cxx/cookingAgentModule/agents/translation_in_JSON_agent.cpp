#include "translation_in_JSON_agent.hpp"
#include "keynodes/translatoin_in_JSON_keynodes.hpp"

// Метод для получения класса действия, связанного с переводом в JSON
ScAddr TranslationInJSONAgent::GetActionClass() const
{
    return TranslationKeynodes::action_translation_in_JSON;
};

// Метод для замены символов подчеркивания в строке на пробелы
std::string TranslationInJSONAgent::Replace_WithSpaces(std::string str) 
{
    for (size_t i = 0; i < str.length(); i++)
    {
        if(str[i] == '_') // Проверка символа на подчеркивание
            str[i] = ' '; // Замена подчеркивания на пробел
    }
    return str; // Возвращение измененной строки
}

// Метод для удаления из строки избыточных символов, начиная с первого подчеркивания
std::string TranslationInJSONAgent::RemoveRedundant(std::string str) 
{
    str = str.substr(str.find('_')); // Обрезаем строку начиная с первого подчеркивания
    str.erase(str.find('_'), 1); // Удаляем первое подчеркивание в строке
    return str; // Возвращаем результат
}

// Метод для преобразования строки, содержащей цифры, в целое число
int TranslationInJSONAgent::ChangeToDigit(std::string str)
{
    int b;
    int sum = 0;
    for(int i = 0; i < str.size(); i++)
    {
        if(isdigit(str[i])) // Проверяем, является ли символ цифрой
        {
            b = str[i] - '0'; // Преобразуем символ в число
            sum = sum * 10 + b; // Формируем итоговое число
        }
    }
    return sum; // Возвращаем результат
}

// Метод для поиска рецепта по имени
ScAddr TranslationInJSONAgent::FindRecipe(std::string recipe_name)
{
    ScTemplate searchlinksTemplate;
    // Шаблон для поиска связей в SC-репозитории
    searchlinksTemplate.Triple(
        TranslationKeynodes::lang_en,
        ScType::PermPosArc,
        ScType::NodeLink >> "_link");

    searchlinksTemplate.Quintuple(
        ScType::NodeStructure >> "_struct",
        ScType::CommonArc,
        "_link",
        ScType::PermPosArc,
        ScKeynodes::nrel_main_idtf);
    
    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchlinksTemplate, result); // Поиск по шаблону

    for (size_t i = 0; i < result.Size(); ++i)
    {
        auto const & construction = result[i];
        ScAddr const & idtfLink = construction["_link"];
        std::string recipe_idtf;
        m_context.GetLinkContent(idtfLink, recipe_idtf); // Получение содержимого ссылки
        if(recipe_idtf == recipe_name) // Сравнение идентификатора с именем рецепта
        { 
            return construction["_struct"]; // Возврат структуры рецепта
        }
    }
    return ScAddr::Empty; // Возврат пустого адреса, если рецепт не найден
}

// Метод для получения единицы измерения ингредиента
std::string TranslationInJSONAgent::GetUnit(ScAddr& structure)
{
    ScTemplate searchunitTemplate;

    // Шаблон для поиска единицы измерения
    searchunitTemplate.Triple(
        structure,
        ScType::PermPosArc,
        ScType::Node >> "_cls_ingredient"
    );

    searchunitTemplate.Quintuple(
        "_cls_ingredient",
        ScType::CommonArc,
        ScType::Node >> "_measurement",
        ScType::PermPosArc,
        TranslationKeynodes::nrel_unit_of_measurement
    );

    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchunitTemplate, result); // Поиск по шаблону

    if (result.Size() == 0) return "None"; // Если ничего не найдено, вернуть "None"

    std::string measurement;

    auto const& construction = result[0];
    ScAddr const& idtfMeasurement = construction["_measurement"];

    measurement = m_context.GetElementSystemIdentifier(idtfMeasurement); // Получение идентификатора

    return measurement; // Возврат единицы измерения
}

// Метод для получения количества ингредиента
std::string TranslationInJSONAgent::GetQuantity(ScAddr & structure)
{
    ScTemplate searchquantityTemplate;

    // Шаблон для поиска количества
    searchquantityTemplate.Quintuple(
        structure,
        ScType::CommonArc,
        ScType::Node >> "_quantity",
        ScType::PermPosArc,
        TranslationKeynodes::nrel_quantity
    );

    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchquantityTemplate, result); // Поиск по шаблону

    if (result.Size() == 0) return "None"; // Если ничего не найдено, вернуть "None"

    std::string quantity;

    auto const& construction = result[0];
    ScAddr const& idtfQuantity = construction["_quantity"];

    quantity = m_context.GetElementSystemIdentifier(idtfQuantity); // Получение идентификатора

    return quantity; // Возврат количества
}

// Метод для получения списка ингредиентов
std::vector<TranslationInJSONAgent::Ingredient> TranslationInJSONAgent::GetListOfIngredients(ScAddr& structure)
{
    ScTemplate searchingredientsTemplate;

    // Шаблон для поиска ингредиентов
    searchingredientsTemplate.Quintuple(
        structure,
        ScType::CommonArc,
        ScType::NodeTuple >> "_tuple",
        ScType::PermPosArc,
        TranslationKeynodes::nrel_ingredients
    );

    searchingredientsTemplate.Triple(
        "_tuple",
        ScType::PermPosArc,
        ScType::Node >> "_ingredient"
    );
    
    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchingredientsTemplate, result); // Поиск по шаблону

    std::vector<Ingredient> ingredients;

    for (size_t i = 0; i < result.Size(); ++i)
    {
        auto const& construction = result[i];

        TranslationInJSONAgent::Ingredient ing; // Создание структуры ингредиента
        ScAddr const& idtfIngredient = construction["_ingredient"];

        ScAddr ingred = construction["_ingredient"];

        ing.name = m_context.GetElementSystemIdentifier(idtfIngredient); // Имя ингредиента

        ing.quantity = ChangeToDigit(GetQuantity(ingred)); // Количество ингредиента

        ing.unit = GetUnit(ingred); // Единица измерения

        ingredients.push_back(ing); // Добавление ингредиента в список
    }
    return ingredients; // Возврат списка ингредиентов
}

// Метод для получения списка кухонной утвари
std::vector<std::string> TranslationInJSONAgent::GetListOfCookwares(ScAddr & structure)
{
    ScTemplate searchcookwaresTemplate;

    // Шаблон для поиска кухонной утвари
    searchcookwaresTemplate.Quintuple(
        structure,
        ScType::CommonArc,
        ScType::NodeTuple >> "_tuple",
        ScType::PermPosArc,
        TranslationKeynodes::nrel_tools
    );

    searchcookwaresTemplate.Triple(
        "_tuple",
        ScType::PermPosArc,
        ScType::Node >> "_cookware"
    );
    
    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchcookwaresTemplate, result); // Поиск по шаблону

    std::vector<std::string> cookwares;

    for (size_t i = 0; i < result.Size(); ++i)
    {
        auto const& construction = result[i];
        ScAddr const& idtfCookware = construction["_cookware"];
        std::string cookware;

        cookware = m_context.GetElementSystemIdentifier(idtfCookware); // Имя кухонной утвари

        cookwares.push_back(cookware); // Добавление кухонной утвари в список
    }
    return cookwares; // Возврат списка кухонной утвари
}

// Метод для получения информации о белках
std::string TranslationInJSONAgent::GetProteins(ScAddr & structure)
{
    ScTemplate searchingredientsTemplate;

    // Шаблон для поиска информации о белках
    searchingredientsTemplate.Quintuple(
        ScType::NodeClass >> "_recipe",
        ScType::CommonArc,
        structure,
        ScType::PermPosArc,
        TranslationKeynodes::nrel_recipe
    );

    searchingredientsTemplate.Quintuple(
        "_recipe",
        ScType::CommonArc,
        ScType::Node >> "_protein",
        ScType::PermPosArc,
        TranslationKeynodes::nrel_proteins
    );

    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchingredientsTemplate, result); // Поиск по шаблону

    std::string protein;

    auto const & construction = result[0];
    ScAddr const & idtfProtein = construction["_protein"];
        
    protein = m_context.GetElementSystemIdentifier(idtfProtein); // Получение идентификатора
    
    return protein; // Возврат информации о белках
}

// Метод для получения информации о жирах
std::string TranslationInJSONAgent::GetFats(ScAddr & structure)
{
    ScTemplate searchingredientsTemplate;

    // Шаблон для поиска жиров, связанных с рецептом
    searchingredientsTemplate.Quintuple(
        ScType::NodeClass >> "_recipe", 
        ScType::CommonArc,              
        structure,                      
        ScType::PermPosArc,           
        TranslationKeynodes::nrel_recipe 
    );

    searchingredientsTemplate.Quintuple(
        "_recipe",
        ScType::CommonArc,
        ScType::Node >> "_fat",         
        ScType::PermPosArc,
        TranslationKeynodes::nrel_fats  
    );

    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchingredientsTemplate, result); // Поиск по шаблону

    std::string fat;

    auto const & construction = result[0];
    ScAddr const & idtfFat = construction["_fat"]; // Получение идентификатора жиров

    fat = m_context.GetElementSystemIdentifier(idtfFat); // Извлечение информации о жирах
    
    return fat; // Возврат значения жиров
}

// Метод для получения информации о углеводах
std::string TranslationInJSONAgent::GetCarbohydrates(ScAddr & structure)
{
    ScTemplate searchingredientsTemplate;

    // Шаблон для поиска углеводов, связанных с рецептом
    searchingredientsTemplate.Quintuple(
        ScType::NodeClass >> "_recipe",
        ScType::CommonArc,
        structure,
        ScType::PermPosArc,
        TranslationKeynodes::nrel_recipe
    );

    searchingredientsTemplate.Quintuple(
        "_recipe",
        ScType::CommonArc,
        ScType::Node >> "_carbohydrates",  // Узел для хранения информации об углеводах
        ScType::PermPosArc,
        TranslationKeynodes::nrel_carbohydrates // Ролевая связь "углеводы"
    );

    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchingredientsTemplate, result); // Поиск по шаблону

    std::string carbohydrates;

    auto const & construction = result[0];
    ScAddr const & idtfCarbohydrates = construction["_carbohydrates"]; // Получение идентификатора углеводов

    carbohydrates = m_context.GetElementSystemIdentifier(idtfCarbohydrates); // Извлечение информации об углеводах
    
    return carbohydrates; // Возврат значения углеводов
}

// Метод для получения информации о калориях
std::string TranslationInJSONAgent::GetCalories(ScAddr & structure)
{
    ScTemplate searchingredientsTemplate;

    // Шаблон для поиска калорий, связанных с рецептом
    searchingredientsTemplate.Quintuple(
        ScType::NodeClass >> "_recipe",
        ScType::CommonArc,
        structure,
        ScType::PermPosArc,
        TranslationKeynodes::nrel_recipe
    );

    searchingredientsTemplate.Quintuple(
        "_recipe",
        ScType::CommonArc,
        ScType::Node >> "_calories", // Узел для хранения информации о калориях
        ScType::PermPosArc,
        TranslationKeynodes::nrel_calorie_content // Ролевая связь "калорийность"
    );

    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchingredientsTemplate, result); // Поиск по шаблону

    std::string calories;

    auto const & construction = result[0];
    ScAddr const & idtfCalories = construction["_calories"]; // Получение идентификатора калорий

    calories = m_context.GetElementSystemIdentifier(idtfCalories); // Извлечение информации о калориях
    
    return calories; // Возврат значения калорий
}

// Метод для получения количества порций
std::string TranslationInJSONAgent::GetServings(ScAddr & structure)
{
    ScTemplate searchingredientsTemplate;

    // Шаблон для поиска количества порций
    searchingredientsTemplate.Quintuple(
        ScType::NodeClass >> "_recipe",
        ScType::CommonArc,
        structure,
        ScType::PermPosArc,
        TranslationKeynodes::nrel_recipe
    );
searchingredientsTemplate.Quintuple(
        "_recipe",
        ScType::CommonArc,
        ScType::Node >> "_servings", // Узел для хранения информации о порциях
        ScType::PermPosArc,
        TranslationKeynodes::nrel_portion_number // Ролевая связь "количество порций"
    );

    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchingredientsTemplate, result); // Поиск по шаблону

    std::string servings;

    auto const & construction = result[0];
    ScAddr const & idtfServings = construction["_servings"]; // Получение идентификатора порций

    servings = m_context.GetElementSystemIdentifier(idtfServings); // Извлечение информации о порциях
    
    return servings; // Возврат значения количества порций
}

// Метод для получения общего времени приготовления
std::string TranslationInJSONAgent::GetTotalTime(ScAddr & structure)
{
    ScTemplate searchingredientsTemplate;

    // Шаблон для поиска времени приготовления
    searchingredientsTemplate.Quintuple(
        ScType::NodeClass >> "_recipe",
        ScType::CommonArc,
        structure,
        ScType::PermPosArc,
        TranslationKeynodes::nrel_recipe
    );

    searchingredientsTemplate.Quintuple(
        "_recipe",
        ScType::CommonArc,
        ScType::Node >> "_cooking_time", // Узел для хранения времени приготовления
        ScType::PermPosArc,
        TranslationKeynodes::nrel_cooking_time // Ролевая связь "время приготовления"
    );

    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchingredientsTemplate, result); // Поиск по шаблону

    std::string cookingTime;
    auto const & construction = result[0];
    ScAddr const & idtfCookingTime = construction["_cooking_time"]; // Получение идентификатора времени приготовления

    cookingTime = m_context.GetElementSystemIdentifier(idtfCookingTime); // Извлечение информации о времени приготовления
    
    return cookingTime; // Возврат значения времени приготовления
}

// Метод для получения действия рецепта
std::string TranslationInJSONAgent::GetAction(ScAddr & structure)
{
    ScTemplate searchnodeTemplate;

    // Шаблон для поиска действия, связанного с рецептом
    searchnodeTemplate.Triple(
        ScType::NodeClass >> "_action", // Узел действия
        ScType::PermPosArc,
        structure
    );

    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchnodeTemplate, result); // Поиск по шаблону

    if(result.Size() == 0) return "None"; // Если действие не найдено, вернуть "None"

    std::string action;

    auto const & construction = result[0];
    ScAddr const & idtfAction = construction["_action"]; // Получение идентификатора действия

    action = m_context.GetElementSystemIdentifier(idtfAction); // Извлечение информации о действии
    
    return action; // Возврат действия
}

// Метод для получения информации о кухонной утвари
std::string TranslationInJSONAgent::GetCookware(ScAddr & structure)
{
    ScTemplate searchtoolTemplate;

    // Шаблон для поиска кухонной утвари
    searchtoolTemplate.Quintuple(
        structure,
        ScType::PermPosArc,
        ScType::Node >> "_cookware", // Узел для хранения информации об утвари
        ScType::PermPosArc,
        ScKeynodes::rrel_1 // Ролевая связь
    );

    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchtoolTemplate, result); // Поиск по шаблону

    if(result.Size() == 0) return "None"; // Если утварь не найдена, вернуть "None"

    std::string cookware;

    auto const & construction = result[0];
    ScAddr const & idtfCookware = construction["_cookware"]; // Получение идентификатора утвари

    cookware = m_context.GetElementSystemIdentifier(idtfCookware); // Извлечение информации об утвари
    
    return cookware; // Возврат кухонной утвари
}

// Метод для получения информации о состоянии рецепта
std::string TranslationInJSONAgent::GetCondition(ScAddr & structure)
{
    ScTemplate searchconditionTemplate;

    // Шаблон для поиска состояния, связанного с рецептом
    searchconditionTemplate.Quintuple(
        structure,                 
        ScType::PermPosArc,        
        ScType::Node >> "_condition", 
        ScType::PermPosArc,
        ScKeynodes::rrel_3         
    );

    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchconditionTemplate, result); // Поиск по шаблону

    if(result.Size() == 0) return "None"; // Если состояние не найдено, вернуть "None"

    std::string condition;

    auto const & construction = result[0];
    ScAddr const & idtfCondition = construction["_condition"]; // Получение идентификатора состояния

    condition = m_context.GetElementSystemIdentifier(idtfCondition); // Извлечение информации о состоянии
    
    return condition; // Возврат состояния
}

// Метод для поиска первого шага в рецепте
ScAddr TranslationInJSONAgent::FindFirstStep(ScAddr & structure)
{
    ScTemplate searchnodeTemplate;

    // Шаблон для поиска первого шага
    searchnodeTemplate.Triple(
        structure,                     
        ScType::PermPosArc,            
        ScType::NodeTuple >> "_tuple"  
    );

    searchnodeTemplate.Quintuple(
        "_tuple",
        ScType::PermPosArc,
        ScType::Node >> "_first_node", // Первый шаг
        ScType::PermPosArc,
        ScKeynodes::rrel_1            
    );

    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchnodeTemplate, result); // Поиск по шаблону

    if (result.Size() == 0) 
    {
        std::cout << "В этом рецепте нет шагов!";
        return ScAddr::Empty; // Если шаги не найдены, вернуть пустой адрес
    }
    return result[0]["_first_node"]; // Вернуть адрес первого шага
}

// Метод для проверки наличия следующего шага
bool TranslationInJSONAgent::IsNextStep(ScAddr & structure)
{
    ScTemplate searchnextstepTemplate;

    // Шаблон для поиска связи "следующий шаг"
    searchnextstepTemplate.Quintuple(
        structure,                      // Текущий шаг
        ScType::CommonArc,             
        ScType::Node,                   // Узел следующего шага
        ScType::PermPosArc,            
        TranslationKeynodes::nrel_goto  // Ролевая связь "переход к следующему шагу"
    );

    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchnextstepTemplate, result); // Поиск по шаблону

    if (result.Size() == 0) return false; // Если следующий шаг не найден, вернуть false
    return true; // Следующий шаг найден
}

// Метод для перехода к следующему шагу
ScAddr TranslationInJSONAgent::SetNextStep(ScAddr & structure)
{
    ScTemplate searchnextstepTemplate;

    // Шаблон для поиска следующего шага
    searchnextstepTemplate.Quintuple(
        structure,                      // Текущий шаг
        ScType::CommonArc,
        ScType::Node >> "_next_step",   // Узел следующего шага
        ScType::PermPosArc,
        TranslationKeynodes::nrel_goto  // Ролевая связь "переход"
    );

    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchnextstepTemplate, result); // Поиск по шаблону

    if (result.Size() == 0) return ScAddr::Empty; // Если следующий шаг не найден, вернуть пустой адрес

    structure = result[0]["_next_step"]; // Установить текущую структуру на следующий шаг
    return structure; // Вернуть адрес следующего шага
}

// Метод для получения информации о шаге приготовления
nlohmann::json TranslationInJSONAgent::GetStep(ScAddr & structure, const std::vector<TranslationInJSONAgent::Ingredient>& ingredients)
{
    nlohmann::json step; // JSON-объект для хранения информации о шаге
    int numberOfStepIngredients;
// Добавление действия, кухонной утвари и условий
    step["Action"] = Replace_WithSpaces(RemoveRedundant(GetAction(structure)));
    step["Cookware"] = Replace_WithSpaces(RemoveRedundant(GetCookware(structure)));
    step["Condition"] = Replace_WithSpaces(GetCondition(structure));

    step["Ingredients"] = nlohmann::json::array(); // Создание массива для ингредиентов

    // Шаблон для поиска ингредиентов
    ScTemplate searchingredientTemplate;
    searchingredientTemplate.Quintuple(
        structure,
        ScType::PermPosArc,
        ScType::Node >> "_ingredient",
        ScType::PermPosArc,
        ScKeynodes::rrel_2        
    );

    ScTemplateSearchResult result;
    m_context.SearchByTemplate(searchingredientTemplate, result); // Поиск по шаблону

    if (result.Size() == 0) step["Ingredients"].push_back("None"); // Если ингредиенты не найдены, добавить "None"

    for (size_t i = 0; i < result.Size(); ++i) // Цикл по найденным ингредиентам
    {
        auto const & construction = result[i];
        std::string ingredient;

        ingredient = m_context.GetElementSystemIdentifier(construction["_ingredient"]); // Получение идентификатора ингредиента

        step["Ingredients"].push_back(Replace_WithSpaces(RemoveRedundant(ingredient))); // Добавление ингредиента в JSON
    }

    return step; // Возврат информации о шаге
}

// Метод для выполнения программы создания JSON рецепта
ScResult TranslationInJSONAgent::DoProgram(ScAction &action)
{
    nlohmann::json recipe; // JSON-объект для хранения рецепта
    std::string recipe_name;
    double proteins, fats, carbohydrates, calories;
    int servings, totalTime, numberOfSteps;

    std::cout << "Введите название рецепта: ";
    getline(std::cin, recipe_name); // Считывание названия рецепта

    ScAddr structure = FindRecipe(recipe_name); // Поиск структуры рецепта

    if (structure == ScAddr::Empty){
        std::cout << "There is no such dish!";
        return action.FinishUnsuccessfully();
    }

    // Добавление информации о названии рецепта
    recipe["Dish Name"] = recipe_name;

    std::vector<TranslationInJSONAgent::Ingredient> ingredients = GetListOfIngredients(structure); // Получение ингредиентов

    // Добавление информации об ингредиентах
    recipe["Ingredients"] = nlohmann::json::array();
    for (const auto& ing : ingredients) {
        nlohmann::json ingJson;
        ingJson["Name"] = Replace_WithSpaces(RemoveRedundant(ing.name));
        ingJson["Quantity"] = ing.quantity;
        ingJson["Unit"] = ing.unit;
        recipe["Ingredients"].push_back(ingJson);
    }

    // Добавление списка кухонной утвари
    std::vector<std::string> cookwares = GetListOfCookwares(structure);
    recipe["Cookwares"] = nlohmann::json::array();
    for (const auto& cookware : cookwares) {
        recipe["Cookwares"].push_back(Replace_WithSpaces(RemoveRedundant(cookware)));
    }

    // Добавление информации о питательных веществах, времени и порциях
    recipe["Proteins"] = Replace_WithSpaces(GetProteins(structure));
    recipe["Fats"] = Replace_WithSpaces(GetFats(structure));
    recipe["Carbohydrates"] = Replace_WithSpaces(GetCarbohydrates(structure));
    recipe["Calories"] = Replace_WithSpaces(GetCalories(structure));
    recipe["Servings"] = Replace_WithSpaces(GetServings(structure));
    recipe["Total Cooking Time"] = Replace_WithSpaces(GetTotalTime(structure));

    // Добавление шагов приготовления
    recipe["Preparation Steps"] = nlohmann::json::array();
    ScAddr first_step = FindFirstStep(structure);

    while(IsNextStep(first_step)) { // Цикл по шагам
        nlohmann::json step1 = GetStep(first_step, ingredients);
        recipe["Preparation Steps"].push_back(step1);
        first_step = SetNextStep(first_step);
    }

    nlohmann::json step = GetStep(first_step, ingredients); // Последний шаг
    recipe["Preparation Steps"].push_back(step);

    // Сохранение рецепта в файл
    std::ofstream file("recipe.json");
    file << std::setw(4) << recipe << std::endl;
    file.close();
    std::cout << "\nИнформация о рецепте сохранена в recipe.json\n";

    return action.FinishSuccessfully(); // Завершение программы
}