#ifndef TEST_PANDORAANALYSIS_CXX
#define TEST_PANDORAANALYSIS_CXX

#include "PandoraAnalysis.hh"
#include <iostream>

test::PandoraAnalysis::PandoraAnalysis()
  : fAlgName("PandoraAnalysis")
{}

void test::PandoraAnalysis::SetupOutputTree(TTree* tfs_tree){
  fTree = tfs_tree;

  std::string title = fAlgName + " Tree";
  fTree->SetObject(fTree->GetName(),title.c_str());
}

void test::PandoraAnalysis::RunAnalysis(){
  PrintInfo();
}

void test::PandoraAnalysis::PrintInfo(){
  std::cout << "\n================================== PandoraAnalysis ==========================" << std::endl;
  std::cout << "This is a ub_PandoraAnalysis class called " << fAlgName << std::endl;
  std::cout << "\tThere is an output tree called "
            << fTree->GetName() << " (" << fTree->GetTitle() << ")" << std::endl;
  std::cout << "==========================================================================\n" << std::endl;
}

#endif
