#include <iostream>
#include <fstream>
#include <filesystem>
#include <unordered_set>
#include <define.h>
#include <cstdlib>
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

int main(int argc, char* argv[]) {
    std::string rseq_dir;
    std::string rrig_path;
    std::string rmdl_path;
    uint32_t in_season = 27;

    static std::unordered_set<std::string> supported_in_seasons = { "3", "4", "25", "26", "27"};

    for (int i = 1; i < argc;) {
        if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            std::string ss = argv[i + 1];
            in_season = std::atoi(ss.c_str());

            for (int j = i; j + 2 < argc; ++j) argv[j] = argv[j + 2];
            argc -= 2;

            continue;
        }

        if (strcmp(argv[i], "-rrig") == 0 && i + 1 < argc) {
            std::string str = argv[i + 1];
            rrig_path = str;

            for (int j = i; j + 2 < argc; ++j) argv[j] = argv[j + 2];
            argc -= 2;

            if (!std::filesystem::exists(rrig_path)) std::cerr << "skipping .rrig file does not exists." << std::endl;
            continue;
        }

        if (strcmp(argv[i], "-rmdl") == 0 && i + 1 < argc) {
            std::string str = argv[i + 1];
            rmdl_path = str;

            for (int j = i; j + 2 < argc; ++j) argv[j] = argv[j + 2];
            argc -= 2;

            if (!std::filesystem::exists(rmdl_path)) std::cerr << "skipping .rmdl file does not exists." << std::endl;
            continue;
        }

        if (strcmp(argv[i], "-rseq") == 0 && i + 1 < argc) {
            std::string str = argv[i + 1];
            rseq_dir = str;

            for (int j = i; j + 2 < argc; ++j) argv[j] = argv[j + 2];
            argc -= 2;

            if (!std::filesystem::exists(rseq_dir)) std::cerr << "skipping rseq directory does not exists." << std::endl;
            continue;
        }
        i++;
    }

    if ((rmdl_path.empty() && rrig_path.empty() && rseq_dir.empty()) && argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <parent directory> or [-rseq rseq_directory] [-rrig rrig_path.rrig] [-rmdl rmdl_path.rmdl] [-v version[.subversion]]\n";
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
    printf("Parsing Assets ss%d\n", in_season);
    PrintSets(supported_in_seasons, "Supported seasons");

    try {
        switch (in_season){
        case 3:
            if (!rrig_path.empty()) readRrig_v10(rrig_path, modelOut);
            if (!rmdl_path.empty()) readRmdl_v10(rmdl_path, modelOut);
            if (!rseq_paths.empty()) readRseq_v7(in_dir, rseq_paths, modelOut);
            break;
        case 4:
            if (!rrig_path.empty()) readRrig_v13(rrig_path, modelOut);
            if (!rmdl_path.empty()) readRmdl_v121(rmdl_path, modelOut);
            if (!rseq_paths.empty()) readRseq_v7(in_dir, rseq_paths, modelOut);
            break;
        case 25:
            if (!rrig_path.empty()) readRrig_v16(rrig_path, modelOut);
            if (!rmdl_path.empty()) readRmdl_v16(rmdl_path, modelOut);
            if (!rseq_paths.empty()) readRseq_v12(in_dir, rseq_paths, modelOut);
            break;
        case 26:
        case 27:
            if (!rrig_path.empty()) readRrig_v16(rrig_path, modelOut);
            if (!rmdl_path.empty()) readRmdl_v16(rmdl_path, modelOut);
            if (!rseq_paths.empty()) readRseq_v121(in_dir, rseq_paths, modelOut);
            break;
        default:
            throw std::runtime_error("Unsupported version");
        }

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