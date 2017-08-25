#include "include/qlSim.h"
namespace
{
class alpha87: public AlphaBase
{
    public:
    explicit alpha87(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        vwap(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_vwap")),
        vol(dr.getData<QL_MATRIX<QL_FLOAT>>("volume")), 
        close(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_close")),
        indus(dr.getData<QL_MATRIX<QL_INT>>("industry")),
        Num1(cfg->getAttributeIntDefault("para1",1)),
	Num2(cfg->getAttributeIntDefault("para2",81)),
	Num3(cfg->getAttributeIntDefault("para3",2)),
    Num4(cfg->getAttributeIntDefault("para4",13)),
    Num5(cfg->getAttributeIntDefault("para5",4)),
    Num6(cfg->getAttributeIntDefault("para6",14))
    {

        adv81.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            adv81[di].resize(GLOBAL::Instruments.size(),nan);
        }        

        adv_ind.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            adv_ind[di].resize(GLOBAL::Instruments.size(),nan);
        }
  
        cv.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            cv[di].resize(GLOBAL::Instruments.size(),nan);
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
                vector<float> c1(Num2,nan);

                for (int j = 0; j < Num2; ++ j)
                {
                    c1[j] = vol[di - delay - j][ii];
                }
                adv81[di-delay][ii] = QL_Oputils::mean(c1);
            }
        }

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                cv[di-delay][ii] = close[di-delay][ii] * 0.369701 + vwap[di-delay][ii] * (1-0.369701); 
                adv_ind[di-delay][ii] = adv81[di-delay][ii];
            }
        }
       
       for(int ii = di - delay - Num4 - Num5 - Num6; ii < di; ++ ii)
        {
             QL_Oputils::industryNeutralize(adv_ind[di-delay-ii],indus[di-delay-ii]);
                 
         }


       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                   rank1[ii] = 0;
                for (int a =0; a<Num3; ++a){
                   rank1[ii] +=  float(cv[di-delay-a][ii] -cv[di-delay-a-Num1][ii]) * (Num3 - a) / ((Num3+1) * Num3 / 2.0);
                }
                }
        }

        QL_Oputils::rank(rank1);

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                vector<float> c1(Num4,nan);
                vector<float> c2(Num4,nan);

                            for (int d=0;d<Num4; ++d){
                                    c1[d] = adv_ind[di-delay-d][ii];
                                    c2[d] = close[di-delay-d][ii];
                                }
                                cor[di-delay][ii] = QL_Oputils::corr(c1,c2);

                            if (cor[di-delay][ii] < 0 )
                                {cor[di-delay][ii] = - cor[di-delay][ii];}
                            }
                        }

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                    dec[di-delay][ii] = 0;

                    for (int b=0;b<Num5; ++b){
                            dec[di-delay][ii] += float(cor[di-delay-b][ii]) * (Num5 - b) / ((Num5+1) * Num5 / 2.0);
                        }
                    }
                }


        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                vector<float> c4(Num6,nan);

                for (int a=0;a<Num6; ++a){
                  
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
					alpha[ii] = -1* rank2[ii];
                }
                else {alpha[ii] = -1 * rank1[ii];}
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
    const QL_MATRIX<QL_FLOAT> &close;
    const QL_MATRIX<QL_INT> &indus;
    vector<vector<float> > adv81;
    vector<vector<float> > adv_ind;
    vector<vector<float> > cv;
    vector<vector<float> > cor;
    vector<vector<float> > dec;
        int Num1; 
	int Num2;
	int Num3;
    int Num4;
    int Num5;
    int Num6;

};
}
extern "C"
{
    AlphaBase * createStrategy(XMLCONFIG::Element *cfg)
    {
        AlphaBase * str = new alpha87(cfg);
        return str;
    }
}

