#include <algorithm>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include <cmath>
#include <map>
#include <vector>

#include "GEvent.h"

#include "TMath.h"
#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TRandom3.h"
#include "TObject.h"

#include <sys/types.h>
#include <unistd.h>

using namespace std;

#define MAXID   12
#define ARGS    "i:o:"
#define MAXCH   1000
#define SQ2     1.414213562
#define FWHM    2.354820045
#define BWIDTH  25

// Options
struct OPT
{
    unsigned long long int   skipEvents  ;
    long long int   numEvents   ;
    unsigned long long int   firstEvent  ;
    unsigned long long int   lastEvent   ;
    short int       maxChannels ;
    short int       numFiles    ;
    short int       pulserDt    ;
    double          pileUpTime_F;
    double          pileUpTime_B;
    double          coincTime   ;
    double          integTime   ;
    double          startTime   ;
    double          deadTime    ;
    double          BigT        ;
    double          rate        ;
    float           distCut     ;
    char            inpSpec     ;
    char            outSpec     ;
    bool            gList       ;
    bool            partOut     ;
    int             seed        ;

    vector<int>         partialsToIgnore    ;
    vector<double>      effCurveParams  ;
    vector<double>      qFactors        ;   

    map<int, double>    threshP0        ;
    map<int, double>    threshP1        ;
    map<int, double>    expTails        ;
    map<int, double>    resP0, resP1    ;
    map<int, bool>      excChannels     ;
    map<int, int>       chan2det        ;
    map<int, double>    pX, pY, pZ      ;

    string expTailFile ;
    string exclChFile  ;
    string detMapFile  ;
    string threshFile  ; 
    string quenchFile  ;
    string effCurve    ;
    string combFile    ;
    string inpFile     ;
    string outFile     ;
    string resFile     ;
    string posFile     ;
    string output      ; 
    string pileUp      ;
    string input       ;
    string listOfPartialsToIgnore;

    bool verbose;
};
typedef struct OPT OPTIONS;

// Info
struct IN
{
    string                  inCommandLine   ;
    string                  outCommandLine  ;
    unsigned long long int  nTotMC          ;
    unsigned long long int  nChainMC        ;
    unsigned long long int  nTot            ;
    unsigned long long int  nChainResets    ;
    unsigned long long int  lastReadNChain  ;
};
typedef IN INFO;

// Cut events
struct CUT
{
    int integrated  ;
    int exclChan    ;
    int threshold   ;
    int pileUp      ;
    int pulserPU    ;
    int deadT       ;
    int effCurve    ;
};
typedef struct CUT CUTS;

// ================================================================
// ================================================================
// ================================================================

// Initialize all options

void initialize(OPTIONS& opt, CUTS& cut, INFO& info)
{
    opt.partialsToIgnore.clear();

    opt.maxChannels = MAXCH ;
    opt.skipEvents  = 0     ;
    opt.firstEvent  = 0     ;
    opt.lastEvent   = 0     ;
    opt.BigT        = 3600. ;
    opt.coincTime   = 0     ;
    opt.integTime   = 0.005 ;
    opt.deadTime    = 0     ;
    opt.inpSpec     = 'n'   ;
    opt.inpFile     = ""    ;
    opt.outSpec     = 'n'   ;
    opt.outFile     = ""    ;
    opt.numEvents   = 0     ;
    opt.rate        = 0.    ;
    opt.input       = ""    ;
    opt.output      = ""    ;
    opt.exclChFile  = ""    ;
    opt.expTailFile = ""    ;
    opt.detMapFile  = ""    ;
    opt.resFile     = ""    ;
    opt.threshFile  = ""    ;
    opt.quenchFile  = ""    ;
    opt.posFile     = ""    ;
    opt.pileUp      = ""    ;
    opt.combFile    = ""    ;
    opt.verbose     = false ;
    opt.partOut     = false ;
    opt.gList       = false ;
    opt.numFiles    = 0     ;
    opt.pulserDt    = 0     ;
    opt.distCut     = 0.    ;
    opt.startTime   = -1.   ;
    opt.listOfPartialsToIgnore = "";

    cut.integrated  = 0     ;
    cut.exclChan    = 0     ;
    cut.threshold   = 0     ;
    cut.pileUp      = 0     ;
    cut.deadT       = 0     ;
    cut.pulserPU    = 0     ;
    cut.effCurve    = 0     ;

    info.inCommandLine  = "";
    info.outCommandLine = "";
    info.nTotMC         = 0 ;
    info.nChainMC       = 0 ;
    info.nTot           = 0 ; 
    info.nChainResets   = 0 ;
    info.lastReadNChain = 0 ;
}



bool eventSort(GEvent* e1, GEvent* e2)
{
    return ( e1->GetAbsTime() < e2->GetAbsTime() );
}

void ThrowError(string message)
{
    cout << "[*************************]" << endl;
    cout << "[          Error          ]" << "\t" << message << endl;
    cout << "[*************************]" << endl;
    cout << endl;

    exit(1);
}

int main(int argc, char** argv)
{

  vector<GEvent*>    DATA    ;
  short int       maxChannel = 0  ;


  OPTIONS opt ;
  CUTS    cut ;
  INFO    info;
  initialize(opt, cut, info);
  
  string err;

  // get input parameters
  int c;

      while( (c = getopt(argc, argv, ARGS)) != -1 )
    {
        switch(c)
        {
	case 'i': opt.input = string(optarg); break;      // Input file
        }
    }

      cout << opt.input << endl;

      (opt.qFactors).resize(MAXID, 1);

   
   // Input file
    if(opt.input == "")
      ThrowError("Input file not specified!");

    
    TChain* inChain  = new TChain("qtree");
    TFile*  tmpTFile = new TFile();
    TTree*  infoTree = new TTree();
    //TTree*  tmpTree  = new TTree();
    
    // Info Tree variables
    unsigned long long int nTot, nChain;
    string * cLine = new string();
    
    // MC Tree variables
    short int       channel             ;
    ULong64_t       nchain              ;
    double          time, energy        ;
    double          cuenergy            ;
    double          posX, posY, posZ    ;
    double          dirX, dirY          ;
    double          penergy[MAXID]      ; 
    double          energyq = 0         ;
    
    double          gbFraction          ;
    double          aFraction           ;
    double          rFraction           ;
    string *        pName = new string();
    string *        dName = new string();
    //
    // -5 option
    int         numGamma;
    double      gammaList[50];

   unsigned long long int totalEvents;

    double lastIterTime = 0;

    if(inChain->Add(opt.input.c_str()) == 0)
      {
	err = "Input file " + opt.input + " does not contain a valid ROOT TTree named qtree";
	ThrowError(err);
      }
    
    // INFOtree
    
    tmpTFile = TFile::Open(opt.input.c_str());
    if( (infoTree = (TTree*) tmpTFile->Get("INFOtree")) == NULL )
      {
	err = "Input file " + opt.input + " does not contain a valid ROOT TTree named 'INFOtree'";
	ThrowError(err);
      }
    
    infoTree->SetBranchAddress("CommandLine", &cLine );
    infoTree->SetBranchAddress("Ntot"       , &nTot  );
    infoTree->SetBranchAddress("Nchain"     , &nChain);
    
    short int infoEvents = infoTree->GetEntries();
    
    // Variables for output infoTree
    
    for(int i = 0; i < infoEvents; i++)
      {
	infoTree->GetEntry(i);
	
	info.inCommandLine  = *cLine ;
	info.nTotMC        +=  nTot  ;
	info.nChainMC      +=  nChain;
	
		if(nTot == 0)
		  info.nChainResets++;
      }
    delete infoTree;
    tmpTFile->Close();

    inChain->SetBranchAddress("Channel"                  , &channel  );
    inChain->SetBranchAddress("ChainNumber"              , &nchain   );
    inChain->SetBranchAddress("Time"                     , &time     );
    inChain->SetBranchAddress("DepositedEnergy"          , &energy   );
    inChain->SetBranchAddress("DepositedEnergyByParticle", &penergy  );
    inChain->SetBranchAddress("EnergyDepositedInCopper"  , &cuenergy );
    inChain->SetBranchAddress("ParticlePositionX"        , &posX     );
    inChain->SetBranchAddress("ParticlePositionY"        , &posY     );
    inChain->SetBranchAddress("ParticlePositionZ"        , &posZ     );
    inChain->SetBranchAddress("ParticleDirectionX"       , &dirX     );
    inChain->SetBranchAddress("ParticleDirectionY"       , &dirY     );
    inChain->SetBranchAddress("ParticleName"             , &pName    );
    inChain->SetBranchAddress("DaughterName"             , &dName    );

    if(opt.gList)
      {
	if(inChain->GetListOfBranches()->FindObject("GammaList"))
	  {
	    inChain->SetBranchAddress("NGamma"   , &numGamma );
	    inChain->SetBranchAddress("GammaList", &gammaList);
	  }
	else
	  {
	    cout << endl;
	    ThrowError("-5 option specified, but input MC file does not contain the deexcitation gamma list");
	  }
      }


    GEvent* ev;
    
    unsigned long long int evPerStep = 2e9 / sizeof(*ev);
    cout << "Reading in " << evPerStep << " events per step" << endl;
    
    totalEvents = inChain->GetEntries();
    
    bool looping = true;
    int loop_number = 0;
    int loop_total = ceil(float(totalEvents - opt.skipEvents) / evPerStep);
    cout << "Total events to read: " << totalEvents - opt.skipEvents << endl; 


    while (looping)
      {

	
	loop_number++;
	
	// Set the first event to where we ended in last loop
	if(opt.lastEvent != 0) // ignores first loop
	  opt.firstEvent += evPerStep;
	opt.lastEvent += evPerStep;
	
	// Skip events
	if(opt.firstEvent == 0)
	  opt.firstEvent = opt.skipEvents;
	
	while(opt.lastEvent < opt.skipEvents)
	  opt.lastEvent  += evPerStep;
		

	unsigned long long int delta = opt.lastEvent - opt.firstEvent;
	// Number of events - n option
	if(opt.numEvents != 0)
	  {
	    opt.numEvents -= delta;
	    if(opt.numEvents <= 0)
	      {
		    opt.lastEvent = opt.firstEvent + opt.numEvents + delta;
		    looping = false;
		  }
	      }
                
	// Out of bounds
	if(opt.lastEvent > totalEvents)
	  {
	    opt.lastEvent = totalEvents;
	    looping = false;
	  }
	string echo;
	stringstream ss;
	ss << " Loop " << loop_number << " of " << loop_total;
	echo += ss.str();
	
	cout << "[" << left << setw(BWIDTH) << echo << "]\r" << flush <<  endl;
	cout << "Event Start: " << opt.firstEvent << " | Event End: " << opt.lastEvent <<  endl;
	
	cout << "[" << left << setw(BWIDTH) << " Reading data..." << "]\r" << flush;

	DATA.reserve(delta);
	
	unsigned long long int step = delta / 20;


            for(ULong64_t e = opt.firstEvent; e < opt.lastEvent; e++)
            {
	      if(step > 0 && e % step == 0)
		cout << "[" << left << setw(BWIDTH) << " Reading data..." << "]\t" << setw(3) << 5 * (int)((e-opt.firstEvent)/step) << "%\r" << flush;
	      inChain->GetEntry();

	      
	      gbFraction = (penergy[0] + penergy[1] + penergy[2]) / energy;
	      aFraction  = penergy[7] / energy;
	      rFraction  = penergy[8] / energy;
	      
	      energy *= 1000.;
	      
	      
	      for(int j = 0; j < MAXID; j++)
		energyq += penergy[j] * 1000. * opt.qFactors[j];
	      

	      DATA.push_back(new GEvent(channel, nchain, time, energy, energyq, gbFraction, aFraction, rFraction));

	      if(channel > maxChannel)
		maxChannel = channel;

	    }
	    
            info.lastReadNChain = nchain;
	    // RAM memory usage at this point
    
	    char memFile[100];
	    int PID = getpid();
	    sprintf(memFile, "/proc/%d/statm", PID);
	
	    ifstream memS(memFile);
	    int mem0, mem1;
	    string line;
	    while(getline(memS, line))
	      {
		stringstream SS(line);
		SS >> mem0 >> mem1;
	      }

	    cout << "[" << left << setw(BWIDTH) << " Memory usage" << "]\t" << mem1 * 4./1024. << " Mb" << endl;

	    
	    for (vector<GEvent*>::iterator it = DATA.begin(); it != DATA.end(); ++it)
	      delete (*it);
	    DATA.clear();


            cout << "[" << left << setw(BWIDTH) << " Reading data..." << "]\t100%" << endl;

      }


    opt.maxChannels = maxChannel + 1;

    cout << "[" << left << setw(BWIDTH) << " Entries read" << "]\t" << totalEvents << endl;
    
    // RAM memory usage at this point
    
        char memFile[100];
        int PID = getpid();
        sprintf(memFile, "/proc/%d/statm", PID);
	
        ifstream memS(memFile);
        int mem0, mem1;
	string line;
        while(getline(memS, line))
	  {
            stringstream SS(line);
            SS >> mem0 >> mem1;
	  }

        cout << "[" << left << setw(BWIDTH) << " Memory usage" << "]\t" << mem1 * 4./1024. << " Mb" << endl;
	
}


