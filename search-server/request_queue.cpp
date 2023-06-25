#include "request_queue.h"
#include <algorithm>

//public
std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentStatus status) {
    auto result = search_server_.FindTopDocuments(raw_query, status);
    addResult(raw_query, result);
    return result;
}

std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query) {
    auto result = search_server_.FindTopDocuments(raw_query);
    addResult(raw_query, result);
    return result;
}

int RequestQueue::GetNoResultRequests() const {
    return std::count_if(requests_.begin(), requests_.end(), [](const QueryResult& request) {
        if (request.result.empty()) {
            return true;
        }
        return false;
        });;
}


//private
void RequestQueue::addResult(const std::string& query, const std::vector<Document>& result) {
    if (requests_.size() == min_in_day_) {
        requests_.pop_front();
    }
    requests_.push_back({ query, result });
}