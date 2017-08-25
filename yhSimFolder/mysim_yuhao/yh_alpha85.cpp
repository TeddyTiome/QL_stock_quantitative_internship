#include "include/qlSim.h"
namespace
{
class alpha85: public AlphaBase
{
    public:
    explicit alpha85(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        vol(dr.getData<QL_MATRIX<QL_FLOAT>>("volume")),
        high(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_high")),
        low(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_low")),
        close(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_close")),
        Num1(cfg->getAttributeIntDefault("para1",30)),
	Num2(cfg->getAttributeIntDefault("para2",9)),
	Num3(cfg->getAttributeIntDefault("para3",3)),
	Num4(cfg->getAttributeIntDefault("para4",10)),
	Num5(cfg->getAttributeIntDefault("para5",7))
    {
    	adv30.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            adv30[di].resize(GLOBAL::Instruments.size(),nan);
        }
    }
    void generate(int di) override
    {

			vector<float> rank1(GLOBAL::Instruments.size(),nan);
            vector<float> rank2(GLOBAL::Instruments.size(),nan);

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {

                vector<float> c1(Num1,nan);

                for (int j = 0; j < Num1; ++ j)
                {
                    c1[j] = vol[di - delay - j][ii];
                }
                adv30[di-delay][ii] = QL_Oputils::mean(c1);
            }
        }

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
	            vector<float> c1(Num2,nan);
	            vector<float> c2(Num2,nan);

				for (int a=0; a<Num2;++a){
					c1[a]=0.876703*high[di-delay-a][ii]+close[di-delay-a][ii]*(1-0.876703);
					c2[a]=adv30[di-delay-a][ii];
			}
			
			rank1[ii] = QL_Oputils::corr(c1,c2);
            }
    	}

    	QL_Oputils::rank(rank1);

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
		
				vector<float> c5(Num3,nan);
				vector<float> c6(Num5,nan);
				vector<float> c7(Num4,nan);
				vector<float> c8(Num5,nan);

			for (int c=0; c<Num5; ++c){
				for (int d=0; d<Num3; ++d){
					c5[d]=0.5*(high[di-delay-c-d][ii]+low[di-delay-c-d][ii]);
				}
				QL_Oputils::rank(c5);
				c6[c]=c5[0];
				for (int e=0; e<Num4;++e){
					c7[e]= vol[di-delay-e-c][ii];
				}
				QL_Oputils::rank(c7);
				c8[c]=c7[0];
			}
			rank2[ii] = QL_Oputils::corr(c6,c8);

			}
		}

		QL_Oputils::rank(rank2);

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
 				alpha[ii]= powf(float(rank1[ii]),float(rank2[ii]));	    		
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
	const QL_MATRIX<QL_FLOAT> &high;
	const QL_MATRIX<QL_FLOAT> &low;
	const QL_MATRIX<QL_FLOAT> &close;
    vector<vector<float> > adv30;

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
        AlphaBase * str = new alpha85(cfg);
        return str;
    }
}

