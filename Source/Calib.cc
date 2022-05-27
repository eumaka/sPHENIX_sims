
#include "Calib.h"
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>
#include <fun4all/Fun4AllServer.h>

#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4VtxPoint.h>
#include <g4main/PHG4Particle.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHObject.h>
#include <phool/getClass.h>
#include <phool/phool.h>


#include <calobase/RawTowerDefs.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawCluster.h>
#include <calobase/RawTowerv1.h>

#include <phgeom/PHGeomUtility.h>
#include <TTree.h>
#include <TH2D.h>
#include <TVector3.h>
#include <TRandom3.h>
#include <TMath.h>

#include <fstream>



using namespace std;

//----------------------------------------------------------------------------//
//-- Constructor:
//--  simple initialization
//----------------------------------------------------------------------------//

Calib::Calib(const string &name) :
  SubsysReco(name), _event_tree(NULL)
{
	//initialize
	_event = 0;
	_outfile_name = "Tower_test.root";

}

//----------------------------------------------------------------------------//
//-- Init():
//--   Intialize all histograms, trees, and ntuples
//----------------------------------------------------------------------------//
int Calib::Init(PHCompositeNode *topNode) {

	cout << PHWHERE << " Opening file " << _outfile_name << endl;

	PHTFileServer::get().open(_outfile_name, "RECREATE");
	PHTFileServer::get().cd(_outfile_name);

	_event_tree = new TTree("event", "Calib => event info");
    
	_event_tree->Branch("event", &_event, "_event/I");
    _event_tree->Branch("oCalib_ohcal", &_oCalib_ohcal, "_oCalib_ohcal/F");
    
    
	return Fun4AllReturnCodes::EVENT_OK;
}

int Calib::InitRun(PHCompositeNode *topNode)
{
	return Fun4AllReturnCodes::EVENT_OK;
}

//----------------------------------------------------------------------------//
//-- process_event():
//--   Call user instructions for every event.
//--   This function contains the analysis structure.
//----------------------------------------------------------------------------//

int Calib::process_event(PHCompositeNode *topNode)
{
	_event++;

	GetNodes(topNode);

	fill_tree(topNode);

	return Fun4AllReturnCodes::EVENT_OK;
}

//----------------------------------------------------------------------------//
//-- End():
//--   End method, wrap everything up
//----------------------------------------------------------------------------//

int Calib::EndRun(PHCompositeNode *topNode)
{

    return Fun4AllReturnCodes::EVENT_OK;

}

int Calib::End(PHCompositeNode *topNode)
{

	PHTFileServer::get().cd(_outfile_name);
	PHTFileServer::get().write(_outfile_name);

	return Fun4AllReturnCodes::EVENT_OK;
}

//----------------------------------------------------------------------------//
//-- fill_tree():
//--   Fill the various trees...
//----------------------------------------------------------------------------//

void Calib::fill_tree(PHCompositeNode *topNode)
{

  cout << _event << endl; 

    
   
    //ohcal  towers
       int ietabin, iphibin = -1;
      float oh_ocal = 0.0;
      float OHCAL_o[24][64] = {{0.0}};

    //loop over ohcal towers
    RawTowerContainer::ConstRange begin_end_oo = _ohcal_towers_o->getTowers();
      RawTowerContainer::ConstIterator oh_o = begin_end_oo.first;
      for (; oh_o != begin_end_oo.second; ++oh_o) {
        RawTowerDefs::keytype towerid_o_o = oh_o->first;
        RawTower *rawtower_o_o = _ohcal_towers_o->getTower(towerid_o_o);
        if(rawtower_o_o) {
        RawTowerGeom *tgeo_o_o = _ohcal_towergeom->get_tower_geometry(towerid_o_o);
        // get towers energy
        oh_ocal  = rawtower_o_o->get_energy();
            
        // binning in eta and phi
        ietabin = tgeo_o_o->get_column();
        iphibin = tgeo_o_o->get_row();
        if((iphibin >= 0)&&(ietabin >= 0)) OHCAL_o[ietabin][iphibin] += oh_ocal;
       }
      }
    
  

    //---- Store energy ------//
    _oCalib_ohcal = 0.0;
  
    for(int i=0; i<24; i++){
          for(int j=0; j<64; j++){
              _oCalib_ohcal += OHCAL_o[i][j];
          }
    }
   
  
_event_tree->Fill();


return;

}

//----------------------------------------------------------------------------//
//-- GetNodes():
//--   Get all the all the required nodes off the node tree
//----------------------------------------------------------------------------//
int Calib::GetNodes(PHCompositeNode * topNode) {
    
    //oHCAL
 
    _ohcal_towers_o = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALOUT");
    if (!_ohcal_towers_o)
      {
    std::cout << PHWHERE << ": Could not find node TOWER_CALIB_HCALOUT" << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
      }
    
    
      //towergeom
    _ohcal_towergeom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");
    if (! _ohcal_towergeom)
      {
    cout << PHWHERE << ": Could not find node TOWERGEOM_HCALOUT" << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
      }
   
     return Fun4AllReturnCodes::EVENT_OK;
}


