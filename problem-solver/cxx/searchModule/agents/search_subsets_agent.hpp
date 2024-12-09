#pragma once 

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_agent.hpp>

class SearchSubsetsAgent : public ScActionInitiatedAgent
{
public:
 ScAddr GetActionClass() const override;

 ScResult DoProgram(ScAction & action) override;
};