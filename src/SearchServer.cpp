#include "SearchServer.h"
#include <sstream>
#include <set>
#include <future>
#include <unordered_map>
#include <algorithm>

/**
  * @brief Constructs a SearchServer with a reference to an InvertedIndex.
  * @param idx Reference to an existing InvertedIndex object.
  * @param responses_limit Maximum number of responses to return for a query.
  */
SearchServer::SearchServer(InvertedIndex& idx, int responses_limit)
  : _index(idx), _responses_limit(responses_limit) {}

/**
 * @brief Processes a list of search queries asynchronously.
 * @param queries_input Vector of search query strings.
 * @return Vector of vectors containing RelativeIndex objects for each query.
 */

std::vector<std::vector<RelativeIndex> > SearchServer::search(const std::vector<std::string>& queries_input) {
  std::vector<std::future<std::vector<RelativeIndex>>> futures;
  std::vector<std::vector<RelativeIndex>> result;

  // Process each query asynchronously
  for (const auto& query :queries_input) {
    futures.push_back(std::async(std::launch::async, [this, query]() {
      return ProcessQuery(query);
    }));
  }

  // Collect results from all futures
  for (auto& future : futures) {
    result.push_back(future.get());
  }

  return result;
}

/**
 * @brief Processes a single search query.
 * @param query The search query string.
 * @return Vector of RelativeIndex objects representing search results.
 */
std::vector<RelativeIndex> SearchServer::ProcessQuery(const std::string& query) {
  std::istringstream iss(query);
  std::string word;
  std::unordered_map<size_t, size_t> doc_to_count;
  std::set<std::string> unique_words;

  // Extract unique words from the query
  while(iss >> word) unique_words.insert(word);

  // Accumulate word counts for each document
  for (const auto& word : unique_words) {
    auto entries = _index.GetWordCount(word);
    for (const auto& entry : entries) {
      doc_to_count[entry.doc_id] += entry.count;
    }
  }

  // Find the maximum absolute relevance
  size_t max_absolute_relevance = 0;
  for (const auto& [doc_id, count] : doc_to_count) {
    max_absolute_relevance = std::max(max_absolute_relevance, count);
  }

  std::vector<RelativeIndex> relative_indices;

  // Calculate the relative relevance for each document
  for (const auto& [doc_id, count] : doc_to_count) {
    relative_indices.push_back({doc_id, static_cast<float>(count) / max_absolute_relevance});
  }

  // Sort results by rank in descending order
  std::sort(relative_indices.begin(), relative_indices.end(),
    [](const RelativeIndex& a, const RelativeIndex& b) {
      return b.rank > a.rank;
    });

  // Limit the number of responses to _responses_limit
  if (relative_indices.size() > static_cast<size_t>(_responses_limit)) {
    relative_indices.resize(_responses_limit);
  }

  return relative_indices;
}



