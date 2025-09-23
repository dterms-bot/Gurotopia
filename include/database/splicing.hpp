#pragma once
    #ifndef SPLICING_HPP
    #define SPLICING_HPP

    #include "items.hpp"
    #include <string>
    #include <vector>
    #include <map>
    #include <utility>

    struct splicing_recipe {
        std::string parent1_name;
        std::string parent2_name;
        item result_item;
    };

    extern std::map<std::pair<u_short, u_short>, u_short> splicing_lookup_table;

    extern void load_splicing_recipes();

    #endif
