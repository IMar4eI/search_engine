#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <mutex>
#include "Entry.h"

/**
 * @brief Builds an inverted index from a collection of documents.
 */
class InvertedIndex {
  public:
    InvertedIndex() = default;

    /**
     * Updates the document base and rebuilds the inverted index.
     * @param input_docs A vector containing the content of each document.
     */
    void UpdateDocumentBase(const std::vector<std::string>& input_docs);

    /**
     * Retrieves the list of entries (document IDs and counts) for a given word.
     * @param word The word to search for.
     * @return A vector of Entry objects containing document IDs and word counts.
     */
    std::vector<Entry> GetWordCount(const std::string& word);

    /**
     * Gets the total number of documents.
     * @return The number of documents.
     */
    size_t GetDocsSize() const { return docs.size(); }

  private:
    std::vector<std::string> docs; // List of document contents.
    std::unordered_map<std::string, std::vector<Entry>> freq_dictionary; // Frequency dictionary (inverted index).
    std::mutex freq_mutex; // Mutex to protect freq_dictionary during updates.
};