#include "gtest/gtest.h"

#include "InvertedIndex.h"
#include "SearchServer.h"

/**
 * @brief Helper function to test the functionality of InvertedIndex.
 * @param docs Collection of documents to be indexed.
 * @param requests Search queries.
 * @param expected Expected results for each query.
 */
void TestInvertedIndexFunctionality(
    const std::vector<std::string>& docs,
    const std::vector<std::string>& requests,
    const std::vector<std::vector<Entry>>& expected
) {
  std::vector<std::vector<Entry>> result;
  InvertedIndex idx;
  idx.UpdateDocumentBase(docs);

  for (const auto& request : requests) {
    std::vector<Entry> word_count = idx.GetWordCount(request);
    result.push_back(word_count);
  }

  ASSERT_EQ(result, expected);
}

/**
 * @brief Basic test for InvertedIndex with simple documents and queries.
 */

TEST(TestCaseInvertedIndex, TestBasic) {
  const std::vector<std::string> docs = {
    "london is the capital of great britain",
    "big ben is the nickname for the Great bell of the striking clock"
};
  const std::vector<std::string> requests = { "london", "the" };
  const std::vector<std::vector<Entry>> expected = {
    { {0, 1} },
    { {0, 1}, {1, 3} }
  };
  TestInvertedIndexFunctionality(docs, requests, expected);
}

/**
 * @brief Additional test for InvertedIndex with multiple occurrences.
 */
TEST(TestCaseInvertedIndex, TestBasic2) {
  const std::vector<std::string> docs = {
    "milk milk milk milk water water water",
    "milk water water",
    "milk milk milk milk milk water water water water water",
    "americano cappuccino"
};
  const std::vector<std::string> requests = { "milk", "water", "cappuccino" };
  const std::vector<std::vector<Entry>> expected = {
    { {0, 4}, {1, 1}, {2, 5} },
    { {0, 3}, {1, 2}, {2, 5} },
    { {3, 1} }
  };
  TestInvertedIndexFunctionality(docs, requests, expected);
}

/**
 * @brief Test to ensure correct behavior when a word is missing from the documents.
 */
TEST(TestCaseInvertedIndex, TestMissingWord) {
  const std::vector<std::string> docs = {
    "a b c d e f g h i j k l", "statement"
};
  const std::vector<std::string> requests = { "m", "statement" };
  const std::vector<std::vector<Entry>> expected = {
    {},
    { {1, 1} }
  };
  TestInvertedIndexFunctionality(docs, requests, expected);
}

/**
 * @brief Simple test for SearchServer to verify search results.
 */
TEST(TestCaseSearchServer, TestSimple) {
  const std::vector<std::string> docs = {
    "milk milk milk milk water water water",
    "milk water water",
    "milk milk milk milk milk water water water water water",
    "americano cappuccino"
};
  const std::vector<std::string> request = { "milk water", "sugar" };
  const std::vector<std::vector<RelativeIndex>> expected = {
    { {2, 1}, {0, 0.7f}, {1, 0.3f} },
    {}
  };
  InvertedIndex idx;
  idx.UpdateDocumentBase(docs);
  SearchServer srv(idx);
  std::vector<std::vector<RelativeIndex>> result = srv.search(request);
  ASSERT_EQ(result, expected);
}

/**
 * @brief Test for SearchServer to verify top N results functionality.
 */
TEST(TestCaseSearchServer, TestTop5) {
  const std::vector<std::string> docs = {
    "london is the capital of great britain",
    "paris is the capital of france",
    "berlin is the capital of germany",
    "rome is the capital of italy",
    "madrid is the capital of spain",
    "lisboa is the capital of portugal",
    "bern is the capital of switzerland",
    "moscow is the capital of russia",
    "kiev is the capital of ukraine",
    "minsk is the capital of belarus",
    "astana is the capital of kazakhstan",
    "beijing is the capital of china",
    "tokyo is the capital of japan",
    "bangkok is the capital of thailand",
    "welcome to moscow the capital of russia the third rome",
    "amsterdam is the capital of netherlands",
    "helsinki is the capital of finland",
    "oslo is the capital of norway",
    "stockholm is the capital of sweden",
    "riga is the capital of latvia",
    "tallinn is the capital of estonia",
    "warsaw is the capital of poland"
};
  const std::vector<std::string> request = { "moscow is the capital of russia" };
  const std::vector<std::vector<RelativeIndex>> expected = {
    {
      {7, 1},
      {14, 1},
      {0, 0.6666667f},
      {1, 0.6666667f},
      {2, 0.6666667f}
    }
  };
  InvertedIndex idx;
  idx.UpdateDocumentBase(docs);
  SearchServer srv(idx);
  std::vector<std::vector<RelativeIndex>> result = srv.search(request);
  ASSERT_EQ(result, expected);
}

/**
 * @brief Test to check behavior with an empty query.
 */
TEST(SearchServerTest, EmptyQuery) {
  const std::vector<std::string> docs = {
    "milk water sugar",
    "coffee tea milk"
};
  const std::vector<std::string> requests = {""}; // Empty query
  const std::vector<std::vector<RelativeIndex>> expected = {{}};

  InvertedIndex idx;
  idx.UpdateDocumentBase(docs);

  SearchServer server(idx);
  auto result = server.search(requests);

  ASSERT_EQ(result, expected);
}

/**
 * @brief Test to verify performance with a large dataset.
 */
TEST(SearchServerTest, PerformanceWithLargeData) {
  const size_t num_docs = 100000; // 100,000 documents
  const size_t num_queries = 1000; // 1,000 queries

  std::vector<std::string> docs(num_docs, "lorem ipsum dolor sit amet consectetur adipiscing elit");
  std::vector<std::string> requests(num_queries, "lorem ipsum");

  InvertedIndex idx;
  idx.UpdateDocumentBase(docs);

  SearchServer server(idx);

  // Measure execution time
  auto start = std::chrono::high_resolution_clock::now();
  auto result = server.search(requests);
  auto end = std::chrono::high_resolution_clock::now();

  // Ensure execution time is within reasonable limits (e.g., 1 second)
  ASSERT_LT(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(), 1000);

  // Verify the number of results matches the number of queries
  ASSERT_EQ(result.size(), num_queries);
}

/**
 * @brief Test to check parallel processing of multiple queries.
 */
TEST(SearchServerTest, ParallelProcessing) {
  const std::vector<std::string> docs = {
    "apple banana orange",
    "apple apple banana",
    "orange banana apple"
};

  const std::vector<std::string> requests = {
    "apple banana",
    "orange banana",
    "apple orange banana"
};

  InvertedIndex idx;
  idx.UpdateDocumentBase(docs);

  SearchServer server(idx);
  auto result = server.search(requests);

  ASSERT_EQ(result.size(), 3);
  ASSERT_EQ(result[0].size(), 3); // 3 documents in the results
  ASSERT_EQ(result[1].size(), 3);
  ASSERT_EQ(result[2].size(), 3);
}

TEST(TestCaseInvertedIndex, TestCaseSensitivity) {
  const std::vector<std::string> docs = {
    "Apple banana orange",
    "apple Banana ORANGE",
    "APPLE BANANA ORANGE"
};
  const std::vector<std::string> requests = { "apple", "Banana", "ORANGE" };
  const std::vector<std::vector<Entry>> expected = {
    { {0, 1}, {1, 1}, {2, 1} },
    { {0, 1}, {1, 1}, {2, 1} },
    { {0, 1}, {1, 1}, {2, 1} }
  };
  TestInvertedIndexFunctionality(docs, requests, expected);
}

TEST(TestCaseInvertedIndex, TestPunctuation) {
  const std::vector<std::string> docs = {
    "Hello, world!",
    "Hello... world?",
    "Hello - world; hello: world."
};
  const std::vector<std::string> requests = { "hello", "world" };
  const std::vector<std::vector<Entry>> expected = {
    { {0, 1}, {1, 1}, {2, 2} },
    { {0, 1}, {1, 1}, {2, 2} }
  };
  TestInvertedIndexFunctionality(docs, requests, expected);
}

TEST(TestCaseInvertedIndex, TestEmptyDocuments) {
  const std::vector<std::string> docs = {
    "",
    "   ",
    "word"
};
  const std::vector<std::string> requests = { "word" };
  const std::vector<std::vector<Entry>> expected = {
    { {2, 1} }
  };
  TestInvertedIndexFunctionality(docs, requests, expected);
}

TEST(TestCaseInvertedIndex, TestLargeDocument) {
  std::string large_doc(1000000, 'a'); // Документ из 1 миллиона символов 'a'
  const std::vector<std::string> docs = { large_doc };
  const std::vector<std::string> requests = { "a" };
  const std::vector<std::vector<Entry>> expected = {
    { {0, 1000000} }
  };
  TestInvertedIndexFunctionality(docs, requests, expected);
}

TEST(TestCaseInvertedIndex, TestInvalidInput) {
  InvertedIndex idx;
  ASSERT_THROW(idx.UpdateDocumentBase({}), std::invalid_argument);
}

TEST(SearchServerTest, TestEmptyQuery) {
  InvertedIndex idx;
  idx.UpdateDocumentBase({ "document" });
  SearchServer srv(idx);
  ASSERT_THROW(srv.search({ "" }), std::invalid_argument);
}

TEST(SearchServerTest, TestResponsesLimit) {
  const std::vector<std::string> docs = {
    "apple banana",
    "apple banana cherry",
    "apple banana cherry date",
    "apple banana cherry date egg"
};
  const std::vector<std::string> requests = { "apple banana cherry date egg" };
  const std::vector<std::vector<RelativeIndex>> expected = {
    { {3, 1.0f}, {2, 0.8f}, {1, 0.6f} }
  };
  InvertedIndex idx;
  idx.UpdateDocumentBase(docs);
  SearchServer srv(idx, 3);
  auto result = srv.search(requests);
  ASSERT_EQ(result, expected);
}



