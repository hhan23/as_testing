#include <iostream>
#include <fstream>
#include <ostream>
#include <filesystem>//c++17

#include "globaldata.h"
#include "solver.h"
#include "input.h"
#include "enums.h"
#include "swmm5.h"

using gd = GlobalData;

bool hasTxtExtension(const char* filename) {
    int length = std::strlen(filename); // 获取字符串长度
    if (length < 4) {  // 如果长度小于4，直接返回false
        return false;
    }

    // 检查最后四个字符
    return filename[length - 1] == 't' &&
        filename[length - 2] == 'x' &&
        filename[length - 3] == 't' &&
        filename[length - 4] == '.';
}

int main(int argc,char *argv[])
{
    char* arg1;
    char* inputFile;
    if (argc == 1) {
        printf("\nNot Enough Arguments (See Help --help)\n\n");
    }
    else if (argc == 2) {
        arg1 = argv[1];
        //help
        if (strcmp(arg1, "--help") == 0 || strcmp(arg1, "-h") == 0) {
            //help
            printf("\n\nUrban Pluvial FLOOD\n\n");
            printf("COMMANDS:\n");
            printf("\t--help (-h)    UPFLOOD Help");
            printf("\t--version (-v)    Build Version\n");
            printf("\nRUNNING A SIMULATION:\n");

        }
        //version
        else if (strcmp(arg1, "--version") == 0 || strcmp(arg1, "-v") == 0) {
            printf("");
        }
        //read inputfile
        else if (hasTxtExtension(arg1)) {
            inputFile = argv[1];
            std::ifstream file(inputFile);
            if (file.is_open()) {
                std::cerr << "Error: Could not open file.\n";
                return 1;
            }
            
            ASCIIGridFile gf;
            try{
                input::input_readData(file);
                gf.load(gd::DemFilePath);
                gd::inital(gf);
                if (gd::building) {
                    if (gd::buildinginputMode == SHAPEFILEPOLYGON) {
                        ShapeFilePolygon buildingprofile;
                        buildingprofile.loadSHP(gd::BuildingFilePath);
                        gd::freshBuildings(buildingprofile);
                    }
                    else if(gd::buildinginputMode == ASCIIGRIDFILE){
                        ASCIIGridFile buildingprofile;
                        buildingprofile.load(gd::BuildingFilePath);
                        gd::freshBuildings(buildingprofile);
                    } 
                }
                if (gd::drain) {
                    if (gd::swmm) {
                        ShapeFilePoint drainprofile;
                        drainprofile.loadSHP(gd::DrainFilePath);
                        gd::initalinpfile();
                        gd::freshDrains(gd::f1,gd::f2,gd::f3);
                    }

                }
                Solver* solver = new Solver();
                solver->init();
                solver->run(std::filesystem::current_path().string());
                delete solver;
                GlobalData::deinit();


            }
            catch(const std::exception& e){
                std::cerr << "Error: " << e.what() << '\n';
                return 1;
            }
            
        }
        //unknown message
        else {
            printf("\nUnknown Argument (See Help --help)\n\n");
        }
    }
    return 0;
}




