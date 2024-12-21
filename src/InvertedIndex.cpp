#include "InvertedIndex.h"
#include <sstream>
#include <future>
#include <unordered_map>
#include <iostream>
#include <cctype>

std::string InvertedIndex::CleanWord(const std::string& word) {
  std::string clean_word;
  for (char c : word) {
    if (std::isalnum(c)) {
      clean_word += std::tolower(c);
    }
  }
  return clean_word;
}

/**
 * @brief Updates the document base and rebuilds the inverted index.
 * Processes documents in parallel to improve performance.
 * @param input_docs A vector containing the content of each document.
 */
void InvertedIndex::UpdateDocumentBase(const std::vector<std::string>& input_docs) {
  if (input_docs.empty()) {
    throw std::invalid_argument("Input documents list is empty.");
  }

  docs = input_docs;
  freq_dictionary.clear();

  size_t num_threads = std::thread::hardware_concurrency();
  if (num_threads == 0) num_threads = 2; // Fallback if hardware_concurrency() can't be determine the number

  std::vector<std::future<std::unordered_map<std::string, std::vector<Entry>>>> futures;
  futures.reserve(num_threads);

  size_t total_docs = docs.size();
  size_t docs_per_thread = (total_docs + num_threads - 1) / num_threads;

  // Divide documents into blocks and process each block in a separate thread
  for (size_t i = 0; i < num_threads; ++i) {
    size_t start_doc = i * docs_per_thread;
    if (start_doc >= total_docs) break;
    size_t end_doc = std::min(start_doc + docs_per_thread, total_docs);

    futures.emplace_back(std::async(std::launch::async, [this, start_doc, end_doc]() {
        std::unordered_map<std::string, std::vector<Entry>> local_freq_dict;

        for (size_t doc_id = start_doc; doc_id < end_doc; ++doc_id) {
          std::istringstream iss(docs[doc_id]);
          std::string word;
          std::unordered_map<std::string, size_t> word_count_in_doc;

          // Count occurrences of each word in the document
          while (iss >> word) {
            word = CleanWord(word);
            ++word_count_in_doc[word];
          }

          // Populate the local frequency dictionary
          for (const auto& [word, count] : word_count_in_doc) {
            local_freq_dict[word].push_back({ doc_id, count });
          }
        }

        return local_freq_dict;
    }));
  }

  // Temporary structure to store combined results
  std::unordered_map<std::string, std::vector<Entry>> combined_freq_dictionary;

  // Merge results from all threads
  for (auto& future : futures) {
    try {
      auto local_freq_dict = future.get();
      for (const auto& [word, entries] : local_freq_dict) {
        combined_freq_dictionary[word].insert(
          combined_freq_dictionary[word].end(),
          std::make_move_iterator(entries.begin()),
          std::make_move_iterator(entries.end())
        );
      }
    } catch (const std::exception& e) {
      std::cerr << "Error updating document base: " << e.what() << std::endl;
    }
  }

  // Update the main frequency dictionary
  freq_dictionary = std::move(combined_freq_dictionary);
}


/**
 * @brief Retrieves the frequency of a word across all documents.
 * @param word The word to search for.
 * @return A vector of Entry objects containing document IDs and counts.
 */
std::vector<Entry> InvertedIndex::GetWordCount(const std::string& word) {
  std::string clean_word = CleanWord(word);
  auto it = freq_dictionary.find(clean_word);
  if (it != freq_dictionary.end()) {
    return it->second;
  }
  return {};
}



