#ifndef macroutils_h
#define macroutils_h


#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/ChainEvent.h"
#include "DataFormats/Common/interface/MergeableCounter.h"


//Load here all the dataformat that we will need
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"



#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "FWCore/PythonParameterSet/interface/MakePyBind11ParameterSets.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/RegexMatch.h"
#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"




#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/IPTools/interface/IPTools.h"

#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"

#include "DataFormats/Math/interface/deltaPhi.h"

#include "DataFormats/PatCandidates/interface/GenericParticle.h"

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"

#include "UserCode/bsmhiggs_fwk/interface/PatUtils.h"

//------------------ needed headers for slew rate corrections -------------------//
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"

#include <istream>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <vector>
#include "TVector3.h"
#include "TMath.h"
#include "TGraph.h"
#include <Math/VectorUtil.h>


typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LorentzVector;


namespace utils
{
  namespace cmssw
  {
    //retrieve last state before decay
    const reco::Candidate *getGeneratorFinalStateFor(const reco::Candidate *p, bool isSherpa);

    //check pdg id code for b-flavored hadrons
    bool isBhadron(int pdgId);

    //get impact parameter for a track
    template<class T>
    std::pair<bool,Measurement1D> getImpactParameter(const T &trkRef, reco::VertexRef &vtx, const edm::EventSetup &iSetup, bool is3d=true)
      {
	edm::ESHandle<TransientTrackBuilder> trackBuilder;
	iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder", trackBuilder);
	reco::TransientTrack tt = trackBuilder->build(trkRef.get());
	if(is3d) return IPTools::absoluteImpactParameter3D(tt, *vtx);
	else     return IPTools::absoluteTransverseImpactParameter(tt, *vtx);
      }

    //
    template<class T>
    float getArcCos(T &a,T &b)
      {
	TVector3 mom1(a.px(),a.py(),a.pz());
	TVector3 mom2(b.px(),b.py(),b.pz());
	float cosine = mom1.Dot(mom2)/(mom1.Mag()*mom2.Mag());
	float arcCosine = acos(cosine);
	return arcCosine;
      }

    template<class T>
    float getMT(T &visible, T &invisible, bool setSameMass=false)
    {
      float mt(-1);
      if(setSameMass){
	T sum=visible+invisible;
	mt= TMath::Power(TMath::Sqrt(TMath::Power(visible.pt(),2)+pow(visible.mass(),2))+TMath::Sqrt(TMath::Power(invisible.pt(),2)+pow(visible.mass(),2)),2);
	mt-=TMath::Power(sum.pt(),2);
	mt=TMath::Sqrt(mt);
      }else{
	double dphi=fabs((double)deltaPhi((double)invisible.phi(),(double)visible.phi()));
	mt=TMath::Sqrt(2*invisible.pt()*visible.pt()*(1-TMath::Cos(dphi)));
      }
      return mt;
    }

    template<class T1, class T2>
    float getMT(T1 &visible, T2 &invisible, bool setSameMass=false)
    {
      float mt(-1);
      if(setSameMass){
	LorentzVector sum= LorentzVector(visible)+LorentzVector(invisible);
	mt= TMath::Power(TMath::Sqrt(TMath::Power(visible.pt(),2)+pow(visible.mass(),2))+TMath::Sqrt(TMath::Power(invisible.pt(),2)+pow(visible.mass(),2)),2);
	mt-=TMath::Power(sum.pt(),2);
	mt=TMath::Sqrt(mt);
      }else{
	double dphi=fabs(deltaPhi((double)invisible.phi(),(double)visible.phi()));
	mt=TMath::Sqrt(2*invisible.pt()*visible.pt()*(1-TMath::Cos(dphi)));
      }
      return mt;
    }
 
    std::vector<float> smearJER(float pt, float eta, float genPt);
    std::vector<float> smearJES(float pt, float eta, JetCorrectionUncertainty *jecUnc);

    typedef std::vector<TGraph *> PuShifter_t;
    enum PuShifterTypes {PUDOWN,PUUP};
    utils::cmssw::PuShifter_t getPUshifters(std::vector< float > &Lumi_distr, float puUnc);
    Float_t getEffectiveArea(int id, float eta, Int_t yearBits, TString isoSum="");
//    double relIso(llvvLepton lep, double rho);

    // Single muon trigger efficiency 
    void getSingleMuTrigEff(const double&, const double&, double& );

    //cf. https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections#JetEnCorFWLite
    FactorizedJetCorrector *getJetCorrector(TString baseDir, bool isMC);
  


    // JES/JER Smearing
      std::vector<double> smearJER(double pt, double eta, double genPt);
      std::vector<float> smearJES(double pt, double eta, JetCorrectionUncertainty *jecUnc);
   
//    
//    //set new jet energy corrections
     void updateJEC(pat::JetCollection& jets, FactorizedJetCorrector *jesCor, JetCorrectionUncertainty *totalJESUnc, float rho, int nvtx,bool isMC);
   
		 void SlewRateCorrection(const fwlite::Event& ev, pat::Electron& ele);  
 
//    //apply MET variations
//    enum METvariations { NOMINAL, JERUP, JERDOWN, JESUP, JESDOWN, UMETUP, UMETDOWN, LESUP, LESDOWN };
//    std::vector<LorentzVector> getMETvariations(LorentzVector &rawMETP4, pat::JetCollection &jets, std::vector<patUtils::GenericLepton> &leptons, bool isMC);
    
  }
  

  //round up and show in TeX
  std::string toLatexRounded(double value, double error, double systError=-1,bool doPowers=true, double systErrorDown = -1);

  //clean up ROOT version of TeX
  void TLatexToTex(TString &expr);

  inline float deltaPhi(float phi1, float phi2) { 
     float result = phi1 - phi2;
     while (result > float(M_PI)) result -= float(2*M_PI);
     while (result <= -float(M_PI)) result += float(2*M_PI);
     return result;
  }

// FWLITE CODE

  // loop on all the lumi blocks for an EDM file in order to count the number of events that are in a sample
  // this is useful to determine how to normalize the events (compute weight)
  double getTotalNumberOfEvents(std::vector<std::string>& urls, bool fast=false, bool weightSum=false);

  unsigned long getMergeableCounterValue(const std::vector<std::string>& urls, std::string counter);
  double getMCPileupDistributionAndTotalEventFromMiniAOD(std::vector<std::string>& urls, unsigned int Npu, std::vector<float>& mcpileup);
  void getMCPileupDistributionFromMiniAOD(std::vector<std::string>& urls, unsigned int Npu, std::vector<float>& mcpileup);
  bool isGoodVertex(reco::Vertex& vtx);
  void getMCPileupDistributionFromMiniAODtemp(std::vector<std::string>& urls, unsigned int Npu, std::vector<float>& mcpileup);
  void getMCPileupDistributionFromMiniAOD(fwlite::ChainEvent& ev, unsigned int Npu, std::vector<float>& mcpileup);
  void getMCPileupDistribution(fwlite::ChainEvent& ev, unsigned int Npu, std::vector<float>& mcpileup);
  void getPileupNormalization(std::vector<float>& mcpileup, double* PUNorm, edm::LumiReWeighting* LumiWeights, utils::cmssw::PuShifter_t PuShifters);

  bool passTriggerPatternsAndGetName(edm::TriggerResultsByName& tr, std::string& pathName, std::string pattern);
  bool passTriggerPatterns(edm::TriggerResultsByName& tr, std::string pattern);
  bool passTriggerPatterns(edm::TriggerResultsByName& tr, std::string pattern1, std::string pattern2, std::string pattern3="", std::string pattern4="");
  bool passTriggerPatterns(edm::TriggerResultsByName& tr, std::vector<std::string>& patterns);

  void getHiggsLineshapeFromMiniAOD(std::vector<std::string>& urls, TH1D* hGen);

  inline bool sort_CandidatesByPt(const pat::GenericParticle &a, const pat::GenericParticle &b)  { return a.pt()>b.pt(); }
}

// CODE FOR DUPLICATE EVENTS CHECKING
//  template<> struct hash< std::string >{
//    size_t operator()( const std::string& x ) const{ return hash< const char* >()( x.c_str() );  }
//  };

class DuplicatesChecker{
 private :
  typedef std::unordered_map<std::string, bool > RunEventHashMap;
  RunEventHashMap map;
 public :
  DuplicatesChecker(){}
  ~DuplicatesChecker(){}
  void Clear(){map.clear();}
  bool isDuplicate(unsigned int Run, unsigned int Lumi, unsigned int Event){
    char tmp[255];sprintf(tmp,"%i_%i_%i",Run,Lumi,Event);
    RunEventHashMap::iterator it = map.find(tmp);
    if(it==map.end()){
      map[tmp] = true;
      return false;
    }
    return true;
  }
  bool isDuplicate(unsigned int Run, unsigned int Lumi, unsigned int Event,unsigned int cat){
    char tmp[255];sprintf(tmp,"%i_%i_%i_%i",Run,Lumi,Event,cat);
    RunEventHashMap::iterator it = map.find(tmp);
    if(it==map.end()){
      map[tmp] = true;
      return false;
    }
    return true;
  }
};


#endif
