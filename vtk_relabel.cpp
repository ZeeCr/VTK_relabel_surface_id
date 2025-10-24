// Relabel surface labels for meshes stored in ASCII VTK format
// Tested on VTK version 4.2 - should work for other versions
//	provided surface labels are stored under CellEntityIds subfield

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <cstdlib>

/**
 * @brief Reads VTK input file and, if found, converts CellEntityIds to new vals then saves file
*/

void getLineWrite(std::ifstream& read, std::ofstream& write, std::string& line) {
	std::getline(read, line);
	write << line << std::endl;
}

auto setLabelMap(std::vector<int>& old_labels, std::vector<int>& new_labels, std::map<int, int>& label_map) -> int {
	size_t o_size = old_labels.size();
	size_t n_size = new_labels.size();

	if (o_size != n_size) {
		std::cerr << "Label array sizes do not match" << std::endl;
		return 1;
	}
	for (size_t i = 0; i != o_size; ++i) {
		label_map[old_labels[i]] = new_labels[i];
	}

	return 0;
}

// Unused now, replaced with label_map
void arraySearch(int& bdry_label, std::vector<int>& old_labels, std::vector<int>& new_labels) {

	for (int i = 0; i < old_labels.size(); i = i + 1) {
		if (bdry_label == old_labels[i]) {
			bdry_label = new_labels[i];
			return;
		}
	}

}

int main()
{
	// User input
	std::string inputFileName = "NA223572.vtk";
	std::vector<int> old_labels { 1, 2, 5 };
	std::vector<int> new_labels { 5, 2, 1 }; // Permutation of labels

	std::string outputFileName = "relabelled_" + inputFileName;

	// Code
	std::string linea = "";
	std::istringstream iss;
	int token_int; // current token
	std::vector<int> tokens_int; // Store tokens - the tokens are going to be the individual numbers between the separator
	std::map<int, int> label_map; // Map from old to new labels

	// fstreams
	std::ifstream reada(inputFileName);
	std::ofstream writing(outputFileName);

	// Output error if file not read
	if (!reada) {
		std::cerr << "Error: Failed to read input file " << inputFileName << std::endl;
		return EXIT_FAILURE;
	}
	// ^.. not openable
	if (!writing) {
		std::cerr << "Error: Failed to open output file: " << outputFileName << std::endl;
		reada.close();
		return EXIT_FAILURE;
	}

	// Setup map
	if (setLabelMap(old_labels, new_labels, label_map) > 0) {
		writing.close();
		return EXIT_FAILURE;
	}

	// Find header for surface labels
	// If header isn't found, just copy until EOF
	while (linea.substr(0, 13) != "CellEntityIds") {
		getLineWrite(reada, writing, linea);
	}

	// Read until empty line or EOF
	while (std::getline(reada, linea)) {

		iss.clear();
		tokens_int.clear();
		iss.str(linea);

		while (iss >> token_int) {

			auto it = label_map.find(token_int);
			if (it != label_map.end()) {
				tokens_int.push_back(it->second);
			}
			else {
				tokens_int.push_back(token_int);
			}

		}

		// Must be white space or EOF so save line and exit getline loop
		if (tokens_int.empty()) {
			writing << linea << std::endl;
			break;
		}

		// May need changing on tokens_int.end() to remove trailing white space
		//	depending on particular file format
		for (auto label : tokens_int) {
			writing << label << " ";
		}
		writing << std::endl;

	}

	// Copy remaining part of file if not EOF
	while (std::getline(reada, linea)) {
		writing << linea << std::endl;
	}

	reada.close();
	writing.close();

	return 0;
}