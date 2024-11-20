#pragma once

#include <vector>
#include <string>
#include "RelativeIndex.h"
#include "InvertedIndex.h"

/**
 * @brief Implements a search server that processes queries using an inverted index.
 */
class SearchServer {
  public:
  /**
   * @brief Constructs a SearchServer with a reference to an InvertedIndex.
   * @param idx Reference to an existing InvertedIndex object.
   * @param responses_limit Maximum number of responses to return for a query.
   */
    SearchServer(InvertedIndex& idx, int responses_limit = 5);

 /**
  * @brief Processes a list of search queries.
  * @param queries_input Vector of search query strings.
  * @return Vector of vectors containing RelativeIndex objects for each query.
  */
    std::vector<std::vector<RelativeIndex>> search(const std::vector<std::string>& queries_input);
  private:
    InvertedIndex& _index; // Reference to the inverted index.
    int _responses_limit; // Maximum number of responses per query.

  /**
   * @brief Processes a single search query.
   * @param query The search query string.
   * @return Vector of RelativeIndex objects representing search results.
   */
    std::vector<RelativeIndex> ProcessQuery(const std::string& query);
};

