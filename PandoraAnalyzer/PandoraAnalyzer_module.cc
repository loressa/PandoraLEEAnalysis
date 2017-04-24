////////////////////////////////////////////////////////////////////////
// Class:       PandoraAnalyzer
// Module Type: analyzer
// File:        PandoraAnalyzer_module.cc
//
// Generated at Thu Jun 23 00:24:52 2016 by Lorena Escudero Sanchez using artmod
// from cetpkgsupport v1_10_02.
////////////////////////////////////////////////////////////////////////

// TODO
// - Put fidvol, electron_energy_threshold and proton_energy_threshold as fcl parameters
// - Use Geometry service for TPC size

#include <fstream>

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"


//uncomment the lines below as you use these objects

#include "lardataobj/RecoBase/PFParticle.h"
#include "lardataobj/RecoBase/Vertex.h"
#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/Shower.h"
#include "lardataobj/RecoBase/Cluster.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/MCBase/MCShower.h"
#include "nusimdata/SimulationBase/MCTruth.h"
#include "nusimdata/SimulationBase/MCParticle.h"
#include "canvas/Persistency/Common/FindOneP.h"
#include "canvas/Persistency/Common/FindManyP.h"
#include "canvas/Utilities/InputTag.h"
#include "larcore/Geometry/Geometry.h"

#include "larpandora/LArPandoraInterface/LArPandoraHelper.h"

#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"
#include "THStack.h"

#include "TEfficiency.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "PandoraAnalysis/PandoraAnalysis.hh"

using namespace lar_pandora;


namespace test {
  class PandoraAnalyzer;
}


double distance(double a[3], double b[3]) {
  double d = 0;

  for (int i = 0; i < 3; i++) {
    d += pow((a[i]-b[i]),2);
  }

  return sqrt(d);
}

class test::PandoraAnalyzer : public art::EDAnalyzer {
public:
  explicit PandoraAnalyzer(fhicl::ParameterSet const & pset);
  // The destructor generated by the compiler is fine for classes
  // without bare pointers or other resource use.
  virtual ~PandoraAnalyzer();

  // Plugins should not be copied or assigned.
  PandoraAnalyzer(PandoraAnalyzer const &) = delete;
  PandoraAnalyzer(PandoraAnalyzer &&) = delete;
  PandoraAnalyzer & operator = (PandoraAnalyzer const &) = delete;
  PandoraAnalyzer & operator = (PandoraAnalyzer &&) = delete;

  // Required functions.
  void analyze(art::Event const & e) override;
  void reconfigure(fhicl::ParameterSet const &pset) override;

private:

  test::PandoraAnalysis fMyAnalysisObj;
  TFile * myTFile;
  TTree * myTTree;
  TEfficiency * e_energy;
  TH1F * h_nu_e;
  TH1F * h_nu_mu;
  TH1F * h_dirt;
  TH1F * h_cosmic;
  TH1F * h_nc;
  THStack * h_e_stacked;


  bool m_printDebug;
  double m_fidvolXstart;
  double m_fidvolXend;

  double m_fidvolYstart;
  double m_fidvolYend;

  double m_fidvolZstart;
  double m_fidvolZend;

  double m_trackLength;

  const int k_cosmic = 1;
  const int k_nu_e = 2;
  const int k_nu_mu = 3;
  const int k_nc = 4;
  const int k_dirt = 5;

  bool is_fiducial(double x[3]) const;
  double distance(double a[3], double b[3]);
  bool is_dirt(double x[3]) const;
  void measure_energy(size_t ipf, const art::Event & evt, double & energy);
  size_t choose_candidate(std::vector<size_t> & candidates, const art::Event & evt);
  void get_daughter_tracks(size_t ipf, const art::Event & evt, std::vector<recob::Track> &tracks);

};


test::PandoraAnalyzer::PandoraAnalyzer(fhicl::ParameterSet const & pset)
  :
  EDAnalyzer(pset)  // ,
 // More initializers here.
{

  //create output tree
  art::ServiceHandle<art::TFileService> tfs;
  myTFile = new TFile("PandoraAnalyzerOutput.root", "RECREATE");
  myTTree = tfs->make<TTree>("pandoratree","PandoraAnalysis Tree");

  e_energy = tfs->make<TEfficiency>("e_energy",";#nu_{e} energy [GeV];N. Entries / 0.1 GeV",30,0,3);
  h_cosmic = tfs->make<TH1F>("h_cosmic",";#nu_{e} energy [GeV];N. Entries / 0.1 GeV",30,0,3);
  h_nc = tfs->make<TH1F>("h_nc",";#nu_{e} energy [GeV];N. Entries / 0.1 GeV",30,0,3);
  h_nu_e = tfs->make<TH1F>("h_nu_e",";#nu_{e} energy [GeV];N. Entries / 0.1 GeV",30,0,3);
  h_nu_mu = tfs->make<TH1F>("h_nu_mu",";#nu_{e} energy [GeV];N. Entries / 0.1 GeV",30,0,3);
  h_dirt = tfs->make<TH1F>("h_dirt",";#nu_{e} energy [GeV];N. Entries / 0.1 GeV",30,0,3);

  h_e_stacked = tfs->make<THStack>("h_e_stacked",";#nu_{e} energy [GeV];N. Entries / 0.1 GeV");


  //add branches


  this->reconfigure(pset);

}

test::PandoraAnalyzer::~PandoraAnalyzer()
{

  //store output tree
  myTFile->cd();
  myTTree->Write("pandoratree");

  h_cosmic->SetLineColor(1);
  h_cosmic->SetLineWidth(2);
  h_cosmic->SetFillColor(kRed-3);
  h_cosmic->Write();

  h_nc->SetLineColor(1);
  h_nc->SetLineWidth(2);
  h_nc->SetFillColor(kBlue-9);
  h_nc->Write();

  h_nu_e->SetLineColor(1);
  h_nu_e->SetLineWidth(2);
  h_nu_e->SetFillColor(kGreen-2);
  h_nu_e->Write();

  h_nu_mu->SetLineColor(1);
  h_nu_mu->SetLineWidth(2);
  h_nu_mu->SetFillColor(kBlue-5);
  h_nu_mu->Write();

  h_dirt->SetLineColor(1);
  h_dirt->SetLineWidth(2);
  h_dirt->SetFillColor(kGray);
  h_dirt->Write();

  h_e_stacked->Add(h_cosmic);
  h_e_stacked->Add(h_nc);
  h_e_stacked->Add(h_nu_e);
  h_e_stacked->Add(h_nu_mu);
  h_e_stacked->Add(h_dirt);
  h_e_stacked->Write();

  myTFile->Close();


  std::cout << "End!" << std::endl;
}

double test::PandoraAnalyzer::distance(double a[3], double b[3]) {
  double d = 0;

  for (int i = 0; i < 3; i++) {
    d += pow((a[i]-b[i]),2);
  }

  return sqrt(d);
}

void test::PandoraAnalyzer::get_daughter_tracks(size_t ipf, const art::Event & evt, std::vector<recob::Track> &tracks) {
  art::InputTag pandoraNu_tag { "pandoraNu" };

  auto const& pfparticle_handle = evt.getValidHandle< std::vector< recob::PFParticle > >( pandoraNu_tag );
  auto const& pfparticles(*pfparticle_handle);

  art::FindOneP< recob::Track > track_per_pfpart(pfparticle_handle, evt, pandoraNu_tag);

  for (auto const& pfdaughter: pfparticles[ipf].Daughters()) {
    if (pfparticles[pfdaughter].PdgCode() == 13) {
      auto const& track_obj = track_per_pfpart.at(pfdaughter);
      if (track_obj->Length() < m_trackLength) {
        tracks.push_back(track_obj);
      }
    }
  }
}

void test::PandoraAnalyzer::measure_energy(size_t ipf, const art::Event & evt, double & energy) {

  art::InputTag pandoraNu_tag { "pandoraNu" };

  auto const& pfparticle_handle = evt.getValidHandle< std::vector< recob::PFParticle > >( pandoraNu_tag );
  auto const& pfparticles(*pfparticle_handle);

  art::FindManyP<recob::Shower > showers_per_pfparticle ( pfparticle_handle, evt, pandoraNu_tag );
  std::vector<art::Ptr<recob::Shower>> showers = showers_per_pfparticle.at(ipf);

  for(size_t ish = 0; ish < showers.size(); ish++) {
    energy += showers[ish]->Energy()[showers[ish]->best_plane()];
  }

  for (auto const& pfdaughter: pfparticles[ipf].Daughters()) {
    measure_energy(pfdaughter, evt, energy);
  }
}

size_t test::PandoraAnalyzer::choose_candidate(std::vector<size_t> & candidates, const art::Event & evt) {

  art::InputTag pandoraNu_tag { "pandoraNu" };

  auto const& pfparticle_handle = evt.getValidHandle< std::vector< recob::PFParticle > >( pandoraNu_tag );
  auto const& pfparticles(*pfparticle_handle);

  art::FindOneP< recob::Shower > shower_per_pfpart(pfparticle_handle, evt, pandoraNu_tag);
  art::FindOneP< recob::Track > track_per_pfpart(pfparticle_handle, evt, pandoraNu_tag);

  size_t chosen_candidate = 0;
  double most_z = -1;

  for (auto const& ic: candidates) {

    double longest_track = 0;
    double longest_track_dir = -1;

    for (auto const& pfdaughter: pfparticles[ic].Daughters()) {

      if (pfparticles[pfdaughter].PdgCode() == 13) {
        auto const& track_obj = track_per_pfpart.at(pfdaughter);
        if (track_obj->Length() > longest_track) {
          longest_track = track_obj->Length();
          longest_track_dir = track_obj->StartDirection().Z();
        }
      }
    }

    if (longest_track_dir > most_z) {
      chosen_candidate = ic;
      most_z = longest_track_dir;
    }

  }

  return chosen_candidate;

}

bool test::PandoraAnalyzer::is_fiducial(double x[3]) const
{
  art::ServiceHandle<geo::Geometry> geo;
  double bnd[6] = {0.,2.*geo->DetHalfWidth(),-geo->DetHalfHeight(),geo->DetHalfHeight(),0.,geo->DetLength()};

  bool is_x = x[0] > (bnd[0]+m_fidvolXstart) && x[0] < (bnd[1]-m_fidvolXend);
  bool is_y = x[1] > (bnd[2]+m_fidvolYstart) && x[1] < (bnd[3]-m_fidvolYend);
  bool is_z = x[2] > (bnd[4]+m_fidvolZstart) && x[2] < (bnd[5]-m_fidvolZend);
  return is_x && is_y && is_z;
}


bool test::PandoraAnalyzer::is_dirt(double x[3]) const
{
  art::ServiceHandle<geo::Geometry> geo;
  double bnd[6] = {0.,2.*geo->DetHalfWidth(),-geo->DetHalfHeight(),geo->DetHalfHeight(),0.,geo->DetLength()};

  bool is_x = x[0] > bnd[0] && x[0] < bnd[1];
  bool is_y = x[1] > bnd[2] && x[1] < bnd[3];
  bool is_z = x[2] > bnd[4] && x[2] < bnd[5];
  return !(is_x && is_y && is_z);
}

void test::PandoraAnalyzer::analyze(art::Event const & evt)
{

  //do the analysis
  art::InputTag pandoraNu_tag { "pandoraNu" };
  art::InputTag generator_tag { "generator" };

  int bkg_category = 0;

  auto const& generator_handle = evt.getValidHandle< std::vector< simb::MCTruth > >( generator_tag );
  auto const& generator(*generator_handle);
  int ccnc = -1;
  double true_neutrino_vertex[3];
  std::vector<simb::MCParticle> nu_mcparticles;

  if (generator.size() > 0) {
    ccnc = generator[0].GetNeutrino().CCNC();
    if (ccnc == 1) {
      bkg_category = k_nc;
    }

    true_neutrino_vertex[0] = generator[0].GetNeutrino().Nu().Vx();
    true_neutrino_vertex[1] = generator[0].GetNeutrino().Nu().Vy();
    true_neutrino_vertex[2] = generator[0].GetNeutrino().Nu().Vz();

    if (is_dirt(true_neutrino_vertex)) {
      bkg_category = k_dirt;
    }

    for (int i = 0; i < generator[0].NParticles(); i++) {
      if (generator[0].Origin() == 1) {
        nu_mcparticles.push_back(generator[0].GetParticle(i));
      }
    }
  } else {
    bkg_category = k_cosmic;
  }

  int protons = 0;
  int electrons = 0;
  int muons = 0;

  for (auto& mcparticle: nu_mcparticles) {
    if (mcparticle.Process() == "primary" and mcparticle.T() != 0 and mcparticle.StatusCode() == 1) {

      switch(mcparticle.PdgCode())
      {
        case (abs(2212)):
        protons++;
        break;

        case (abs(11)):
        electrons++;
        break;

        case (abs(13)):
        muons++;
        break;
      }

    }
  }
  double reco_energy = 0;

  try {
    auto const& pfparticle_handle = evt.getValidHandle< std::vector< recob::PFParticle > >( pandoraNu_tag );
    auto const& pfparticles(*pfparticle_handle);

    art::FindOneP< recob::Shower > shower_per_pfpart(pfparticle_handle, evt, pandoraNu_tag);
    art::FindOneP< recob::Track > track_per_pfpart(pfparticle_handle, evt, pandoraNu_tag);


    std::vector<size_t> nu_candidates;

    for (size_t ipf = 0; ipf < pfparticles.size(); ipf++) {

      bool is_neutrino = (abs(pfparticles[ipf].PdgCode()) == 12 || abs(pfparticles[ipf].PdgCode()) == 14) && pfparticles[ipf].IsPrimary();
      if (!is_neutrino) continue;

      int showers = 0;
      int tracks = 0;

      std::vector<recob::Track> nu_tracks;
      get_daughter_tracks(ipf, evt, nu_tracks);

      for (auto const& pfdaughter: pfparticles[ipf].Daughters()) {

        if (pfparticles[pfdaughter].PdgCode() == 11) {
          auto const& shower_obj = shower_per_pfpart.at(pfdaughter);
          bool contained_shower = false;
          double start_point[3];
          double end_point[3];

          double shower_length = shower_obj->Length();
          for (int ix = 0; ix < 3; ix++) {
            start_point[ix] = shower_obj->ShowerStart()[ix];
            end_point[ix] = shower_obj->ShowerStart()[ix]+shower_length*shower_obj->Direction()[ix];
          }

          contained_shower = is_fiducial(start_point) && is_fiducial(end_point);

          if (contained_shower) showers++;
        }

        if (pfparticles[pfdaughter].PdgCode() == 13) {
          auto const& track_obj = track_per_pfpart.at(pfdaughter);
          if (track_obj->Length() < m_trackLength) {
            tracks++;
          }
        }

      } // end for pfparticle daughters

      if (nu_tracks.size() > 0 && showers >= 1) {
        nu_candidates.push_back(ipf);
      }

    } // end for pfparticles

    if (nu_candidates.size() == 0) return;

    size_t ipf_candidate = choose_candidate(nu_candidates, evt);

    measure_energy(ipf_candidate, evt, reco_energy);

    if (generator.size() > 0) {
      art::FindOneP< recob::Vertex > vertex_per_pfpart(pfparticle_handle, evt, pandoraNu_tag);
      auto const& vertex_obj = vertex_per_pfpart.at(ipf_candidate);

      double reco_neutrino_vertex[3];
      vertex_obj->XYZ(reco_neutrino_vertex);
      if (distance(reco_neutrino_vertex,true_neutrino_vertex) > 10) {
        bkg_category = k_cosmic;
      }
    }

    std::cout << "Chosen neutrino " << ipf_candidate << std::endl;

  } catch (...) {
    std::cout << "NO RECO DATA PRODUCTS" << std::endl;
  }

  if (bkg_category != k_cosmic && bkg_category != k_dirt && bkg_category != k_nc) {
    if (protons != 0 && electrons != 0) {
      bkg_category = k_nu_e;
    } else if (protons != 0 && muons != 0) {
      bkg_category = k_nu_mu;
    }
  }

  if (bkg_category == k_cosmic) {
    h_cosmic->Fill(reco_energy);
  } else if (bkg_category == k_nu_mu) {
    h_nu_mu->Fill(reco_energy);
  } else if (bkg_category == k_nu_e) {
    h_nu_e->Fill(reco_energy);
  } else if (bkg_category == k_dirt) {
    h_dirt->Fill(reco_energy);
  } else if (bkg_category == k_nc) {
    h_nc->Fill(reco_energy);
  }


  std::cout << "Category " << bkg_category << std::endl;


} // end analyze function

//------------------------------------------------------------------------------------------------------------------------------------


void test::PandoraAnalyzer::reconfigure(fhicl::ParameterSet const & pset)
{

  //TODO: add an external fcl file to change configuration
  //add what you want to read, and default values of your labels etc. example:
  //  m_particleLabel = pset.get<std::string>("PFParticleModule","pandoraNu");

  m_printDebug = pset.get<bool>("PrintDebug",false);
  m_trackLength = pset.get<int>("trackLength",100);

  m_fidvolXstart = pset.get<double>("fidvolXstart",10);
  m_fidvolXend = pset.get<double>("fidvolXstart",10);

  m_fidvolYstart = pset.get<double>("fidvolYstart",20);
  m_fidvolYend = pset.get<double>("fidvolYend",20);

  m_fidvolZstart = pset.get<double>("fidvolZstart",10);
  m_fidvolZend = pset.get<double>("fidvolZend",50);
}

//---------------------------------------------------------------------------------------------------------------------------
//add other functions here

DEFINE_ART_MODULE(test::PandoraAnalyzer)
