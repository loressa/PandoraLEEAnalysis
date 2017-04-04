/**
 * \file PandoraAnalysis.h
 *
 * 
 * \brief Little sample program for establishing a user analysis space.
 *
 * @author loressa
 */

#ifndef TEST_PANDORAANALYSIS_H
#define TEST_PANDORAANALYSIS_H

#include <string>

#include "TTree.h"

namespace test{
  class PandoraAnalysis;
}

class test::PandoraAnalysis{
  
 public:
  
  /// Default constructor
  PandoraAnalysis();

  /// Default destructor
  virtual ~PandoraAnalysis(){};

  void RunAnalysis();
  void SetupOutputTree(TTree*);
  
 private:

  std::string fAlgName;
  TTree*      fTree;
  
  void PrintInfo();

  
};

#endif
