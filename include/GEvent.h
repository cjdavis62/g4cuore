/*#include <string>
#include <vector>
#include <map>
*/

using namespace std;

class GEvent
{
    public:
        GEvent();

        GEvent( int channel_ , long long int nchain_ , double time_ , double energy_ , double energyQ_ = 0, double gbFraction_ = 0, double aFraction_ = 0, double rFraction_ = 0, int numGamma_ = 0, double* gammaList_ = 0);
        ~GEvent() {;}

        // Get
        int             GetChannel       () const {return channel      ;}
        int             GetDetector      () const {return detector     ;}
        short int       GetNChain        () const {return nchain       ;}
        short int       GetMulti         () const {return multi        ;}
        short int       GetMultiInd      () const {return multiInd     ;}
        double          GetAbsTime       () const {return absTime      ;}
        double          GetRelTime       () const {return relTime      ;}
        double          GetEnergy        () const {return energy       ;}
        double          GetEnergy_Q      () const {return energy_Q     ;}
        double          GetTotEnergy     () const {return totEnergy    ;}
        double          GetTotEnergy_Q   () const {return totEnergy_Q  ;}
        double          GetDistance      () const {return distance     ;}
        double          GetGBFraction    () const {return gbFraction   ;}
        double          GetAFraction     () const {return aFraction    ;}
        double          GetRFraction     () const {return rFraction    ;}
        short int       GetNumIntegrated () const {return numIntegrated;}
        vector<double>  GetGammaList     () const {return gammaList    ;}

        int             GetMultiplet (int ind) { return multiplet.at(ind); }
        int             GetMultiplet() { return multiplet.size(); }
        void            SetMultiplet (int);

        // Set
        void            SetAbsTime      (double     );
        void            SetRelTime      (double     );
        void            SetEnergy       (double     );
        void            SetEnergy_Q     (double     );
        void            SetTotEnergy    (double     );
        void            SetTotEnergy_Q  (double     );
        void            SetChannel      (int        );
        void            SetDetector     (int        );
        void            SetMulti        (short int  );
        void            SetMultiInd     (short int  );
        void            SetDistance     (double     );
        void            AddIntegrated   (short int  );
        void            SetGBFraction   (double     );
        void            SetAFraction    (double     );
        void            SetRFraction    (double     );

        // Flag
        void            Usable      (bool   );
        bool            Usable      (       ) {return usable;}

        void            PileUp      (bool   );
        bool            PileUp      (       ) {return pileup;}

        void            SetAccidental(bool);
        bool            GetAccidental() {return accidental;}

    private:
        int             channel      ;
        int             detector     ;
        long long int   nchain       ;
        short int       multi        ;
        short int       multiInd     ;
        double          absTime      ;
        double          relTime      ;
        double          energy       ;
        double          energy_Q     ;
        double          totEnergy    ;
        double          totEnergy_Q  ;
        double          distance     ;
        double          gbFraction   ;
        double          aFraction    ;
        double          rFraction    ;
        short int       numIntegrated;
        vector<double>  gammaList    ;

        vector<int> multiplet;
    
        bool            usable  ;
        bool            pileup  ;
        bool            accidental;
};
