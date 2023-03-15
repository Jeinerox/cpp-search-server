#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <cmath>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        }
        else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

struct Document {
    int id;
    double relevance;
};

class SearchServer { //мне удобнее, когда класс не забит определениями методов. 
public:              //Я попытался вспомнить, как разделить программу на файлы, но, видимо, буду снова этому учиться далее на курсе))
    void SetStopWords(const string& text);
    void AddDocument(int document_id, const string& document);
    vector<Document> FindTopDocuments(const string& raw_query) const;
private:
    struct Query {
        set<string> plusQuery;
        set<string> minusQuery;
    };
    map<string, map<int, double>> word_to_document_freqs_;
    int document_count_ = 0;
    set<string> stop_words_;

    bool IsStopWord(const string& word) const { return stop_words_.count(word) > 0; }
    vector<string> SplitIntoWordsNoStop(const string& text) const;
    Query ParseQuery(const string& text) const;
    double CalcIDF(int document_count_, int document_count_with_word) const;
    vector<Document> FindAllDocuments(const Query& query_words) const;
};


void SearchServer::SetStopWords(const string& text) {
    for (const string& word : SplitIntoWords(text)) {
        stop_words_.insert(word);
    }
}

void SearchServer::AddDocument(int document_id, const string& document) {
    ++document_count_;
    const vector<string> words = SplitIntoWordsNoStop(document);

    for (const auto& word : words) {
        word_to_document_freqs_[word][document_id] += 1.0 / words.size();
    }

    /*count_if(words.begin(), words.end(), [&, this, document_id](string word) {
       word_to_document_freqs_[word][document_id] += 1.0 / words.size();
       return 0;
       });*/ //ну уж очень хотелось куда-то count_if влепить, главное не забыть при этом экземпляр класса захватить))
       // передача int по ссылке - не баг, а фича B)
}

vector<Document> SearchServer::FindTopDocuments(const string& raw_query) const {
    const Query query_words = ParseQuery(raw_query);
    auto matched_documents = FindAllDocuments(query_words);

    sort(matched_documents.begin(), matched_documents.end(),
        [](const Document& lhs, const Document& rhs) {
            return lhs.relevance > rhs.relevance;
        });
    if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
        matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
    }
    return matched_documents;
}


vector<string> SearchServer::SplitIntoWordsNoStop(const string& text) const {
    vector<string> words;
    for (const string& word : SplitIntoWords(text)) {
        if (!IsStopWord(word)) {
            words.push_back(word);
        }
    }
    return words;
}

SearchServer::Query SearchServer::ParseQuery(const string& text) const {
    Query query_words;
    for (const string& word : SplitIntoWordsNoStop(text)) {
        if (word[0] == '-') {
            string withoutMinus = word;
            withoutMinus.erase(0, 1); //.erase(1) сделает диаметрально противоположное действие: оставит лишь первый символ, остальные удалит)
            if (!stop_words_.count(withoutMinus))
                query_words.minusQuery.insert(withoutMinus);
        }
        else
            query_words.plusQuery.insert(word);
    }

    return query_words;
}

double SearchServer::CalcIDF(int document_count_, int document_count_with_word) const
{
    return log(static_cast<double>(document_count_) / document_count_with_word);
}


vector<Document> SearchServer::FindAllDocuments(const Query& query_words) const {
    map<int, double> documents_rel;
    map<int, double> id_freq;
    for (const auto& word : query_words.plusQuery) {
        if (word_to_document_freqs_.count(word)) {
            double idf = CalcIDF(document_count_, word_to_document_freqs_.at(word).size());
            for (const auto& [id, tf] : word_to_document_freqs_.at(word)) {
                documents_rel[id] += tf * idf;
            }
        }
    }
    for (const auto& word : query_words.minusQuery) {
        if (word_to_document_freqs_.count(word))
            for (const auto& id : word_to_document_freqs_.at(word))
                documents_rel.erase(id.first);
    }
    vector<Document> matched_documents;
    for (const auto& [id, rel] : documents_rel)
        matched_documents.push_back({ id, rel });

    return matched_documents;
}

SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());

    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }

    return search_server;
}

int main() {
    const SearchServer search_server = CreateSearchServer();

    const string query = ReadLine();
    for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << document_id << ", "
            << "relevance = "s << relevance << " }"s << endl;
    }
}