#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include "GEvent.h"

GEvent::GEvent() {;}

GEvent::GEvent(int channel_, long long int nchain_, double time_, double energy_, double energyQ_, double gbFraction_, double aFraction_, double rFraction_, int numGamma_, double* gammaList_)
{
    channel     = channel_   ;
    nchain      = nchain_    ;
    absTime     = 0          ;
    relTime     = time_      ;
    energy      = energy_    ;
    energy_Q    = energyQ_   ;
    totEnergy   = energy_    ;
    totEnergy_Q = energyQ_   ;
    detector    = channel_   ;
    gbFraction  = gbFraction_;
    aFraction   = aFraction_ ;
    rFraction   = rFraction_ ;

    for(int i = 0; i < numGamma_; i++)
        gammaList.push_back(gammaList_[i] * 1000.);

    absTime       = 0;
    usable        = 1;
    pileup        = 1;
    multi         = 1;
    multiInd      = 1;
    distance      = 0;
    accidental = true;
    numIntegrated = 1;
}

void GEvent::SetAbsTime     (double       time_     ) { absTime     = time_    ;}
void GEvent::SetRelTime     (double       time_     ) { relTime     = time_    ;}
void GEvent::SetEnergy      (double       energy_   ) { energy      = energy_  ;}
void GEvent::SetEnergy_Q    (double       energy_   ) { energy_Q    = energy_  ;}
void GEvent::SetTotEnergy   (double       energy_   ) { totEnergy   = energy_  ;}
void GEvent::SetTotEnergy_Q (double       energy_   ) { totEnergy_Q = energy_  ;}
void GEvent::SetChannel     (int          channel_  ) { channel     = channel_ ;}
void GEvent::SetDetector    (int          detector_ ) { detector    = detector_;}
void GEvent::SetMulti       (short int    multi_    ) { multi       = multi_   ;}
void GEvent::SetMultiInd    (short int    multiInd_ ) { multiInd    = multiInd_;}
void GEvent::SetDistance    (double       distance_ ) { distance    = distance_;}
void GEvent::Usable         (bool         usable_   ) { usable      = usable_  ;}
void GEvent::PileUp         (bool         pileup_   ) { pileup      = pileup_  ;}
void GEvent::AddIntegrated  (short int    count     ) { numIntegrated += count ;}
void GEvent::SetAccidental  (bool accidental_) {accidental = accidental_;}
void GEvent::SetGBFraction  (double       gbFrac_   ) { gbFraction  = gbFrac_  ;}
void GEvent::SetAFraction   (double       aFrac_    ) { aFraction   = aFrac_   ;}
void GEvent::SetRFraction   (double       rFrac_    ) { rFraction   = rFrac_   ;}
void GEvent::SetMultiplet   (int          chan_     ) { multiplet.push_back(chan_)  ; }
