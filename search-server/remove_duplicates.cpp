#include "remove_duplicates.h"
#include <map>
#include <set>





void RemoveDuplicates(SearchServer& search_server) { //эта реализация оказалась в 45 раз быстрее))
	std::set<std::set<std::string>> documents;
	std::vector<int> ids_to_remove;

	auto MapToSet = [](std::map<std::string, double> words_freq){ //нужно чтобы игнорировать частоты
		std::set<std::string> out;
		for (const auto& word_freq : words_freq){
			out.insert(word_freq.first);
		}
		return out;
	};
	
	for (const int document_id : search_server) {
		std::set<std::string> words = MapToSet(search_server.GetWordFrequencies(document_id));
		if (documents.count(words)) {
			ids_to_remove.push_back(document_id);
			std::cout << "Found duplicate document id " << document_id << std::endl;
		}
		else
			documents.insert(words);
	}

	for (const int id : ids_to_remove) {
		search_server.RemoveDocument(id);
	}

}


