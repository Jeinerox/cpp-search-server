#pragma once
#include <vector>

template <typename Iterator>
class IteratorRange {
private:
    Iterator begin_, end_;
public:
    IteratorRange(Iterator begin, Iterator end) : begin_(begin), end_(end) {}
    Iterator begin() const { return begin_; }
    Iterator end() const { return end_; }
    size_t size() const { return end_ - begin_; }
};

template <typename Iterator>
class Paginator {
private:
    std::vector<IteratorRange<Iterator>> pages;
public:
    Paginator(Iterator begin, Iterator end, size_t page_size) {
        for (auto it = begin; it != end; ) {
            size_t current_page_size = min(page_size, static_cast<size_t>(distance(it, end)));
            Iterator page_end = next(it, current_page_size);
            pages.push_back({ it, page_end });
            it = page_end;
        }
    }
    auto begin() const { return pages.begin(); }
    auto end() const { return pages.end(); }
};

template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}
