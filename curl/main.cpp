#define CURL_STATICLIB
#include <iostream>
#include <iomanip>
#include <curl/curl.h>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* outBuffer) {
    size_t totalSize = size * nmemb;
    outBuffer->append((char*)contents, totalSize);
    return totalSize;
}

int main() {
    CURL* curl;
    std::string readBuffer, lottoNum, currentNum, tempStr;
    const char* website;
    char tempChar;
    std::ifstream ifile("dataset.txt");
    int indexPlus = 18;
    int index, occurence, tempNum = 0;
    int lottoArray[9999] = {};
    std::vector<int> currentData;
    std::vector<std::vector<int>> data;
    
    for (int i = 1; i < 10000; i++) {
        CURLcode res;
        std::string strOccurence;
        bool finding_occurence = true;

        currentNum = std::to_string(i);

        if (i > 999) {
            lottoNum = currentNum;
        }
        else if (i > 99) {
            lottoNum = "0" + currentNum;
        }
        else if (i > 9) {
            lottoNum = "00" + currentNum;
        }
        else {
            lottoNum = "000" + currentNum;
        }

        tempStr = "https://www.4dpredict.org/gdlotto/" + lottoNum + "/";
        website = tempStr.c_str();

        curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, website);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            res = curl_easy_perform(curl);

            if (res != CURLE_OK) {
                std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << '\n';
                i--;
            }
            else {
                index = readBuffer.find("There are <strong>");

                while (finding_occurence) {
                    tempChar = readBuffer.at(index + indexPlus);

                    if (isdigit(tempChar)) {
                        strOccurence += tempChar;
                        indexPlus += 1;
                    }
                    else {
                        finding_occurence = false;
                        indexPlus = 18;
                    }
                }

                if (strOccurence != "") {
                    occurence = stoi(strOccurence);
                }
                else {
                    occurence = 0;
                }


                currentData.emplace_back(i);
                currentData.emplace_back(occurence);
                data.emplace_back(currentData);

                std::cout << "LottoNum: " << i << '\n';
                std::cout << "Occurence: " << occurence << '\n';

                curl_easy_reset(curl);
                readBuffer.clear();
                currentData.clear();
            }
        }
    }

    while (ifile) {
        ifile >> tempNum;
        currentData.emplace_back(tempNum);
        ifile >> tempNum;
        currentData.emplace_back(tempNum);
        data.emplace_back(currentData);
        currentData.clear();
    }
    ifile.close();

    std::sort(data.begin(), data.end(),
        [](auto const& a, auto const& b) { return a[1] > b[1]; });

    std::ofstream ofile("dataset.txt");
    index = 0;
    for (auto const &row : data) {
        if (std::find(std::begin(lottoArray), std::end(lottoArray), row[0]) == std::end(lottoArray)) {
            if (row[0] != 0) {
                if (row[0] > 999) {
                    tempStr = std::to_string(row[0]);
                }
                else if (row[0] > 99) {
                    tempStr = "0" + std::to_string(row[0]);
                }
                else if (row[0] > 9) {
                    tempStr = "00" + std::to_string(row[0]);
                }
                else {
                    tempStr = "000" + std::to_string(row[0]);
                }
                ofile << std::setw(8) << std::left << tempStr << row[1] << '\n';

                lottoArray[index] = row[0];
                index += 1;
            }
        }
    }
    ofile.close();
    
    curl_global_cleanup();
    return 0;
}