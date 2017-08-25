#include "include/qlSim.h"
namespace
{
class alpha63: public AlphaBase
{
    public:
    explicit alpha63(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        vwap(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_vwap")),
        vol(dr.getData<QL_MATRIX<QL_FLOAT>>("volume")), 
        close(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_close")),
        open(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_open")),
        indus(dr.getData<QL_MATRIX<QL_INT>>("industry")),
        Num1(cfg->getAttributeIntDefault("para1",2)),
	Num2(cfg->getAttributeIntDefault("para2",180)),
	Num3(cfg->getAttributeIntDefault("para3",8)),
    Num4(cfg->getAttributeIntDefault("para4",37)),
    Num5(cfg->getAttributeIntDefault("para5",13)),
    Num6(cfg->getAttributeIntDefault("para6",12))
    {

        adv180.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            adv180[di].resize(GLOBAL::Instruments.size(),nan);
        }
  
        close_ind.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            close_ind[di].resize(GLOBAL::Instruments.size(),nan);
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
                adv180[di-delay][ii] = QL_Oputils::mean(c1);
            }
        }

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                close_ind[di-delay][ii] = close[di-delay][ii]; 
            }
        }
       
       for(int ii = di - delay - Num1 - Num3; ii < di; ++ ii)
        {
             QL_Oputils::industryNeutralize(close_ind[di-delay-ii],indus[di-delay-ii]);
                 
         }


       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                   rank1[ii] = 0;
                   vector<float> c1(Num3,nan);
                for (int a =0; a<Num3; ++a){
                   c1[a] = close_ind[di-delay-a][ii] -close_ind[di-delay-a-Num1][ii];
                    
                }
                for (int a =0; a<Num3; ++a){
                   rank1[ii] +=  float(c1[a]) * (Num3 - a) / ((Num3+1) * Num3 / 2.0);
                }
                }
        }

        QL_Oputils::rank(rank1);



        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                rank2[ii] = 0;
                vector<float> c1(Num6,nan);
                vector<float> c2(Num5,nan);
                vector<float> c3(Num4,nan);
                vector<float> c4(Num5,nan);

                for (int a=0;a<Num6; ++a){
                    for (int b=0;b<Num5; ++b){
                    c2[b] = vwap[di-delay-b-a][ii] * 0.318108 + open[di-delay-b-a][ii] * (1-0.318108);
                

                    for (int j=0; j<Num4; ++j){
                    c3[j]=adv180[di-delay-j-b-a][ii];
                    }
                    c4[b] = QL_Oputils::sum(c3);
                    
                }
                    c1[a] = QL_Oputils::corr(c2,c4);
                    
                }
				
                for (int a=0;a<Num6; ++a){
                rank2[ii] += float(c1[a])* (Num6 - a) / ((Num6 +1) * Num6 / 2.0);
                }

	    }
        }

        QL_Oputils::rank(rank2);

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
        		
					alpha[ii] = -1* float(rank1[ii] - rank2[ii]);
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
    const QL_MATRIX<QL_FLOAT> &open;
    const QL_MATRIX<QL_INT> &indus;
    vector<vector<float> > adv180;
    vector<vector<float> > close_ind;
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
        AlphaBase * str = new alpha63(cfg);
        return str;
    }
}

