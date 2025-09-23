#include "pch.hpp"
#include "splicing.hpp"
#include "tools/json.hpp"

#include <fstream>
#include <iostream>
#include <algorithm>

// Define the global variable that was declared 'extern' in the header
std::map<std::pair<u_short, u_short>, u_short> splicing_lookup_table;

// Using alias for convenience
using json = nlohmann::json;

// Function to load recipes
void load_splicing_recipes() {
    std::ifstream f("resources/splicing_recipes.json");
    if (!f.is_open()) {
        std::cerr << "NOTICE: Could not open resources/splicing_recipes.json. No custom recipes will be loaded." << std::endl;
        return;
    }

    json data;
    try {
        data = json::parse(f);
    } catch (const json::exception& e) {
        std::cerr << "ERROR: Failed to parse splicing_recipes.json: " << e.what() << std::endl;
        return;
    }

    // Find the highest existing item ID to avoid collisions.
    u_short max_id = 0;
    for (const auto& pair : items) {
        if (pair.first > max_id) {
            max_id = pair.first;
        }
    }

    // Create a reverse lookup map from item name to ID for efficiency.
    std::map<std::string, u_short> name_to_id;
    for (const auto& pair : items) {
        name_to_id[pair.second.raw_name] = pair.second.id;
    }

    printf("Loading splicing recipes...\n");

    for (const auto& recipe : data) {
        try {
            std::string parent1_name = recipe.at("parent1").get<std::string>();
            std::string parent2_name = recipe.at("parent2").get<std::string>();

            if (name_to_id.find(parent1_name) == name_to_id.end() || name_to_id.find(parent2_name) == name_to_id.end()) {
                fprintf(stderr, "Could not find parent items for recipe: %s + %s\n", parent1_name.c_str(), parent2_name.c_str());
                continue;
            }

            u_short id1 = name_to_id[parent1_name];
            u_short id2 = name_to_id[parent2_name];

            // Create new item from recipe
            item new_item{};
            const auto& result_data = recipe.at("result");

            max_id++;
            new_item.id = max_id;
            new_item.raw_name = result_data.at("raw_name").get<std::string>();
            // Use .value() to provide defaults for optional fields
            new_item.type = result_data.value("type", (u_char)type::SEED);
            new_item.hits = result_data.value("hits", (short)4);
            new_item.rarity = result_data.value("rarity", (short)1);
            new_item.tick = result_data.value("tick", 900); // Default growth time: 15 mins
            new_item.info = result_data.value("info", "A spliceable seed.");
            new_item.cloth_type = clothing::none; // Default clothing type

            // Add to global items map
            items[new_item.id] = new_item;

            // Add to lookup table, ensuring order doesn't matter
            u_short first_id = std::min(id1, id2);
            u_short second_id = std::max(id1, id2);
            splicing_lookup_table[std::make_pair(first_id, second_id)] = new_item.id;

            printf("Loaded recipe: %s + %s -> %s (ID: %d)\n", parent1_name.c_str(), parent2_name.c_str(), new_item.raw_name.c_str(), new_item.id);

        } catch (const json::exception& e) {
            fprintf(stderr, "Failed to parse a recipe: %s\n", e.what());
        }
    }
    printf("Finished loading splicing recipes.\n");
}
