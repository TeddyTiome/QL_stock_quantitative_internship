#include "include/qlSim.h"
namespace
{
class alpha78: public AlphaBase
{
    public:
    explicit alpha78(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        vwap(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_vwap")),
        vol(dr.getData<QL_MATRIX<QL_FLOAT>>("volume")), 
        low(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_low")),
        Num1(cfg->getAttributeIntDefault("para1",19)),
	Num2(cfg->getAttributeIntDefault("para2",40)),
	Num3(cfg->getAttributeIntDefault("para3",6)),
	Num4(cfg->getAttributeIntDefault("para4",5))
    {

        adv40.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            adv40[di].resize(GLOBAL::Instruments.size(),nan);
        }

        vwap_rank.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            vwap_rank[di].resize(GLOBAL::Instruments.size(),nan);
        }

        vol_rank.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            vol_rank[di].resize(GLOBAL::Instruments.size(),nan);
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
                adv40[di-delay][ii] = QL_Oputils::mean(c1);
            }
        }

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                vwap_rank[di-delay][ii] = vwap[di-delay][ii];
                vol_rank[di-delay][ii] =vol[di-delay][ii];
            }
        }

            QL_Oputils::rank(vwap_rank[di-delay]);
            QL_Oputils::rank(vol_rank[di-delay]);

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
            	vector<float> c1(Num1,nan);
				vector<float> c3(Num1,nan);
				vector<float> c4(Num3,nan);
				vector<float> c5(Num3,nan);

				for (int m=0; m<Num3;++m){
				for (int i =0; i<Num1;++i){
					c1[i]=low[di-delay-i-m][ii]*0.352233+vwap[di-delay-i-m][ii]*(1-0.352233);
				}
					c4[m]=QL_Oputils::sum(c1);
				for (int i =0; i<Num1;++i){
					c3[i]=adv40[di-delay-i][ii];
				}
					c5[m]=QL_Oputils::sum(c3);
				}	
				rank1[ii] = QL_Oputils::corr(c4,c5);

			}
		}

		QL_Oputils::rank(rank1);

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                
				vector<float> c6(Num4,nan);
				vector<float> c7(Num4,nan);

				for (int i=0; i<Num4; ++i){
					//QL_Oputils::rank(vwap_rank[di-delay-i]);
					//QL_Oputils::rank(vol_rank[di-delay-i]);

					c6[i]=vwap_rank[di-delay-i][ii];
					c7[i]=vol_rank[di-delay-i][ii];
				}

				rank2[ii] = QL_Oputils::corr(c6,c7);

	    }
        }
        QL_Oputils::rank(rank2);

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
        		if (!(GLOBALFUNC::equal(float(rank2[ii]),0)))
					{alpha[ii]=powf(float(rank1[ii]),float(rank2[ii]));}
				else
					{alpha[ii]=0;}
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
    vector<vector<float> > adv40;
    vector<vector<float> > vwap_rank;
    vector<vector<float> > vol_rank;
        int Num1; 
	int Num2;
	int Num3;
	int Num4;

};
}
extern "C"
{
    AlphaBase * createStrategy(XMLCONFIG::Element *cfg)
    {
        AlphaBase * str = new alpha78(cfg);
        return str;
    }
}

