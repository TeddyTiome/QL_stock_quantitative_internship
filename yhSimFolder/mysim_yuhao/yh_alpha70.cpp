#include "include/qlSim.h"
namespace
{
class alpha70: public AlphaBase
{
    public:
    explicit alpha70(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        vol(dr.getData<QL_MATRIX<QL_FLOAT>>("volume")),
        vwap(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_vwap")),
        close(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_close")),
        indus(dr.getData<QL_MATRIX<QL_INT>>("industry")),
        Num1(cfg->getAttributeIntDefault("para1",50)),
	Num2(cfg->getAttributeIntDefault("para2",1)),
	Num3(cfg->getAttributeIntDefault("para3",17))

    {
    	adv50.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            adv50[di].resize(GLOBAL::Instruments.size(),nan);
        }

    	close_ind.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            close_ind[di].resize(GLOBAL::Instruments.size(),nan);
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
                adv50[di-delay][ii] = QL_Oputils::mean(c1);
            }
        }

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
            	close_ind[di-delay][ii] = close[di-delay][ii]; 
            }
        }

        for (int i = di-delay-Num3-Num3; i< di; ++i)
        {       
        	QL_Oputils::industryNeutralize(close_ind[di-delay-i],indus[di-delay-i]);
        }
     

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
				rank1[ii] = vwap[di-delay][ii] - vwap[di-delay-Num2][ii];
            }
        }

        QL_Oputils::rank(rank1);


        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
		
				vector<float> c5(Num3,nan);
				vector<float> c6(Num3,nan);
				vector<float> c8(Num3,nan);

			for (int b=0; b<Num3;++b){
				for (int c=0; c<Num3; ++c){
				c6[c]= close_ind[di-delay-c-b][ii];	
				c8[c]=adv50[di-delay-c-b][ii];
			}
				c5[b] = QL_Oputils::corr(c6,c8);
			}
			QL_Oputils::rank(c5);

			rank2[ii] = c5[0];
			}
		}

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
 				alpha[ii]= -1 * powf(rank1[ii],rank2[ii]);	    		
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
	const QL_MATRIX<QL_FLOAT> &vwap;
	const QL_MATRIX<QL_FLOAT> &close;
	const QL_MATRIX<QL_INT> &indus;
    vector<vector<float> > adv50;
    vector<vector<float> > close_ind;

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
        AlphaBase * str = new alpha70(cfg);
        return str;
    }
}

