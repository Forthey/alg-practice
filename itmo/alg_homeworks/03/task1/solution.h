#pragma once
#include <cstdint>
#include <vector>

using SerialNumber = int;

void sortShiftedElements(std::vector<SerialNumber>& arr, std::int64_t shift);

void insertionSort(std::vector<SerialNumber>& arr);

void sortShiftedElementsFast(std::vector<SerialNumber>& arr, std::int64_t shift);
