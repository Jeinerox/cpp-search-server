#include "remove_duplicates.h"

void RemoveDuplicates(SearchServer& search_server) {

    auto IsInVector = [](const std::vector<int>& vec, int id) {
        return std::find(vec.begin(), vec.end(), id) != vec.end();
    };


    std::vector<int> ids_to_remove;
    for (const int document_id_1 : search_server) {
        const auto words_1 = search_server.GetWordFrequencies(document_id_1);

        for (const int document_id_2 : search_server) {
            if (document_id_1 == document_id_2 or IsInVector(ids_to_remove, document_id_1) or IsInVector(ids_to_remove, document_id_2)) {
                continue;
            }


            const auto words_2 = search_server.GetWordFrequencies(document_id_2);
            if (words_1.size() != words_2.size()) {
                continue;
            }

            auto it1 = words_1.begin();
            auto it2 = words_2.begin();

            while (it1 != words_1.end()) {
                if ((*it1).first != (*it2).first) {
                    break;
                }
                ++it1;
                ++it2;
            }
            if (it1 != words_1.end()) {
                continue;
            }
            int id_to_remove = std::max(document_id_1, document_id_2);
            ids_to_remove.push_back(id_to_remove);
            std::cout << "Found duplicate document id " << id_to_remove << std::endl;
        }
    }
    for (const int id : ids_to_remove) {
        search_server.RemoveDocument(id);
    }

}
