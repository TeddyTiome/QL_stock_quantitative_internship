#include "include/qlSim.h"
namespace
{
class alpha82: public AlphaBase
{
    public:
    explicit alpha82(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        vol(dr.getData<QL_MATRIX<QL_FLOAT>>("volume")), 
        open(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_open")),
        sector(dr.getData<QL_MATRIX<QL_INT>>("sector")),
        Num1(cfg->getAttributeIntDefault("para1",1)),
	Num2(cfg->getAttributeIntDefault("para2",14)),
	Num3(cfg->getAttributeIntDefault("para3",17)),
    Num4(cfg->getAttributeIntDefault("para4",6)),
    Num5(cfg->getAttributeIntDefault("para5",13))
    {

  
        vol_ind.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            vol_ind[di].resize(GLOBAL::Instruments.size(),nan);
        }

        cor.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            cor[di].resize(GLOBAL::Instruments.size(),nan);
        }
      

        dec.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            dec[di].resize(GLOBAL::Instruments.size(),nan);
        }
  


    }
    void generate(int di) override
    {

			vector<float> rank1(GLOBAL::Instruments.size(), nan);
			vector<float> rank2(GLOBAL::Instruments.size(), nan);
	

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                vol_ind[di-delay][ii] = vol[di-delay][ii]; 
            }
        }
       
       for(int ii = di - delay - Num4 - Num5 - Num3; ii < di; ++ ii)
        {
             QL_Oputils::industryNeutralize(vol_ind[di-delay-ii],sector[di-delay-ii]);
                 
         }


       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                   rank1[ii] = 0;
                for (int a =0; a<Num2; ++a){
                   rank1[ii] +=  float(open[di-delay-a][ii] -open[di-delay-a-Num1][ii]) * (Num2 - a) / ((Num2+1) * Num2 / 2.0);
                }
                }
        }

        QL_Oputils::rank(rank1);

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                vector<float> c1(Num3,nan);
                vector<float> c2(Num3,nan);

                            for (int d=0;d<Num3; ++d){
                                    c1[d] = vol_ind[di-delay-d][ii];
                                    c2[d] = open[di-delay-d][ii];
                                }
                                cor[di-delay][ii] = QL_Oputils::corr(c1,c2);
                            }
                        }

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                    dec[di-delay][ii] = 0;

                    for (int b=0;b<Num4; ++b){
                            dec[di-delay][ii] += float(cor[di-delay-b][ii]) * (Num4 - b) / ((Num4+1) * Num4 / 2.0);
                        }
                    }
                }


        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                vector<float> c4(Num5,nan);

                for (int a=0;a<Num5; ++a){
                  
                            c4[a] = dec[di-delay-a][ii];
                        }

                QL_Oputils::rank(c4);
                
                rank2[ii] = float(c4[0]);
                

	    }
        }


        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
        		if( rank1[ii] < rank2[ii]){
					alpha[ii] = -1* rank1[ii];
                }
                else {alpha[ii] = -1 * rank2[ii];}
				}
		}

        return;
		
    }
    
    
    void checkPointSave(boost::archive::binary_oarchive &ar) 
    {
        ar & *this;
    }
    void checkPointLoad(boost::archive::binary_iarchive &ar)
    {
        ar & *this;
    }
    std::string version() const
    {
        return GLOBALCONST::VERSION;
    }
    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int/*file_version*/)
        {
        }
    const QL_MATRIX<QL_FLOAT> &vol;
    const QL_MATRIX<QL_FLOAT> &open;
    const QL_MATRIX<QL_INT> &sector;
    vector<vector<float> > vol_ind;
    vector<vector<float> > cor;
    vector<vector<float> > dec;
        int Num1; 
	int Num2;
	int Num3;
    int Num4;
    int Num5;

};
}
extern "C"
{
    AlphaBase * createStrategy(XMLCONFIG::Element *cfg)
    {
        AlphaBase * str = new alpha82(cfg);
        return str;
    }
}

