//
// Created by sokol on 14.05.20.
//
#include "path_finder/OscarIntegration.h"
pathFinder::CellIdDiskWriter::CellIdDiskWriter(
    pathFinder::CellIdStore<std::vector> &cellIdStore) :_cellIdStore(cellIdStore){
}