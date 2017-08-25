#include "include/qlSim.h"
namespace
{
class alpha89: public AlphaBase
{
    public:
    explicit alpha89(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        vwap(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_vwap")),
        vol(dr.getData<QL_MATRIX<QL_FLOAT>>("volume")), 
        low(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_low")),
        indus(dr.getData<QL_MATRIX<QL_INT>>("industry")),
        Num1(cfg->getAttributeIntDefault("para1",6)),
	Num2(cfg->getAttributeIntDefault("para2",10)),
	Num3(cfg->getAttributeIntDefault("para3",5)),
    Num4(cfg->getAttributeIntDefault("para4",3)),
    Num5(cfg->getAttributeIntDefault("para5",15))
    {

        adv10.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            adv10[di].resize(GLOBAL::Instruments.size(),nan);
        }        

        vwap_ind.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            vwap_ind[di].resize(GLOBAL::Instruments.size(),nan);
        }
  
        dec.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            dec[di].resize(GLOBAL::Instruments.size(),nan);
        }

        cor.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            cor[di].resize(GLOBAL::Instruments.size(),nan);
        }

        dec2.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            dec2[di].resize(GLOBAL::Instruments.size(),nan);
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
                vector<float> c1(Num2,nan);

                for (int j = 0; j < Num2; ++ j)
                {
                    c1[j] = vol[di - delay - j][ii];
                }
                adv10[di-delay][ii] = QL_Oputils::mean(c1);
            }
        }

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                vwap_ind[di-delay][ii] = vwap[di-delay][ii];
            }
        }
       
       for(int ii = di - delay - Num4 - Num5 - Num2; ii < di; ++ ii)
        {
             QL_Oputils::industryNeutralize(vwap_ind[di-delay-ii],indus[di-delay-ii]);
                 
         }


        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                vector<float> c1(Num1,nan);
                vector<float> c2(Num1,nan);

                            for (int d=0;d<Num4; ++d){
                                    c1[d] = low[di-delay-d][ii];
                                    c2[d] = adv10[di-delay-d][ii];
                                }
                                cor[di-delay][ii] = QL_Oputils::corr(c1,c2);

                            }
                        }

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                    dec[di-delay][ii] = 0;

                    for (int b=0;b<Num3; ++b){
                            dec[di-delay][ii] += float(cor[di-delay-b][ii]) * (Num3 - b) / ((Num3+1) * Num3 / 2.0);
                        }
                    }
                }
        
        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                    dec2[di-delay][ii] = 0;

                    for (int b=0;b<Num2; ++b){
                            dec2[di-delay][ii] += float(vwap_ind[di-delay-b][ii]-vwap_ind[di-delay-b - Num4][ii]) * (Num2 - b) / ((Num2+1) * Num2 / 2.0);
                        }
                    }
                }

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                vector<float> c4(Num4,nan);

                for (int a=0;a<Num4; ++a){
                  
                            c4[a] = dec[di-delay-a][ii];
                        }

                QL_Oputils::rank(c4);
                
                rank1[ii] = float(c4[0]);
                
        }
        }

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                vector<float> c4(Num5,nan);

                for (int a=0;a<Num5; ++a){
                  
                            c4[a] = dec2[di-delay-a][ii];
                        }

                QL_Oputils::rank(c4);
                
                rank2[ii] = float(c4[0]);
                
        }
        }


        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
					alpha[ii] = rank1[ii] - rank2[ii];
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
	const QL_MATRIX<QL_FLOAT> &vwap;
    const QL_MATRIX<QL_FLOAT> &vol;
    const QL_MATRIX<QL_FLOAT> &low;
    const QL_MATRIX<QL_INT> &indus;
    vector<vector<float> > adv10;
    vector<vector<float> > vwap_ind;
    vector<vector<float> > dec;
    vector<vector<float> > cor;
    vector<vector<float> > dec2;
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
        AlphaBase * str = new alpha89(cfg);
        return str;
    }
}

