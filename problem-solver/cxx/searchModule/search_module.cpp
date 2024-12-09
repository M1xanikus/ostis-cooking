#include "search_module.hpp"

#include "agents/search_subsets_agent.hpp"

SC_MODULE_REGISTER(SearchModule)
  ->Agent<SearchSubsetsAgent>();