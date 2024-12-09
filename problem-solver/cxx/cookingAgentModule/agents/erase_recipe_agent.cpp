#include "erase_recipe_agent.hpp"
#include "keynodes/erase_recipe_keynodes.hpp"
#include "keynodes/create_recipe_keynodes.hpp"

ScAddr EraseRecipeAgent::GetActionClass() const
{
    return EraseRecipeKeynodes::action_erase_recipe;
}



ScResult EraseRecipeAgent::DoProgram(ScAction & action)
{
    // Инициализация переменных
    auto const & [setAddr] = action.GetArguments<1>();
    ScStructure structure = m_context.GenerateStructure();
    ScAddr const & concAddr = CreateRecipeKeynodes::concept_recipe;

    // Поиск concept_recipe с помощью итератора
    ScIterator3Ptr const it3 = m_context.CreateIterator3(
        concAddr,
        ScType::ConstPermPosArc,
        setAddr
    );
    
    while (it3->Next())
    {
        // Определяем является ли структура рецептом
        ScAddr const & compAddr = it3->Get(0);
        if (!m_context.IsElement(compAddr))
        {
            SC_AGENT_LOG_ERROR("Structure is not recipe");
            return action.FinishWithError();
        }
    }

    // Поиск всех VarNode в структуре
    ScIterator3Ptr const it3_1 = m_context.CreateIterator3(
        setAddr,
        ScType::ConstPermPosArc,
        ScType::VarNode
    );
    while (it3_1->Next())
    {
        // Удаление всех найденых VarNode
        ScAddr const & compAddr = it3_1->Get(2);
        bool const isNodeErased = m_context.EraseElement(compAddr);
    }

    // Поиск всех ConstNodeTuple в структуре
    ScIterator3Ptr const it3_2 = m_context.CreateIterator3(
        setAddr,
        ScType::ConstCommonArc,
        ScType::ConstNodeTuple
    );
    while (it3_2->Next())
    {
        // Удаление всех найденых ConstNodeTuple
        ScAddr const & compAddr = it3_2->Get(2);
        bool const isNodeErased = m_context.EraseElement(compAddr);
    }

    // Поиск ConstNodeLink в структуре
    ScIterator3Ptr const it3_3 = m_context.CreateIterator3(
        setAddr,
        ScType::ConstCommonArc,
        ScType::ConstNodeLink
    );
    while (it3_3->Next())
    {
        // Удаление ConstNodeLink
        ScAddr const & compAddr = it3_3->Get(2);
        bool const isNodeErased = m_context.EraseElement(compAddr);
    }

    // Поиск main_structure 
    ScIterator3Ptr const it3_4 = m_context.CreateIterator3(
        ScType::ConstNodeStructure,
        ScType::ConstPermPosArc,
        setAddr
    );
    while (it3_4->Next())
    {
        // Удаление лишних связей
        ScAddr const & compAddr = it3_4->Get(0);
        ScIterator3Ptr const it3_4_1 = m_context.CreateIterator3(
            compAddr,
            ScType::ConstPermPosArc,
            ScType::ConstNodeTuple
        );   
        while (it3_4_1->Next())
        {
           ScAddr const & tupleAddr = it3_3->Get(2);
           bool const isNodeErased = m_context.EraseElement(tupleAddr); 
        }; 
        ScIterator3Ptr const it3_4_2 = m_context.CreateIterator3(
            compAddr,
            ScType::ConstPermPosArc,
            ScType::ConstCommonArc
        );   
        while (it3_4_2->Next())
        {
           ScAddr const & arcAddr = it3_3->Get(2);
           bool const isNodeErased = m_context.EraseElement(arcAddr); 
        }; 
        bool const isNodeErased = m_context.EraseElement(compAddr);
    }
    bool const isNodeErased2 = m_context.EraseElement(setAddr);
    SC_AGENT_LOG_INFO("Some recipe has been removed");
    action.SetResult(structure);
    return action.FinishSuccessfully();
}
