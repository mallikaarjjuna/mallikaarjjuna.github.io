#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <string>
#include <locale>
#include <codecvt>
#include <vector>

struct Deletion {
    std::wstring word;
    int row, col;
    Deletion(const std::wstring& w, int r, int c) : word(w), row(r), col(c) {}
};

bool isDelimiter(wchar_t c) {
    return c == L' ' || c == L'.' || c == L',' || c == L'\n' || c == L'\r' || c == L'\t';
}

int main() {
    const std::string inputFile = "input.txt";
    const std::string outputFile = "output.txt";
    const std::string logFile = "deleted_words.log";

    // Setup UTF-8 locale for wfstream
    std::locale::global(std::locale(""));
    std::wifstream in(inputFile);
    in.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));

    std::wofstream out(outputFile);
    out.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));

    std::wofstream log(logFile);
    log.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));

    if (!in) {
        std::wcerr << L"Cannot open " << inputFile.c_str() << std::endl;
        return 1;
    }

    std::unordered_set<std::wstring> seenWords;
    std::vector<Deletion> deletions;

    std::wstring line;
    int row = 1;

    while (std::getline(in, line)) {
        std::wstring currentWord;
        std::wstring resultLine;
        int col = 1;

        for (size_t i = 0; i <= line.size(); ++i) {
            wchar_t c = (i < line.size())? line[i] : L'\n';

            if (isDelimiter(c)) {
                if (!currentWord.empty()) {
                    if (seenWords.count(currentWord)) {
                        deletions.emplace_back(currentWord, row, col - (int)currentWord.size());
                    } else {
                        seenWords.insert(currentWord);
                        resultLine += currentWord;
                    }
                    currentWord.clear();
                }
                if (i < line.size()) resultLine += c;
            } else {
                currentWord += c;
            }
            col++;
        }

        out << resultLine;
        if (!in.eof()) out << L'\n';
        row++;
    }

    log << L"Deleted duplicate words [word : row,col]\n";
    log << L"========================================\n";
    for (const auto& d : deletions) {
		log << d.word << L" : " << d.row << L"," << d.col << L"\n";
    }

    std::wcout << L"Done! Removed " << deletions.size() << L" duplicates.\n";
    std::wcout << L"Output: " << outputFile.c_str() << L"\n";
    std::wcout << L"Log: " << logFile.c_str() << L"\n";

    return 0;
}