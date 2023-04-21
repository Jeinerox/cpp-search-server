void TestExcludeStopWordsFromAddedDocumentContent() {
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = { 1, 2, 3 };
    // Сначала убеждаемся, что поиск слова, не входящего в список стоп-слов,
    // находит нужный документ
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("in"s);
        ASSERT(found_docs.size() == 1);
        const Document& doc0 = found_docs[0];
        ASSERT(doc0.id == doc_id);
    }

    // Затем убеждаемся, что поиск этого же слова, входящего в список стоп-слов,
    // возвращает пустой результат
    {
        SearchServer server;
        server.SetStopWords("in the"s);
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        ASSERT(server.FindTopDocuments("in"s).empty());
    }
}

void TestIfAddingDocumentsCorrect() {
    const int doc_id1 = 78;
    const string content1 = "cat in the city"s;
    const vector<int> ratings1 = { 1, 1, 1 };
    const int doc_id2 = 79;
    const string content2 = "dog in the car"s;
    const vector<int> ratings2 = { 1, 2, 3 };
    {
        SearchServer server;
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        const auto found_docs = server.FindTopDocuments("cat"s);
        ASSERT_EQUAL(found_docs.size(), 1);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id1);
    }
}

void TestIfMinusWordsCorrect() { //работают ли минус слова
    const int doc_id1 = 1;
    const string content1 = "cat in the car"s;
    const vector<int> ratings1 = { 1, 1, 1 };
    const int doc_id2 = 2;
    const string content2 = "dog in the car"s;
    const vector<int> ratings2 = { 1, 2, 3 };
    {
        SearchServer server;
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        const auto found_docs = server.FindTopDocuments("car -dog"s);
        ASSERT_EQUAL(found_docs.size(), 1);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id1);
    }
}

void TestIfMatchingCorrect() {
    const int doc_id1 = 1;
    const string content1 = "cat in the car in London"s;
    const vector<int> ratings1 = { 1, 1, 1 };
    const int doc_id2 = 2;
    const string content2 = "dog in the car in London"s;
    const vector<int> ratings2 = { 1, 2, 3 };
    {
        SearchServer server;
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::REMOVED, ratings2);
        const auto found_docs1 = server.MatchDocument("car London -dog"s, 1);
        const auto found_docs2 = server.MatchDocument("car London -dog"s, 2);

        tuple<vector<string>, DocumentStatus> expected1 = { {"London"s, "car"s}, DocumentStatus::ACTUAL };
        tuple<vector<string>, DocumentStatus> expected2 = { {},{DocumentStatus::REMOVED} };

        ASSERT_EQUAL(found_docs1, expected1);
        ASSERT_EQUAL(found_docs2, expected2);

    }
}

void TestIfSortByRatingCorrect() {
    const int doc_id1 = 1;
    const string content1 = "cat in the car in London"s;
    const vector<int> ratings1 = { 1, 1, 1 };
    const int doc_id2 = 2;
    const string content2 = "dog in the car in London"s;
    const vector<int> ratings2 = { 1, 2, 3 };
    const int doc_id3 = 3;
    const string content3 = "dog in London"s;
    const vector<int> ratings3 = { 5, 2, 3 };
    {
        SearchServer server;
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        server.AddDocument(doc_id3, content3, DocumentStatus::ACTUAL, ratings3);
        const auto found_docs1 = server.FindTopDocuments("London"s);
        ASSERT(found_docs1.size() == 3);

        const Document& doc0 = found_docs1[0];
        const Document& doc1 = found_docs1[1];
        const Document& doc2 = found_docs1[2];
        ASSERT_HINT(doc0.id == doc_id3, "sort wrong"s);
        ASSERT_HINT(doc1.id == doc_id2, "sort wrong"s);
        ASSERT_HINT(doc2.id == doc_id1, "sort wrong"s);
    }
}

void TestIfRatingComputeCorrect() {
    const int doc_id1 = 1;
    const string content1 = "cat in the car in London"s;
    const vector<int> ratings1 = { 1, 1, 1 };
    const int doc_id2 = 2;
    const string content2 = "dog in the car in London"s;
    const vector<int> ratings2 = { 2, 2, 2 };
    const int doc_id3 = 3;
    const string content3 = "dog in London"s;
    const vector<int> ratings3 = { 20, 10, 3 };
    {
        SearchServer server;
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        server.AddDocument(doc_id3, content3, DocumentStatus::ACTUAL, ratings3);
        const auto found_docs1 = server.FindTopDocuments("London"s);
        ASSERT_HINT(found_docs1.size() == 3, "size wrong");
        const Document& doc0 = found_docs1[0];
        const Document& doc1 = found_docs1[1];
        const Document& doc2 = found_docs1[2];
        ASSERT_HINT(doc0.rating == 11, "rating wrong"s);
        ASSERT_HINT(doc1.rating == 2, "rating wrong"s);
        ASSERT_HINT(doc2.rating == 1, "rating wrong"s);
    } {
    }
}

void TestIsPredicateCorrect() {
    const int doc_id1 = 1;
    const string content1 = "cat in the car in London"s;
    const vector<int> ratings1 = { 1, 1, 1 };
    const int doc_id2 = 2;
    const string content2 = "dog in the car in London"s;
    const vector<int> ratings2 = { 2, 2, 2 };
    const int doc_id3 = 3;
    const string content3 = "dog in the car in London"s;
    const vector<int> ratings3 = { 6, 7, 8 };
    {
        SearchServer server;
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        server.AddDocument(doc_id3, content3, DocumentStatus::ACTUAL, ratings3);
        const auto found_docs1 = server.FindTopDocuments("London"s, [](int document_id, DocumentStatus status, int rating) {
            return document_id > 1 && rating > 5;
            });
        ASSERT_EQUAL_HINT(found_docs1.size(), 1, "predicate wrong"s);
        const Document& doc1 = found_docs1[0];
        ASSERT_EQUAL_HINT(doc1.id, doc_id3, "predicate wrong"s);
    }
}

void TestIfStatusSearchCorrect() {
    const int doc_id1 = 1;
    const string content1 = "cat in the car in London"s;
    const vector<int> ratings1 = { 1, 1, 1 };
    const int doc_id2 = 2;
    const string content2 = "dog in the car in London"s;
    const vector<int> ratings2 = { 2, 2, 2 };
    const int doc_id3 = 3;
    const string content3 = "dog in London"s;
    const vector<int> ratings3 = { 6, 7, 9 };
    {
        SearchServer server;
        server.AddDocument(doc_id1, content1, DocumentStatus::IRRELEVANT, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::REMOVED, ratings2);
        server.AddDocument(doc_id3, content3, DocumentStatus::IRRELEVANT, ratings3);
        const auto found_docs1 = server.FindTopDocuments("London", DocumentStatus::IRRELEVANT);
        ASSERT(found_docs1.size() == 2);
        const Document& doc1 = found_docs1[0];
        const Document& doc2 = found_docs1[1];
        ASSERT(doc1.id == doc_id3);
        ASSERT(doc2.id == doc_id1);
    }
}

void TestIfRelevationCorrect() {
    const int doc_id1 = 1;
    const string content1 = "black cat in London city founded"s;
    const vector<int> ratings1 = { 1, 1, 1 };
    const int doc_id2 = 2;
    const string content2 = "cat in the car in London pictures"s;
    const vector<int> ratings2 = { 2, 2, 2 };
    const int doc_id3 = 3;
    const string content3 = "dog in London"s;
    const vector<int> ratings3 = { 6, 7, 9 };
    const int doc_id4 = 4;
    const string content4 = "best black cars low price London"s;
    const vector<int> ratings4 = { 6, 7, 9 };

    const double EPSILON = 1e-6;
    {
        SearchServer server;
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        server.AddDocument(doc_id3, content3, DocumentStatus::ACTUAL, ratings3);
        server.AddDocument(doc_id4, content4, DocumentStatus::ACTUAL, ratings4);
        const auto found_docs1 = server.FindTopDocuments("London cat founded"s);
        ASSERT(found_docs1.size() == 4);
        const Document& doc1 = found_docs1[0];
        const Document& doc2 = found_docs1[1];
        const Document& doc3 = found_docs1[2];
        const Document& doc4 = found_docs1[3];
        ASSERT(abs(doc1.relevance - 0.3465735902) < EPSILON);
        ASSERT(abs(doc2.relevance - 0.0990210257) < EPSILON);
        ASSERT(abs(doc3.relevance - 0) < EPSILON);
        ASSERT(abs(doc4.relevance - 0) < EPSILON);

    }
}
// Функция TestSearchServer является точкой входа для запуска тестов

void TestSearchServer() {
    TestExcludeStopWordsFromAddedDocumentContent();
    TestIfAddingDocumentsCorrect();
    TestIfMinusWordsCorrect();
    TestIfMatchingCorrect();
    TestIfSortByRatingCorrect();
    TestIfRatingComputeCorrect();
    TestIsPredicateCorrect();
    TestIfStatusSearchCorrect();
    TestIfRelevationCorrect();
    // Не забудьте вызывать остальные тесты здесь
}