#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cctype>

const int GRID_SIZE = 20;

// Directions to move in the grid
const int directions[8][2] = {
    {-1,  0}, // Up
    {-1,  1}, // Up-Right
    { 0,  1}, // Right
    { 1,  1}, // Down-Right
    { 1,  0}, // Down
    { 1, -1}, // Down-Left
    { 0, -1}, // Left
    {-1, -1}  // Up-Left
};

// Function declarations
bool loadPuzzle(const std::string& filename, std::vector<std::string>& grid, std::vector<std::string>& words);
bool findWord(const std::vector<std::string>& grid, const std::string& word, std::vector<std::vector<bool>>& foundPositions);
bool searchFromPosition(const std::vector<std::string>& grid, const std::string& word, int index,
                        int row, int col, int dirRow, int dirCol, std::vector<std::vector<bool>>& path);
void markWordPositions(std::vector<std::vector<bool>>& foundPositions, const std::vector<std::vector<bool>>& path);
void printSolution(const std::vector<std::string>& grid, const std::vector<std::vector<bool>>& foundPositions);

int main() {
    std::string filename;
    std::cout << "Enter the puzzle filename to solve: ";
    std::getline(std::cin, filename);

    // Load the puzzle
    std::vector<std::string> grid;
    std::vector<std::string> words;
    if (!loadPuzzle(filename, grid, words)) {
        std::cerr << "Failed to load puzzle from file: " << filename << std::endl;
        return 1;
    }

    // Initialize a grid to keep track of found positions
    std::vector<std::vector<bool>> foundPositions(GRID_SIZE, std::vector<bool>(GRID_SIZE, false));

    // Solve the puzzle
    for (const auto& word : words) {
        if (!findWord(grid, word, foundPositions)) {
            std::cout << "Word not found: " << word << std::endl;
        }
    }

    // Print the solution
    std::cout << "\nSolved Puzzle:\n" << std::endl;
    printSolution(grid, foundPositions);

    return 0;
}

bool loadPuzzle(const std::string& filename, std::vector<std::string>& grid, std::vector<std::string>& words) {
    std::ifstream infile(filename);
    if (!infile) {
        std::cerr << "Unable to open puzzle file: " << filename << std::endl;
        return false;
    }

    std::string line;
    bool readingGrid = false;
    bool readingWords = false;

    while (std::getline(infile, line)) {
        if (line == "GRID:") {
            readingGrid = true;
            readingWords = false;
            std::getline(infile, line); // Skip column numbers
            continue;
        } else if (line == "WORDS TO FIND:") {
            readingGrid = false;
            readingWords = true;
            continue;
        }

        if (readingGrid) {
            // Parse grid lines
            if (line.empty()) continue;
            // Remove leading row numbers and spaces
            size_t pos = line.find_first_not_of(" ");
            if (pos != std::string::npos) {
                pos = line.find_first_of(" ", pos); // Skip row number
                if (pos != std::string::npos) {
                    std::string rowData = line.substr(pos);
                    // Remove spaces between letters
                    rowData.erase(remove(rowData.begin(), rowData.end(), ' '), rowData.end());
                    if (!rowData.empty()) {
                        grid.push_back(rowData);
                    }
                }
            }
        } else if (readingWords) {
            // Parse words to find
            if (!line.empty()) {
                // Convert word to uppercase
                std::transform(line.begin(), line.end(), line.begin(), ::toupper);
                words.push_back(line);
            }
        }
    }

    infile.close();

    if (grid.size() != GRID_SIZE) {
        std::cerr << "Invalid grid size in puzzle file." << std::endl;
        return false;
    }

    return true;
}

bool findWord(const std::vector<std::string>& grid, const std::string& word, std::vector<std::vector<bool>>& foundPositions) {
    int n = grid.size();

    // Iterate over each cell in the grid
    for (int row = 0; row < n; ++row) {
        for (int col = 0; col < n; ++col) {
            // If the first letter matches
            if (grid[row][col] == word[0]) {
                // Try all 8 directions
                for (const auto& dir : directions) {
                    std::vector<std::vector<bool>> path(n, std::vector<bool>(n, false));
                    if (searchFromPosition(grid, word, 0, row, col, dir[0], dir[1], path)) {
                        // Mark the positions of the found word
                        markWordPositions(foundPositions, path);
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool searchFromPosition(const std::vector<std::string>& grid, const std::string& word, int index,
                        int row, int col, int dirRow, int dirCol, std::vector<std::vector<bool>>& path) {
    int n = grid.size();

    // Base case: If index is equal to word length, word is found
    if (index == word.length()) {
        return true;
    }

    // Check boundaries
    if (row < 0 || row >= n || col < 0 || col >= n) {
        return false;
    }

    // If the cell does not match the current character
    if (grid[row][col] != word[index]) {
        return false;
    }

    // Mark the current cell as part of the path
    path[row][col] = true;

    // Move to the next character in the specified direction
    return searchFromPosition(grid, word, index + 1, row + dirRow, col + dirCol, dirRow, dirCol, path);
}

void markWordPositions(std::vector<std::vector<bool>>& foundPositions, const std::vector<std::vector<bool>>& path) {
    int n = foundPositions.size();
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (path[i][j]) {
                foundPositions[i][j] = true;
            }
        }
    }
}

void printSolution(const std::vector<std::string>& grid, const std::vector<std::vector<bool>>& foundPositions) {
    // Print column numbers
    std::cout << "   ";
    for (int col = 0; col < GRID_SIZE; ++col) {
        std::cout << std::setw(2) << col << " ";
    }
    std::cout << "\n";

    // Print the grid with found letters bolded (uppercase)
    for (int row = 0; row < GRID_SIZE; ++row) {
        std::cout << std::setw(2) << row << " ";
        for (int col = 0; col < GRID_SIZE; ++col) {
            char c = grid[row][col];
            if (foundPositions[row][col]) {
                // Bold the letter (using uppercase)
                std::cout << " " << static_cast<char>(std::toupper(c)) << " ";
            } else {
                // Non-bold letter (using lowercase)
                std::cout << " " << static_cast<char>(std::tolower(c)) << " ";
            }
        }
        std::cout << "\n";
    }
}
