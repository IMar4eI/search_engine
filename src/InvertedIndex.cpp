#include "InvertedIndex.h"
#include <sstream>
#include <future>
#include <unordered_map>

/**
  * Updates the document base and rebuilds the inverted index.
  * Processes documents in parallel to improve performance.
  * @param input_docs A vector containing the content of each document.
  */
void InvertedIndex::UpdateDocumentBase(const std::vector<std::string>& input_docs) {
  docs = input_docs;
  freq_dictionary.clear();

  std::vector<std::future<void>> futures;

  // Process each document asynchronously
  for (size_t doc_id = 0; doc_id < docs.size(); ++doc_id) {
    futures.push_back(std::async(std::launch::async, [this, doc_id]() {
      std::istringstream iss(docs[doc_id]);
      std::string word;
      std::unordered_map<std::string, size_t> word_count_in_doc;

      // Count occurrences of each word in the document
      while (iss >> word) {
        word_count_in_doc[word]++;
      }

      // Update the frequency dictionary safely
      {
        std::lock_guard<std::mutex> lock(freq_mutex);
        for (const auto& [word, count] : word_count_in_doc) {
          freq_dictionary[word].push_back({doc_id, count});
        }
      }
    }));
  }

  // Wait for all asynchronous tasks to complete
  for (auto& future : futures) {
    future.get();
  }
}

/**
  * Retrieves the frequency of a word across all documents.
  * @param word The word to search for.
  * @return A vector of Entry objects containing document IDs and counts.
  */
std::vector<Entry> InvertedIndex::GetWordCount(const std::string& word) {
  auto it = freq_dictionary.find(word);
  if (it != freq_dictionary.end()) {
    return it->second;
  }
  return {};
}


