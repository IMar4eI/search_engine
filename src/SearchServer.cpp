#include "SearchServer.h"
#include <sstream>
#include <set>
#include <future>
#include <unordered_map>
#include <algorithm>
#include <iostream>

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

std::vector<std::vector<RelativeIndex>> SearchServer::search(const std::vector<std::string>& queries_input) {
  std::vector<std::future<std::vector<RelativeIndex>>> futures;
  std::vector<std::vector<RelativeIndex>> result;
  futures.reserve(queries_input.size());
  result.reserve(queries_input.size());

  // Process each query asynchronously
  for (const auto& query : queries_input) {
    futures.push_back(std::async(std::launch::async, [this, &query]() {
        try {
            return ProcessQuery(query);
        } catch (const std::exception& e) {
            std::cerr << "Error processing query '" << query << "': " << e.what() << std::endl;
            return std::vector<RelativeIndex>{};
        }
    }));
  }

  // Collect results from all futures
  for (auto& future : futures) {
    try {
      result.push_back(future.get());
    } catch (const std::exception& e) {
      std::cerr << "Error getting results from future: " << e.what() << std::endl;
      result.push_back({});
    }
  }

  return result;
}

/**
 * @brief Processes a single search query.
 * @param query The search query string.
 * @return Vector of RelativeIndex objects representing search results.
 */
std::vector<RelativeIndex> SearchServer::ProcessQuery(const std::string& query) {
  if (query.empty()) {
    throw std::invalid_argument("Received empty query.");
  }

  std::istringstream iss(query);
  std::string word;
  std::unordered_map<size_t, size_t> doc_to_count;
  std::set<std::string> unique_words;

  // Extract unique words from the query
  while (iss >> word) {
    unique_words.insert(word);
  }

  if (unique_words.empty()) {
    throw std::invalid_argument("Query contains no valid words.");
  }

  // Accumulate word counts for each document
  for (const auto& word : unique_words) {
    auto entries = _index.GetWordCount(word);
    for (const auto& entry : entries) {
      doc_to_count[entry.doc_id] += entry.count;
    }
  }

  if (doc_to_count.empty()) {
    // No documents found matching the query
    return {};
  }

  // Find the maximum absolute relevance
  size_t max_absolute_relevance = 0;
  for (const auto& [doc_id, count] : doc_to_count) {
    max_absolute_relevance = std::max(max_absolute_relevance, count);
  }

  if (max_absolute_relevance == 0) {
    throw std::runtime_error("Maximum absolute relevance is zero. Possible division by zero.");
  }

  std::vector<RelativeIndex> relative_indices;
  relative_indices.reserve(doc_to_count.size());

  // Calculate the relative relevance for each document
  for (const auto& [doc_id, count] : doc_to_count) {
    float rank = static_cast<float>(count) / static_cast<float>(max_absolute_relevance);
    relative_indices.push_back({ doc_id, rank });
  }

  // Use partial_sort to get top N results
  if (relative_indices.size() > static_cast<size_t>(_responses_limit)) {
    std::partial_sort(relative_indices.begin(),
      relative_indices.begin() + _responses_limit,
      relative_indices.end(),
      [](const RelativeIndex& a, const RelativeIndex& b) {
        if (a.rank == b.rank) {
          return a.doc_id < b.doc_id;
        }
        return a.rank > b.rank;
      });
    relative_indices.resize(_responses_limit);
  } else {
    // Sort all results
    std::sort(relative_indices.begin(), relative_indices.end(),
      [](const RelativeIndex& a, const RelativeIndex& b) {
        if (a.rank == b.rank) {
          return a.doc_id < b.doc_id;
        }
        return a.rank > b.rank;
      });
  }

  return relative_indices;
}



