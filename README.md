# Search Engine

**Search Engine** is a C++ application with a graphical user interface (GUI) built using Qt. It demonstrates efficient file parsing, indexing, and querying with a focus on usability and scalability. The project is developed and tested in CLion on macOS.

---

## 🚀 Features

1. **File Parsing**: Reads and processes JSON files (`config.json`, `requests.json`).
2. **Inverted Indexing**: Efficient word indexing for fast search.
3. **Query Processing**: Handles search queries with relevance scoring.
4. **JSON Export**: Outputs search results to `answers.json`.
5. **Graphical User Interface**:
   - Interactive file opening and search functionality.
   - JSON viewing in a user-friendly format.
   - Real-time animations for a polished user experience.

---

## 🛠️ Project Structure

```plaintext
search_engine/
├── include/               # Header files
│   ├── ConverterJSON.h    # Handles JSON parsing and file operations
│   ├── Entry.h            # Document word frequency structure
│   ├── InvertedIndex.h    # Manages inverted index
│   ├── MainWindow.h       # GUI main window
│   ├── RelativeIndex.h    # Document relevance structure
│   └── SearchServer.h     # Core search logic
├── src/                   # Source files
│   ├── ConverterJSON.cpp
│   ├── InvertedIndex.cpp
│   ├── MainWindow.cpp     # GUI main window logic
│   ├── SearchServer.cpp
│   └── main.cpp           # Application entry point
├── data/                  # Sample JSON files
│   ├── config.json        # Configuration for indexing
│   ├── requests.json      # Search queries
│   └── answers.json       # Search results
├── resources/             # GUI resources (styles, icons, etc.)
├── tests/                 # Unit tests
├── docs/                  # Documentation
├── CMakeLists.txt         # Build configuration
└── README.md              # This file

🧰 Prerequisites

Ensure the following tools are installed:
	1.	CLion IDE: Tested with CLion on macOS.
	2.	C++ Compiler: Support for C++20.
	3.	Qt Framework: Version 6.8 or higher.
	4.	CMake: Version 3.21 or higher.
	5.	GoogleTest: Integrated with CMake for unit testing.

🔧 Build and Run

Follow these steps to build and run the project in CLion:
	1.	Clone the repository:
    git clone https://github.com/your-username/search_engine.git
    cd search_engine
  2.	Open in CLion:
    •	Open the project directory in CLion.
  3.	Configure CMake:
	  •	Ensure the correct Qt paths are set in the CMakeLists.txt file:
    find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
  4.	Build the project:
	  •	Use the “Build” option in CLion to compile the project.
	5.	Run the application:
	  •	Use the “Run” option in CLion to launch the application.

🖥️ Usage

GUI Features:

	1.	Open File:
	•	Click the “Open JSON File” button to load and display a JSON file’s content.
	2.	Search Requests:
	•	Click the “Search” button to process search queries from requests.json and display the results.

Command-Line Execution:

For non-GUI functionality, run the application directly in the terminal. The program will:
	•	Process files based on config.json.
	•	Handle queries from requests.json.
	•	Output results to answers.json.

🧪 Testing

Unit tests verify the core components (e.g., InvertedIndex, SearchServer).
	1.	Build tests:
	  •	Configure the project to include unit_tests.
      cmake --build build --target unit_tests
  2.	Run tests:
    ./build/unit_tests

✨ Highlights

	•	Performance: Optimized indexing with multithreading.
	•	Scalability: Modular design for handling large datasets.
	•	GUI Enhancements:
	•	User-friendly interface with JSON display.
	•	Animations for a polished experience.

💻 Notes

	•	Development Environment:
	•	The project was developed and tested in CLion on macOS.
	•	Compatibility with other systems (Linux/Windows) is untested.

🤝 Contributions
  Contributions are welcome! Submit issues or pull requests to enhance the project.
