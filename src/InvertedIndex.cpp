#include "InvertedIndex.h"
#include <sstream>
#include <future>
#include <unordered_map>
#include <iostream>

/**
  * Updates the document base and rebuilds the inverted index.
  * Processes documents in parallel to improve performance.
  * @param input_docs A vector containing the content of each document.
  */
void InvertedIndex::UpdateDocumentBase(const std::vector<std::string>& input_docs) {
  if (input_docs.empty()) {
    throw std::invalid_argument("Input documents list is empty.");
  }

  docs = input_docs;
  freq_dictionary.clear();

  std::vector<std::future<std::unordered_map<std::string, std::vector<Entry>>>> futures;

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

        // Local frequency dictionary for this document
        std::unordered_map<std::string, std::vector<Entry>> local_freq_dict;

        for (const auto& [word, count] : word_count_in_doc) {
            local_freq_dict[word].push_back({ doc_id, count });
        }

        return local_freq_dict;
    }));
  }

  // Collect results and merge into the main frequency dictionary
  for (auto& future : futures) {
    try {
      auto local_freq_dict = future.get();
      std::lock_guard<std::mutex> lock(freq_mutex);
      for (const auto& [word, entries] : local_freq_dict) {
        freq_dictionary[word].insert(freq_dictionary[word].end(), entries.begin(), entries.end());
      }
    } catch (const std::exception& e) {
      std::cerr << "Error updating document base: " << e.what() << std::endl;
    }
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


