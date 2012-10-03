
#include "TapAnalysis/interface/TapAnalysis.h"
#include "TapAnalysis/interface/PhotonIDCuts.h"

#include "TRegexp.h"
#include <iostream>

#define TapAnalysisDEBUG 0

using namespace std;

void TapAnalysis::FillFlatTree(LoopAll& l, Int_t type, Int_t ipho1, Int_t ipho2, Int_t iele1, Int_t iele2,
			       Float_t mass, Float_t weight, Float_t id) {

  l.FillTree("run", l.run);
  l.FillTree("lumis", l.lumis);
  l.FillTree("event", l.event);
  l.FillTree("type", type);
  l.FillTree("charge", l.el_std_charge[iele1]);
  l.FillTree("chargeTag", l.el_std_charge[iele2]);
  TLorentzVector* p4_tag = (TLorentzVector*) l.el_std_p4->At(iele1);
  TLorentzVector* p4_probe = (TLorentzVector*) l.el_std_p4->At(iele2);
  l.FillTree("eta", (float)p4_probe->Eta());
  l.FillTree("etatag", (float)p4_tag->Eta());
  l.FillTree("mass", mass);
  l.FillTree("weight", weight);
  l.FillTree("fbrem", l.el_std_fbrem[iele2]);  
  l.FillTree("r9tag", l.pho_r9[ipho1]);
  l.FillTree("r9", l.pho_r9[ipho2]);

  TLorentzVector* p4_pho_tag = (TLorentzVector*) l.pho_p4->At(ipho1);
  TLorentzVector* p4_pho_probe = (TLorentzVector*) l.pho_p4->At(ipho2);
  l.FillTree("ettag", (float)p4_pho_tag->E()*(float)sin(p4_tag->Theta()));
  l.FillTree("et",  (float)p4_pho_probe->E()*(float)sin(p4_probe->Theta()));
  l.FillTree("nvtx", l.vtx_std_n);
  l.FillTree("id", id);

  int thevertexind = ChooseVertex(l, iele2, true);
  Float_t mva1 = (*l.pho_mitmva)[ipho1][thevertexind];
  Float_t mva2 = (*l.pho_mitmva)[ipho2][thevertexind];
  std::cout << "Fill: " << mva1 << " "<< mva2 << std::endl;
  l.FillTree("mva1", mva1);
  l.FillTree("mva2", mva2);

  l.FillTree("catTag",  PhotonIDCategory(l, ipho1, 4));
  l.FillTree("cat", PhotonIDCategory(l, ipho2, 4));

  float rhofac = 0.09;
  float rhofacbad = 0.23;
  float isosumconst    = 2.5;
  float isosumconstbad = 2.5;

  float val_isosumoet    = ((*l.pho_pfiso_mycharged03)[ipho2][thevertexind] + l.pho_pfiso_myphoton03[ipho2] + isosumconst - l.rho_algo1*rhofac)*50./((float)p4_pho_tag->E()*(float)sin(p4_tag->Theta()));
  float val_isoqsumoetbad = (l.pho_pfiso_myphoton04[ipho2] + l.pho_pfiso_charged_badvtx_04[ipho2] + isosumconstbad - l.rho_algo1*rhofacbad)*50./((float)p4_pho_tag->E()*(float)sin(p4_tag->Theta()));
  float val_trkisooet    = ((*l.pho_pfiso_mycharged03)[ipho2][thevertexind])*50./((float)p4_pho_tag->E()*(float)sin(p4_tag->Theta()));

  l.FillTree("sieie", l.pho_sieie[ipho2]);
  l.FillTree("isoRvtx", val_isosumoet);
  l.FillTree("isoWvtx", val_isoqsumoetbad);
  l.FillTree("hoe", l.pho_hoe[ipho2]);
  l.FillTree("isoTk", val_trkisooet);

}

// ----------------------------------------------------------------------------------------------------
TapAnalysis::TapAnalysis()  : 
  name_("TapAnalysis")
{}

// ----------------------------------------------------------------------------------------------------
TapAnalysis::~TapAnalysis() 
{}

// ----------------------------------------------------------------------------------------------------
void TapAnalysis::Term(LoopAll& l) 
{}

// ----------------------------------------------------------------------------------------------------
void TapAnalysis::Init(LoopAll& l) {
  if(TapAnalysisDEBUG) 
    cout << "InitRealTapAnalysis START"<<endl;
  
  l.tmvaReaderID_Single_Barrel = new TMVA::Reader("!Color:Silent");
  l.tmvaReaderID_Single_Barrel->AddVariable("ph.r9",   &l.tmva_photonid_r9 );
  l.tmvaReaderID_Single_Barrel->AddVariable("ph.sigietaieta",   &l.tmva_photonid_sieie );
  l.tmvaReaderID_Single_Barrel->AddVariable("ph.scetawidth",   &l.tmva_photonid_etawidth );
  l.tmvaReaderID_Single_Barrel->AddVariable("ph.scphiwidth",   &l.tmva_photonid_phiwidth );
  l.tmvaReaderID_Single_Barrel->AddVariable("ph.idmva_CoviEtaiPhi",   &l.tmva_photonid_sieip );
  l.tmvaReaderID_Single_Barrel->AddVariable("ph.idmva_s4ratio",   &l.tmva_photonid_s4ratio );
  l.tmvaReaderID_Single_Barrel->AddVariable("ph.idmva_GammaIso",   &l.tmva_photonid_pfphotoniso03 );
  l.tmvaReaderID_Single_Barrel->AddVariable("ph.idmva_ChargedIso_selvtx",   &l.tmva_photonid_pfchargedisogood03 );
  l.tmvaReaderID_Single_Barrel->AddVariable("ph.idmva_ChargedIso_worstvtx",   &l.tmva_photonid_pfchargedisobad03 );
  l.tmvaReaderID_Single_Barrel->AddVariable("ph.sceta",   &l.tmva_photonid_sceta );
  l.tmvaReaderID_Single_Barrel->AddVariable("rho",   &l.tmva_photonid_eventrho );
  l.tmvaReaderID_Single_Barrel->BookMVA("AdaBoost", "aux/2012ICHEP_PhotonID_Barrel_BDT.weights.xml");

  l.tmvaReaderID_Single_Endcap = new TMVA::Reader("!Color:Silent");
  l.tmvaReaderID_Single_Endcap->AddVariable("ph.r9",   &l.tmva_photonid_r9 );
  l.tmvaReaderID_Single_Endcap->AddVariable("ph.sigietaieta",   &l.tmva_photonid_sieie );
  l.tmvaReaderID_Single_Endcap->AddVariable("ph.scetawidth",   &l.tmva_photonid_etawidth );
  l.tmvaReaderID_Single_Endcap->AddVariable("ph.scphiwidth",   &l.tmva_photonid_phiwidth );
  l.tmvaReaderID_Single_Endcap->AddVariable("ph.idmva_CoviEtaiPhi",   &l.tmva_photonid_sieip );
  l.tmvaReaderID_Single_Endcap->AddVariable("ph.idmva_s4ratio",   &l.tmva_photonid_s4ratio );
  l.tmvaReaderID_Single_Endcap->AddVariable("ph.idmva_GammaIso",   &l.tmva_photonid_pfphotoniso03 );
  l.tmvaReaderID_Single_Endcap->AddVariable("ph.idmva_ChargedIso_selvtx",   &l.tmva_photonid_pfchargedisogood03 );
  l.tmvaReaderID_Single_Endcap->AddVariable("ph.idmva_ChargedIso_worstvtx",   &l.tmva_photonid_pfchargedisobad03 );
  l.tmvaReaderID_Single_Endcap->AddVariable("ph.sceta",   &l.tmva_photonid_sceta );
  l.tmvaReaderID_Single_Endcap->AddVariable("rho",   &l.tmva_photonid_eventrho );
  l.tmvaReaderID_Single_Endcap->AddVariable("ph.idmva_PsEffWidthSigmaRR",   &l.tmva_photonid_ESEffSigmaRR );
  l.tmvaReaderID_Single_Endcap->BookMVA("AdaBoost", "aux/2012ICHEP_PhotonID_Endcap_BDT.weights_PSCorr.xml");

  
  const int phoNCUTS = LoopAll::phoNCUTS;
  const int phoCiC6NCATEGORIES = LoopAll::phoCiC6NCATEGORIES;
  const int phoCiC4NCATEGORIES = LoopAll::phoCiC4NCATEGORIES;
  const int phoNCUTLEVELS = LoopAll::phoNCUTLEVELS;
  
  for(int iLevel=0; iLevel<phoNCUTLEVELS; ++iLevel) {
    float cic6_cuts_lead[phoNCUTS][phoCiC6NCATEGORIES];
    float cic6_cuts_sublead[phoNCUTS][phoCiC6NCATEGORIES];
    float cic4_cuts_lead[phoNCUTS][phoCiC4NCATEGORIES];
    float cic4_cuts_sublead[phoNCUTS][phoCiC4NCATEGORIES];
    float cic4pf_cuts_lead[phoNCUTS][phoCiC4NCATEGORIES];
    float cic4pf_cuts_sublead[phoNCUTS][phoCiC4NCATEGORIES];
    
    // get the cut values for the current photon CIC level 
    l.SetPhotonCutsInCategories((LoopAll::phoCiCIDLevel)iLevel, 
				&cic6_cuts_lead[0][0], &cic6_cuts_sublead[0][0], 
				&cic4_cuts_lead[0][0], &cic4_cuts_sublead[0][0],
				&cic4pf_cuts_lead[0][0], &cic4pf_cuts_sublead[0][0]);
    
    // rearrange the returned values to arrays with more meaningful names
    float * cic6_cuts_arrays_lead[phoNCUTS] = {
      &l.cic6_cut_lead_isosumoet[0][0], 
      &l.cic6_cut_lead_isosumoetbad[0][0], 
      &l.cic6_cut_lead_trkisooet[0][0], 
      &l.cic6_cut_lead_sieie[0][0],
      &l.cic6_cut_lead_hovere[0][0],
      &l.cic6_cut_lead_r9[0][0], 
      &l.cic6_cut_lead_drtotk_25_99[0][0], 
      &l.cic6_cut_lead_pixel[0][0] 
    };
    
    float * cic6_cuts_arrays_sublead[phoNCUTS] = {
      &l.cic6_cut_sublead_isosumoet[0][0], 
      &l.cic6_cut_sublead_isosumoetbad[0][0], 
      &l.cic6_cut_sublead_trkisooet[0][0], 
      &l.cic6_cut_sublead_sieie[0][0], 
      &l.cic6_cut_sublead_hovere[0][0], 
      &l.cic6_cut_sublead_r9[0][0],
      &l.cic6_cut_sublead_drtotk_25_99[0][0], 
      &l.cic6_cut_sublead_pixel[0][0]
    };
    
    float * cic4_cuts_arrays_lead[phoNCUTS] = {
      &l.cic4_cut_lead_isosumoet[0][0], 
      &l.cic4_cut_lead_isosumoetbad[0][0], 
      &l.cic4_cut_lead_trkisooet[0][0], 
      &l.cic4_cut_lead_sieie[0][0],
      &l.cic4_cut_lead_hovere[0][0], 
      &l.cic4_cut_lead_r9[0][0], 
      &l.cic4_cut_lead_drtotk_25_99[0][0], 
      &l.cic4_cut_lead_pixel[0][0] 
    };
    
    float * cic4_cuts_arrays_sublead[phoNCUTS] = {
      &l.cic4_cut_sublead_isosumoet[0][0], 
      &l.cic4_cut_sublead_isosumoetbad[0][0], 
      &l.cic4_cut_sublead_trkisooet[0][0], 
      &l.cic4_cut_sublead_sieie[0][0], 
      &l.cic4_cut_sublead_hovere[0][0], 
      &l.cic4_cut_sublead_r9[0][0],
      &l.cic4_cut_sublead_drtotk_25_99[0][0], 
      &l.cic4_cut_sublead_pixel[0][0]
    };
    
    float * cic4pf_cuts_arrays_lead[phoNCUTS] = {
      &l.cic4pf_cut_lead_isosumoet[0][0], 
      &l.cic4pf_cut_lead_isosumoetbad[0][0], 
      &l.cic4pf_cut_lead_trkisooet[0][0], 
      &l.cic4pf_cut_lead_sieie[0][0],
      &l.cic4pf_cut_lead_hovere[0][0], 
      &l.cic4pf_cut_lead_r9[0][0], 
      &l.cic4pf_cut_lead_drtotk_25_99[0][0], 
      &l.cic4pf_cut_lead_pixel[0][0] 
    };
    
    float * cic4pf_cuts_arrays_sublead[phoNCUTS] = {
      &l.cic4pf_cut_sublead_isosumoet[0][0], 
      &l.cic4pf_cut_sublead_isosumoetbad[0][0], 
      &l.cic4pf_cut_sublead_trkisooet[0][0], 
      &l.cic4pf_cut_sublead_sieie[0][0], 
      &l.cic4pf_cut_sublead_hovere[0][0], 
      &l.cic4pf_cut_sublead_r9[0][0],
      &l.cic4pf_cut_sublead_drtotk_25_99[0][0], 
      &l.cic4pf_cut_sublead_pixel[0][0]
    };
    for(int iCut=0; iCut<phoNCUTS; ++iCut) {
      for(int iCat=0; iCat<phoCiC6NCATEGORIES; ++iCat) {
	cic6_cuts_arrays_lead[iCut][iLevel*phoCiC6NCATEGORIES+iCat] = cic6_cuts_lead[iCut][iCat];
	cic6_cuts_arrays_sublead[iCut][iLevel*phoCiC6NCATEGORIES+iCat] = cic6_cuts_sublead[iCut][iCat];
      }
      for(int iCat=0; iCat<phoCiC4NCATEGORIES; ++iCat) {
	cic4_cuts_arrays_lead[iCut][iLevel*phoCiC4NCATEGORIES+iCat] = cic4_cuts_lead[iCut][iCat];
	cic4_cuts_arrays_sublead[iCut][iLevel*phoCiC4NCATEGORIES+iCat] = cic4_cuts_sublead[iCut][iCat];
      }
      for(int iCat=0; iCat<phoCiC4NCATEGORIES; ++iCat) {
	cic4pf_cuts_arrays_lead[iCut][iLevel*phoCiC4NCATEGORIES+iCat] = cic4pf_cuts_lead[iCut][iCat];
	cic4pf_cuts_arrays_sublead[iCut][iLevel*phoCiC4NCATEGORIES+iCat] = cic4pf_cuts_sublead[iCut][iCat];
      }
    }
  } // end of loop over all photon cut levels
  

  /* -------------------------------------------------------------------------------------------
     Pileup Reweighting
     https://twiki.cern.ch/twiki/bin/viewauth/CMS/PileupReweighting
     ----------------------------------------------------------------------------------------------  */
  if (puHist != "" && puHist != "auto" ) {
    if(DEBUG) 
      cout << "Opening PU file"<<endl;
    TFile* puFile = TFile::Open( puHist );
    if (puFile) {
      TH1 * target = 0;
      
      if( puTarget != "" ) {
        TFile * puTargetFile = TFile::Open( puTarget ); 
        assert( puTargetFile != 0 );
        target = (TH1*)puTargetFile->Get("pileup");
        if( target == 0 ) { target = (TH1*)puTargetFile->Get("target_pu"); }
        target->Scale( 1. / target->Integral() );
      }
      
      if( puMap != "" ) {
	loadPuMap(puMap, puFile, target); 
      } else {
	loadPuWeights(0, puFile, target);
      }
      puFile->Close();
    }
    else {
      cout<<"Error opening " <<puHist<<" pileup reweighting histogram, using 1.0"<<endl; 
      weights[0].resize(50);
      for (unsigned int i=0; i<weights[0].size(); i++) weights[0][i] = 1.0;
    }
    if(DEBUG) 
      cout << "Opening PU file END"<<endl;
  } else if ( puHist == "auto" ) {
    TFile * puTargetFile = TFile::Open( puTarget ); 
    assert( puTargetFile != 0 );
    puTargetHist = (TH1*)puTargetFile->Get("pileup");
    if( puTargetHist == 0 ) { 
      puTargetHist = (TH1*)puTargetFile->Get("target_pu"); 
    }
    puTargetHist = (TH1*)puTargetHist->Clone();
    puTargetHist->SetDirectory(0);
    puTargetHist->Scale( 1. / puTargetHist->Integral() );
    puTargetFile->Close();
  }

  if(TapAnalysisDEBUG) 
    cout <<"InitRealTapAnalysis END"<<endl;
}

// ----------------------------------------------------------------------------------------------------
bool TapAnalysis::Analysis(LoopAll& l, Int_t jentry) {

  if(TapAnalysisDEBUG) 
    cout <<"Analysis START"<<endl;
  /*
   if (puHist != "" && puHist != "auto" ) {


        if(TapAnalysisDEBUG) 
            cout << "Opening PU file"<<endl;
        TFile* puFile = TFile::Open( puHist );
        if (puFile) {
        TH1 * target = 0;
        
        if( puTarget != "" ) {
        TFile * puTargetFile = TFile::Open( puTarget ); 
        assert( puTargetFile != 0 );
        target = (TH1*)puTargetFile->Get("pileup");
        if( target == 0 ) { target = (TH1*)puTargetFile->Get("target_pileup"); }
        target->Scale( 1. / target->Integral() );
        }
        
            if( puMap != "" ) {
                loadPuMap(puMap, puFile, target); 
            } else {
                loadPuWeights(0, puFile, target);
            }
            puFile->Close();
        }
        else {
            cout<<"Error opening " <<puHist<<" pileup reweighting histogram, using 1.0"<<endl; 
            weights[0].resize(50);
            for (unsigned int i=0; i<weights[0].size(); i++) weights[0][i] = 1.0;
        }
        if(TapAnalysisDEBUG) 
            cout << "Opening PU file END"<<endl;
    } else if ( puHist == "auto" ) {
    TFile * puTargetFile = TFile::Open( puTarget ); 
    assert( puTargetFile != 0 );
    puTargetHist = (TH1*)puTargetFile->Get("pileup");
    if( puTargetHist == 0 ) { puTargetHist = (TH1*)puTargetFile->Get("target_pu"); }
    puTargetHist = (TH1*)puTargetHist->Clone();
    puTargetHist->SetDirectory(0);
    puTargetHist->Scale( 1. / puTargetHist->Integral() );
    puTargetFile->Close();
    }
  */
    //apply pileup reweighting
    unsigned int n_pu = l.pu_n;
    float weight = getPuWeight(l.pu_n, l.itype[l.current], &(l.sampleContainer[l.current_sample_index]), jentry == 1) * l.sampleContainer[l.current_sample_index].weight;
    /*
    float weight =1.;
    if (l.itype[l.current] !=0 && puHist != "") {
        std::vector<double> & puweights = weights.find( l.itype[l.current] ) != weights.end() ? weights[ l.itype[l.current] ] : weights[0]; 
        if(n_pu<puweights.size()){
            weight *= puweights[n_pu];
        }    
        else{ //should not happen as we have a weight for all simulated n_pu multiplicities!
            cout<<"n_pu ("<< n_pu<<") too big ("<<puweights.size()<<"), event will not be reweighted for pileup"<<endl;
        }
    }
    */
  // move ate reduction level
  bool hlTrigger = true; //checkElectronHLT(iElectron, mp, indexfiles, itype, jentry);
  if(!hlTrigger)
    return 0;

  // MET CUT
  if (l.met_pfmet > cutPFMET) 
    return 0;

  ////////////////
  // TAG ELECTRONS
  ////////////////

  // Vector of protoTag (tags without eID selection)
  std::vector<int> selectedTags; selectedTags.clear();
  
  for(unsigned int iElectron = 0; iElectron < l.el_std_n; iElectron++) {
    int iIndexSuperCluster = l.el_std_scind[iElectron];
 
    if (iIndexSuperCluster == -1)
      continue;
 
    TLorentzVector p4_sc = *((TLorentzVector *) l.el_std_sc->At(iElectron));
 
    if(fabs(p4_sc.Eta()) > 1.4442 && fabs(p4_sc.Eta()) < 1.566)
      continue;
    if (fabs(p4_sc.Et()) < cutETTag)
      continue;
 
    Int_t phoindex = -1;
    for (int k=0; k < l.pho_n; k++) {
      if (l.pho_scind[k] == l.el_std_scind[iElectron])
	phoindex = k;
    }
 
    if (phoindex != -1) {
      if (((applyPreselection > 0) && (PhotonId(l, iElectron, phoindex, std::string("Presel"), 1) > 0)) ||
	  applyPreselection == 0) {
	 if (PhotonId(l, iElectron, phoindex, selectionTypeTag, cutSelectionTag) > 0.) {
	  selectedTags.push_back(iElectron);
	}
      }
    }
  }

  //std::cout << "tags" << selectedTags.size() << std::endl;
    
  ////////////////////////////
  // ELECTRON PROBE CANDIDATES
  ////////////////////////////
  // Vector of probe electrons without eID requirements
  std::vector<int> selectedProbes; selectedProbes.clear();
  
  // Define Probe Candidates
  for (unsigned int iEl = 0; iEl<l.el_std_n; iEl++){
    int iIndexSuperCluster = l.el_std_scind[iEl];
    
    // CHECK ELECTRON TYPE
    if(iIndexSuperCluster == -1)
      continue;

    TLorentzVector p4_sc = *((TLorentzVector *) l.el_std_sc->At(iEl));
    if( fabs(p4_sc.Eta()) > 1.4442 && fabs(p4_sc.Eta()) < 1.566)
      continue;
    
    if(p4_sc.Et() < cutETProbe)
      continue;
	  
    Int_t phoindex = -1;
    for (int k=0; k < l.pho_n; k++) {
      if (l.pho_scind[k] == l.el_std_scind[iEl])
	phoindex = k;
    }
	  
    if (phoindex != -1) {
      if (((applyPreselection > 0) && (PhotonId(l, iEl, phoindex, std::string("Presel"), 1) > 0)) ||
	  applyPreselection == 0) {
	if (PhotonId(l, iEl, phoindex, selectionTypeProbe, cutSelectionProbe) > 0)
	  selectedProbes.push_back(iEl);
      }
    }
  }

  //std::cout << "probes" << selectedProbes.size() << std::endl;

  std::vector<std::pair<int, int> > phoEffPairs; phoEffPairs.clear();
  phoEffPairs = TPPairs(l, selectedTags, selectedProbes, 1, 0);
  
  for (unsigned int tpPair=0; tpPair<phoEffPairs.size(); tpPair++) {
    
    TLorentzVector p4_tag = *((TLorentzVector*) l.el_std_p4->At(phoEffPairs[tpPair].first));
    TLorentzVector* p4_probe = (TLorentzVector*) l.el_std_p4->At(phoEffPairs[tpPair].second);

    // APPLY Energy corrections 
    /*
      if (itype == 0) {
      if (fabs(p4_tag.Eta()) < 1.479) {
	p4_tag.SetE(p4_tag.E() *  (double)GetCutValue("value_EnergyCorrection_EB",0));
	p4_tag.SetPx(p4_tag.Px() *  (double)GetCutValue("value_EnergyCorrection_EB",0));
	p4_tag.SetPy(p4_tag.Py() *  (double)GetCutValue("value_EnergyCorrection_EB",0));
	p4_tag.SetPz(p4_tag.Pz() *  (double)GetCutValue("value_EnergyCorrection_EB",0));
      } else {
	p4_tag.SetE(p4_tag.E() *  (double)GetCutValue("value_EnergyCorrection_EE",0));
	p4_tag.SetPx(p4_tag.Px() *  (double)GetCutValue("value_EnergyCorrection_EE",0));
	p4_tag.SetPy(p4_tag.Py() *  (double)GetCutValue("value_EnergyCorrection_EE",0));
	p4_tag.SetPz(p4_tag.Pz() *  (double)GetCutValue("value_EnergyCorrection_EE",0));
      }
      
      if (fabs(p4_probe->Eta()) < 1.479) {
	p4_probe->SetE(p4_probe->E() *  (double)GetCutValue("value_EnergyCorrection_EB",0));	
	p4_probe->SetPx(p4_probe->Px() *  (double)GetCutValue("value_EnergyCorrection_EB",0));
	p4_probe->SetPy(p4_probe->Py() *  (double)GetCutValue("value_EnergyCorrection_EB",0));
	p4_probe->SetPz(p4_probe->Pz() *  (double)GetCutValue("value_EnergyCorrection_EB",0));
      } else {
	p4_probe->SetPx(p4_probe->Px() *  (double)GetCutValue("value_EnergyCorrection_EE",0));
	p4_probe->SetPy(p4_probe->Py() *  (double)GetCutValue("value_EnergyCorrection_EE",0));
	p4_probe->SetPz(p4_probe->Pz() *  (double)GetCutValue("value_EnergyCorrection_EE",0));
      }
    }
    */

    double zMass = (p4_tag + *p4_probe).Mag();
    //cout<< " zmass = "<<zMass<<endl;
	
    if ((zMass > minZMass) && (zMass < maxZMass)) {
      
      int iElTag = phoEffPairs[tpPair].first;
      int iEl = phoEffPairs[tpPair].second;
      
      //std::cout << "Pair" << std::endl;
      bool pass = false;
      Int_t phoindex_tag = -1;
      Int_t phoindex = -1;
      for (int k=0; k < l.pho_n; k++) {
	if (l.pho_scind[k] == l.el_std_scind[iEl])
	  phoindex = k;
	if (l.pho_scind[k] == l.el_std_scind[iElTag])
	  phoindex_tag = k;
      }
      
      if (phoindex != -1) {
	int type = l.itype[l.current];
	Float_t id = PhotonId(l, iEl, phoindex, selectionTypeToMeasure, cutSelectionToMeasure);	
	FillFlatTree(l, type, phoindex_tag, phoindex, iElTag, iEl, zMass, weight, id);
	
	return true;
      }
    }
  }    

  if(TapAnalysisDEBUG) 
    cout<<"Analysis END"<<endl;
  
  return false;
}

std::vector<std::pair<int, int> > TapAnalysis::TPPairs(LoopAll& l, std::vector<int> tags, std::vector<int> probes, int type, int chargePairing) {
  
  std::vector<std::pair<int, int> > tpPairs;

  if (type == 0)
    chargePairing = 0;
  if (chargePairing > 0)
    chargePairing = 1;
  if (chargePairing < 0)
    chargePairing = -1;
  
  // PRODUCE TAG AND PROBE PAIRS
  for(unsigned int i=0; i<tags.size(); i++) {
    for(unsigned int j=0; j<probes.size(); j++) {
      
      // CHECK TAG != PROBE
      if (type == 1) {
	if(probes[j] == tags[i])
	  continue;
      } else { // FOR SC PROBES
	if (probes[j] == l.el_std_scind[tags[i]])
	  continue;
      }
      
      if (chargePairing == 0)
	tpPairs.push_back( std::pair<int, int>(tags[i], probes[j]));
      else if (chargePairing == (l.el_std_charge[tags[i]]*l.el_std_charge[probes[j]]))
	tpPairs.push_back( std::pair<int, int>(tags[i], probes[j]));
    }
  }

  /*
  if (chargePairing == 0) {
    for (int i=0; i<tpPairs.size(); i++) {
      std::cout << tpPairs[i].first << " " << tpPairs[i].second << std::endl; 
    }
    std::cout << "__________" << std::endl;
  }
  */
  return tpPairs;
}

// ----------------------------------------------------------------------------------------------------
void TapAnalysis::GetBranches(TTree *t, std::set<TBranch *>& s ) 
{}

// ----------------------------------------------------------------------------------------------------
void TapAnalysis::FillReductionVariables(LoopAll& l, int jentry) {

  l.pho_mitmva->clear();
  l.pho_pfiso_mycharged02->clear();
  l.pho_pfiso_mycharged03->clear();
  l.pho_pfiso_mycharged04->clear();
  l.pho_pfiso_mycharged02->resize(l.pho_n, std::vector<float>(l.vtx_std_n, 0));
  l.pho_pfiso_mycharged03->resize(l.pho_n, std::vector<float>(l.vtx_std_n, 0));
  l.pho_pfiso_mycharged04->resize(l.pho_n, std::vector<float>(l.vtx_std_n, 0));

  for (int ipho=0;ipho<l.pho_n;ipho++){
    l.pho_s4ratio[ipho]  = l.pho_e2x2[ipho]/l.pho_e5x5[ipho];
    float rr2=l.pho_eseffsixix[ipho]*l.pho_eseffsixix[ipho]+l.pho_eseffsiyiy[ipho]*l.pho_eseffsiyiy[ipho];
    l.pho_ESEffSigmaRR[ipho] = 0.0; 
    if(rr2>0. && rr2<999999.) {
      l.pho_ESEffSigmaRR[ipho] = sqrt(rr2);
    }
  }

  // Data driven MC corrections to cluster shape variables and energy resolution estimate
  if (l.itype[l.current] !=0)
    rescaleClusterVariables(l);

  for(int ipho=0; ipho<l.pho_n; ++ipho) {
    std::vector<float> temp;
    float neu03 = l.pfEcalIso(ipho, 0.3, 0., 0., 0., 0., 0., 0., 5);
    float neu04 = l.pfEcalIso(ipho, 0.4, 0., 0., 0., 0., 0., 0., 5); 
    l.pho_pfiso_myneutral03[ipho] = neu03;
    l.pho_pfiso_myneutral04[ipho] = neu04;

    float pho03 = l.pfEcalIso(ipho, 0.3, 0., 0.070, 0.015, 0., 0., 0.);
    float pho04 = l.pfEcalIso(ipho, 0.4, 0., 0.070, 0.015, 0., 0., 0.); 
    l.pho_pfiso_myphoton03[ipho] = pho03;
    l.pho_pfiso_myphoton04[ipho] = pho04;
	
    float badiso = 0;
    for(int ivtx=0; ivtx<l.vtx_std_n; ++ivtx) {
      
      float ch02 = l.pfTkIsoWithVertex(ipho,ivtx,0.2,0.02,0.02,0.0,0.2,0.1);
      float ch03 = l.pfTkIsoWithVertex(ipho,ivtx,0.3,0.02,0.02,0.0,0.2,0.1);
      float ch04 = l.pfTkIsoWithVertex(ipho,ivtx,0.4,0.02,0.02,0.0,0.2,0.1);
      l.pho_pfiso_mycharged02->at(ipho).at(ivtx) = ch02;
      l.pho_pfiso_mycharged03->at(ipho).at(ivtx) = ch03;
      l.pho_pfiso_mycharged04->at(ipho).at(ivtx) = ch04;
      
      TLorentzVector p4 = get_pho_p4(l, ipho, ivtx);//(TLorentzVector*)l.pho_p4->At(ipho);
      //std::cout << ipho << " "<< ivtx << " " << l.photonIDMVANew(ipho, ivtx, p4, "") << std::endl;
      temp.push_back(l.photonIDMVANew(ipho, ivtx, p4, ""));
      
      if( ch04 > badiso ) {
	badiso = ch04;
	//badvtx = ivtx;
      }
    }
   
    //pho_tkiso_badvtx_id[ipho] = badvtx;
    l.pho_pfiso_charged_badvtx_04[ipho] = badiso;
    l.pho_mitmva->push_back(temp);
  }
}

// ----------------------------------------------------------------------------------------------------
bool TapAnalysis::SelectEventsReduction(LoopAll& l, int jentry) {
  
  std::vector<int> eleIndex; 
  
  int goodEl = 0;
  for(int i =0; i<l.el_std_n; i++) {
    TLorentzVector* p4 = (TLorentzVector*)l.el_std_sc->At(i);
    if (p4->Et() > 20.)
      eleIndex.push_back(i);
  }
  
  Float_t mass = 0.;
  if (eleIndex.size() > 1) {
    for (unsigned int i=0; i<eleIndex.size()-1; i++) {
      TLorentzVector* p1 = (TLorentzVector*)l.el_std_sc->At(eleIndex[i]);
      
      for (unsigned int j=i+1; j<eleIndex.size(); j++) {
	TLorentzVector* p2 = (TLorentzVector*)l.el_std_sc->At(eleIndex[j]);
	Float_t mass_temp = ((*p1)+(*p2)).M();
	if (mass < mass_temp) {
	  mass = mass_temp;
	}
      }
    }
    
    if (mass > 60.)
      return 1;
  }
    
  return 0;
  
  if(TapAnalysisDEBUG)  cout << " ****************** SelectEventsReduction " << endl;
  return true;
}

// ----------------------------------------------------------------------------------------------------
bool TapAnalysis::SkimEvents(LoopAll& l, int jentry) {
  return true;
}

// ----------------------------------------------------------------------------------------------------
bool TapAnalysis::SelectEvents(LoopAll& l, int jentry) {
  return true;
}

// ----------------------------------------------------------------------------------------------------
void TapAnalysis::ReducedOutputTree(LoopAll &l, TTree * outputTree) {
  l.pho_mitmva = new std::vector<std::vector<float> >();

  if (outputTree) {
    l.Branch_pho_mitmva(outputTree);
    l.Branch_pho_pfiso_charged_badvtx_04(outputTree);
  }
}

// ----------------------------------------------------------------------------------------------------
void TapAnalysis::ResetAnalysis()
{}

Float_t TapAnalysis::PhotonId(LoopAll& l, Int_t eleIndex, Int_t phoIndex, std::string type, Float_t selection) {

  if (selection < -10.)
    return true;
 
  Float_t result = 0.;

  int thevertexind = ChooseVertex(l, eleIndex, true);

  if (type == "CiC4PF") {
    std::vector<std::vector<bool> > ph_passcut;
    int level = l.PhotonCiCPFSelectionLevel(phoIndex, thevertexind, ph_passcut, 4, 0);
    if (level >= selection)
      result = 1;
  } else if (type == "MVA") {
    std::cout << "MVA: " << (*l.pho_mitmva)[phoIndex][thevertexind]  << std::endl;
    if ((*l.pho_mitmva)[phoIndex][thevertexind] > selection)
      result = 1.;
    //result = ((*l.pho_mitmva)[phoIndex][thevertexind] > selection);
  } else if (type == "Presel") {
    if (l.PhotonMITPreSelection(phoIndex, thevertexind, 0))
      result = 1.0;
    //result = l.PhotonMITPreSelection(phoIndex, thevertexind, 0);
  }

  return result;
}

bool TapAnalysis::checkEventHLT(LoopAll& l, std::vector<std::string> paths) {

  bool result = false;
  std::vector<unsigned short> hltNumbers;

  for (unsigned int i=0; i<paths.size(); i++) { 
    TRegexp e(TString(paths[i].c_str()));
    for (unsigned int j=0; j<l.hlt_path_names_HLT1->size(); j++) {
      TString str1((*l.hlt_path_names_HLT1)[j].c_str());
      //std::cout << (*hlt_path_names_HLT1)[j] << std::endl;
      if (str1.Contains(e)) {	
	//std::cout << (*hlt_path_names_HLT1)[j] << std::endl;
	hltNumbers.push_back(j);
      }
    }
  }

  for (int j=0; j< hltNumbers.size(); j++) {
    for (int i=0; i<(*l.hlt1_bit).size(); i++) {
      if (hltNumbers[j] == (*l.hlt1_bit)[i]) {
	result = true;
	break;
      }
    }
  }

  return result;
}

int TapAnalysis::ChooseVertex(LoopAll& l, int iEl, bool isGood) {

  TVector3* el_pos = (TVector3*)l.el_std_posvtx->At(iEl); 

  float dist[100];
  int index[100];

  for (int i=0; i<l.vtx_std_n; i++) {
    TVector3* pos = (TVector3*)l.vtx_std_xyz->At(i); 
    dist[i] = ((*pos) - (*el_pos)).Mag();
    index[i] = i;
  }
  
  for (int i=0; i<l.vtx_std_n-1; i++) {
    for (int j=i+1; j<l.vtx_std_n; j++) {
      if (dist[i] >= dist[j]) {
	float ftemp = dist[j]; 
	dist[j] = dist[i];
	dist[i] = ftemp;

	int itemp = index[j]; 
	index[j] = index[i];
	index[i] = itemp;
      }
    }
  }
  
  return index[0];
  /*
  // TOGLI IL VERTICE dell'elettrone e prendi il best sum pT

  int secondGoodIndex = index[0];
  float maxPt = 0;
  for(int i=1; i<l.vtx_std_n; i++) {
    float temp = SumPt2(jentry, index[i]);
    if (temp > maxPt) {
      maxPt = temp;
      secondGoodIndex = index[i];
    }
  }
    
  if (isGood) 
    return index[0];
  else 
    return secondGoodIndex;

  return -1;
  */
}	

TLorentzVector TapAnalysis::get_pho_p4(LoopAll& l, Int_t ipho, int ivtx) {

  TVector3 * vtx = (TVector3*) l.vtx_std_xyz->At(ivtx);
  TVector3 direction = *((TVector3*)l.sc_xyz->At(l.pho_scind[ipho])) - *vtx;
  TVector3 p = direction.Unit() * ((TLorentzVector*)l.pho_p4->At(ipho))->Energy();
  TLorentzVector p4(p.x(),p.y(),p.z(),((TLorentzVector*)l.pho_p4->At(ipho))->Energy());
  return p4;
}

bool TapAnalysis::CiCPhotonIDPF(LoopAll& l, int nCategories, int photonindex, int chosenVtx, int IDlevel) {
  
  bool passes = true;

  float vars[10];
  //float rhofacbad=0.52, rhofac=0.17;
  // need pho_calopos and pho_r9 branches here
  int thisCat = PhotonIDCategory(l, photonindex, nCategories);  
  TLorentzVector phop4 = get_pho_p4(l, photonindex, chosenVtx);

  float rhofac = 0.09;
  float rhofacbad = 0.23;
  float isosumconst    = 2.5;
  float isosumconstbad = 2.5;

  float val_isosumoet    = ((*l.pho_pfiso_mycharged03)[photonindex][chosenVtx] + l.pho_pfiso_myphoton03[photonindex] + isosumconst - l.rho_algo1*rhofac)*50./phop4.Et();
  float val_isosumoetbad = (l.pho_pfiso_myphoton04[photonindex] + l.pho_pfiso_charged_badvtx_04[photonindex] + isosumconstbad - l.rho_algo1*rhofacbad)*50./phop4.Et();
  float val_trkisooet    = ((*l.pho_pfiso_mycharged03)[photonindex][chosenVtx])*50./phop4.Et();

  vars[0] = val_isosumoet;
  vars[1] = val_isosumoetbad;
  vars[2] = val_trkisooet;

  vars[3] = l.pho_sieie[photonindex];
  vars[4] = l.pho_hoe[photonindex];
  vars[5] = l.pho_r9[photonindex];
  vars[6] = 99.; //DeltaRToTrackHgg(jentry, photonindex, chosenVtx, 2.5, 1., 0.1,0);
  vars[7] = 0;//(float)pho_haspixseed[photonindex];

  for(int var=0; var<NVARS; var++){
    std::cout << vars[var] << " " << phoIDcuts4catPF[IDlevel][var][thisCat] << std::endl;
    if(var == 5 || var == 6){   //cuts from below
      if(vars[var] < phoIDcuts4catPF[IDlevel][var][thisCat]) {
	passes =false;
	break;
      }
    } else {                    //cuts from above
      if (vars[var] > phoIDcuts4catPF[IDlevel][var][thisCat]) {
	passes =false;
	break;
      }
    }
  }
  std::cout << "_______________" << std::endl;
  //std::cout << passes << std::endl;
  return passes;
}

int TapAnalysis::PhotonIDCategory(LoopAll& l, int photonindex, int nCategories) {

  int thisCat = -1;
  
  float eta = fabs(((TVector3*)l.sc_xyz->At(l.pho_scind[photonindex]))->Eta());
  float r9  = l.pho_r9[photonindex];

  if (nCategories == 4) {
    bool etaCat = eta>1.479;
    bool r9Cat  = r9<0.94;
    
    thisCat = 2*(etaCat) + r9Cat;
  } else if (nCategories == 6) {
    bool r9Cat = (Int_t)(r9<0.94) + (r9<0.9); // 0, 1, or 2 (high r9 --> low r9)
    bool etaCat = eta>1.479;

    thisCat = 3*(etaCat) + r9Cat;
  } else {
    std::cout << "Wrong number of categories." << std::endl;
  }
    
  return thisCat;
}