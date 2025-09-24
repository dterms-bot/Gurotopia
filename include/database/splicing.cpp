#include "pch.hpp"
#include "splicing.hpp"
#include "items.hpp"
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

// Helper to append raw bytes of any value to a byte vector
template<typename T>
void append_bytes(std::vector<std::byte>& vec, const T& value) {
    const auto* bytes = reinterpret_cast<const std::byte*>(&value);
    vec.insert(vec.end(), bytes, bytes + sizeof(T));
}

void append_spliced_items_to_data() {
    // Get original item count from the header of im_data. The count is a short at offset 62.
    short original_count = 0;
    std::memcpy(&original_count, &im_data[62], sizeof(short));

    if (items.size() <= static_cast<u_short>(original_count)) {
        return; // No new items to add
    }

    printf("Appending %zu new items to item data...\n", items.size() - original_count);

    // The encryption token from cache_items
    static constexpr std::string_view token{"PBG892FXX982ABC*"};

    // Reserve space to avoid reallocations
    im_data.reserve(im_data.size() + (items.size() - original_count) * 200); // Approximate size per item

    for (u_short i = original_count; i < items.size(); ++i) {
        if (items.find(i) == items.end()) continue;
        const item& new_item = items.at(i);

        // This serialization logic is the reverse of cache_items()
        append_bytes(im_data, new_item.id);
        append_bytes(im_data, new_item.property);
        append_bytes(im_data, new_item.cat);
        append_bytes(im_data, new_item.type);
        im_data.push_back(std::byte{0}); // padding

        // name
        short name_len = static_cast<short>(new_item.raw_name.length());
        append_bytes(im_data, name_len);
        for (short c_idx = 0; c_idx < name_len; ++c_idx) {
            im_data.push_back(static_cast<std::byte>(new_item.raw_name[c_idx] ^ token[(c_idx + new_item.id) % token.length()]));
        }

        // Replicate the padding and empty/default fields based on cache_items
        append_bytes(im_data, static_cast<short>(0)); // Skipped field
        im_data.insert(im_data.end(), 13, std::byte{0}); // 13 bytes padding
        append_bytes(im_data, new_item.collision);
        append_bytes(im_data, static_cast<std::byte>(new_item.hits * 6));
        append_bytes(im_data, new_item.hit_reset);
        append_bytes(im_data, static_cast<std::byte>(new_item.cloth_type));
        append_bytes(im_data, new_item.rarity);
        im_data.push_back(std::byte{0}); // padding

        append_bytes(im_data, static_cast<short>(0)); // audio directory (empty)
        append_bytes(im_data, new_item.audioHash);
        im_data.insert(im_data.end(), 4, std::byte{0}); // padding

        for(int j=0; j<4; ++j) append_bytes(im_data, static_cast<short>(0)); // 4 empty strings

        im_data.insert(im_data.end(), 16, std::byte{0}); // padding
        append_bytes(im_data, new_item.tick);

        append_bytes(im_data, static_cast<short>(0)); // padding
        append_bytes(im_data, static_cast<short>(0)); // padding

        for(int j=0; j<3; ++j) append_bytes(im_data, static_cast<short>(0)); // 3 empty strings

        im_data.insert(im_data.end(), 80, std::byte{0}); // padding

        // version-dependent fields, assuming latest version and add empty/default fields
        append_bytes(im_data, static_cast<short>(0));
        append_bytes(im_data, static_cast<int>(0));
        im_data.insert(im_data.end(), 9, std::byte{0});
        append_bytes(im_data, static_cast<int>(0));
        append_bytes(im_data, static_cast<int>(0));
        im_data.insert(im_data.end(), 25, std::byte{0});
        append_bytes(im_data, static_cast<short>(0));
        append_bytes(im_data, static_cast<short>(0));
        append_bytes(im_data, static_cast<int>(0));
        append_bytes(im_data, static_cast<int>(0));
        im_data.insert(im_data.end(), 9, std::byte{0});
        append_bytes(im_data, static_cast<short>(0));

        // item info (v22)
        short info_len = static_cast<short>(new_item.info.length());
        append_bytes(im_data, info_len);
        for (char c : new_item.info) {
            im_data.push_back(static_cast<std::byte>(c));
        }
    }

    // Update the master item count
    short new_total_count = static_cast<short>(items.size());
    std::memcpy(&im_data[62], &new_total_count, sizeof(short));

    printf("Finished appending items. New item count: %d\n", new_total_count);
}
