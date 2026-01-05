#include <pch.h>
#include <define.h>
#include <mdl/studio.h>
#include <mdl/qc.h>

void getRseqPaths(std::filesystem::directory_entry dir, std::vector<std::string>& rseq) {
	if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) {
		std::cerr << "Error rseq directory does not exist.\n";
		return;
	}

	for (const auto& entry : std::filesystem::directory_iterator(dir)) {
		if (entry.is_regular_file()) {
			std::string ext = entry.path().extension().string();
			if (ext == ".rseq") {
				rseq.push_back(entry.path().string());
			}
		}
		else if (entry.is_directory()) {
			getRseqPaths(entry, rseq);
		}
	}
}

static void PrintSets(std::unordered_set<std::string> ss, const char* text = "") {
	printf("%s [ ", text);
	for (auto& v : ss) { printf("%s ", v.c_str()); }
	printf("]\n");
}

#define ARG_BOOL(opt, var) \
    if (arg == opt) { var = true; continue; }

#define ARG_INT(opt, var, err) \
    if (arg == opt) { \
        if (++i >= argc) { \
            printf("%s\n", err); \
            return 1; \
        } \
        var = atoi(argv[i]); \
        continue; \
    }

#define ARG_VAL(opt, var, err) \
    if (arg == opt) { \
        if (++i >= argc) { \
            printf("%s\n", err); \
            return 1; \
        } \
        var = argv[i]; \
        continue; \
    }

using ParseRRIGFn = void(*)(std::string, qc::QCModel&);
using ParseRSEQFn = void(*)(const std::string, const std::vector<std::string>&, qc::QCModel&);

struct ParserSet {
	ParseRRIGFn rrig;
	ParseRSEQFn rseq;
};

static const std::unordered_map<std::string, ParserSet> Parsers = {
	{"3",  {ParseRRIG_v10,  ParseRSEQ_v7}},
	{"9",  {ParseRRIG_v121, ParseRSEQ_v10}},
	{"10", {ParseRRIG_v121, ParseRSEQ_v10}},
	{"11", {ParseRRIG_v121, ParseRSEQ_v10}},
	{"12", {ParseRRIG_v121, ParseRSEQ_v10}},
	{"13", {ParseRRIG_v13,  ParseRSEQ_v10}},
	{"14", {ParseRRIG_v14,  ParseRSEQ_v10}},
	{"15", {ParseRRIG_v16,  ParseRSEQ_v11}},
	{"16", {ParseRRIG_v16,  ParseRSEQ_v11}},
	{"17", {ParseRRIG_v16,  ParseRSEQ_v11}},
	{"18", {ParseRRIG_v16,  ParseRSEQ_v11}},
	{"19", {ParseRRIG_v16,  ParseRSEQ_v11}},
	{"20", {ParseRRIG_v17,  ParseRSEQ_v11}},
	{"21", {ParseRRIG_v17,  ParseRSEQ_v11}},
	{"22", {ParseRRIG_v17,  ParseRSEQ_v11}},
	{"23", {ParseRRIG_v17,  ParseRSEQ_v11}},
	{"24", {ParseRRIG_v18,  ParseRSEQ_v12}},
	{"25", {ParseRRIG_v18,  ParseRSEQ_v12}},
	{"26", {ParseRRIG_v19,  ParseRSEQ_v12}},
	{"27", {ParseRRIG_v191, ParseRSEQ_v121}},
};

int main(int argc, char* argv[]) {
	std::string rseq_dir;
	std::string rrig_path;
	std::string rmdl_path;
	std::string in_season = "27";

	std::string usage = "Usage: \n" \
		"R5-AnimInspector.exe <parent directory> or [-rseq <rseq_directory>] [-rrig <rrig_path.rrig>] [-rmdl <rmdl_path.rmdl>] [-i <in assets season>]\n";

	for (int i = 2; i < argc; ++i) {
		std::string arg = argv[i];
		ARG_VAL("-i", in_season, "[!] Error: -i requires a assets season argument.\n");
		ARG_VAL("-rseq", rseq_dir, "[!] Error: -rseq requires a path argument.\n");
		ARG_VAL("-rrig", rrig_path, "[!] Error: -rrig requires a path argument.\n");
		ARG_VAL("-rmdl", rmdl_path, "[!] Error: -rmdl requires a path argument.\n");

		printf("Unknown option: %s \n%s", arg.c_str(), usage.c_str());
		return 1;
	}

	if ((rmdl_path.empty() && rrig_path.empty() && rseq_dir.empty()) && argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <parent directory> or [-rseq <rseq_directory>] [-rrig <rrig_path.rrig>] [-rmdl <rmdl_path.rmdl>] [-i <in assets season>]\n";
		system("pause");
		return 1;
	}

	std::string in_dir;
	bool useparentdir = rrig_path.empty() && rmdl_path.empty() && rseq_dir.empty();
	if (useparentdir) {
		in_dir = argv[1];
	}
	else {
		if (!rrig_path.empty()) in_dir = rrig_path;
		if (!rmdl_path.empty()) in_dir = rmdl_path;
		if (!rseq_dir.empty()) in_dir = rseq_dir;
	}

	if (std::filesystem::exists(in_dir)) {
		if (!std::filesystem::is_directory(in_dir)) {
			in_dir = std::filesystem::path(in_dir).parent_path().string();
		}
	}
	else {
		std::cerr << "Error: input file/directory does not exist.\n";
		return 1;
	}

	std::vector<std::string> rseq_paths;
	if (useparentdir) {
		for (const auto& entry : std::filesystem::directory_iterator(in_dir)) {
			if (entry.is_regular_file()) {
				std::string ext = entry.path().extension().string();
				if (rrig_path.empty() && ext == ".rrig") {
					rrig_path = entry.path().string();
				}
				if (rmdl_path.empty() && ext == ".rmdl") {
					rmdl_path = entry.path().string();
				}
			}
			else if (entry.is_directory() && rseq_dir.empty()) {
				getRseqPaths(entry, rseq_paths);
			}
		}
	}

	if (!rseq_dir.empty()) {
		getRseqPaths(std::filesystem::directory_entry(rseq_dir), rseq_paths);
		if (rseq_paths.empty()) {
			std::cerr << "No .rseq files found in directories." << std::endl;
			return 1;
		}
	}

	std::filesystem::path parent_dir = std::filesystem::path(in_dir);

	std::string out_path = parent_dir.string() + "\\model.qc";
	qc::QCModel modelOut(out_path);

	try {
		auto parser = Parsers.find(in_season);
		if (parser == Parsers.end()) {
			printf("[!] Error: Unsupported assets version.\n");
			return 1;
		}

		if (rmdl_path.length()) parser->second.rrig(rmdl_path, modelOut);
		if (rrig_path.length()) parser->second.rrig(rrig_path, modelOut);
		if (rseq_paths.size()) parser->second.rseq(in_dir, rseq_paths, modelOut);

		modelOut.SortSequences();
		modelOut.Write();

		printf("[Succeeded] %s", out_path.c_str());
		return 0;
	}
	catch (const std::exception& e) {
		std::cerr << "[!] Error: " << e.what() << std::endl;
		return 1;
	}
}