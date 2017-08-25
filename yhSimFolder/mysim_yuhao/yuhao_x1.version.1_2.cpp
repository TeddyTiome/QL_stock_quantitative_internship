#include "include/qlSim.h"
namespace
{
class x1: public AlphaBase
{
    public:
    explicit x1(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        vol(dr.getData<QL_MATRIX<QL_FLOAT>>("volume")),
        vwap(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_vwap")),
        low(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_low")),
        indus(dr.getData<QL_MATRIX<QL_INT>>("industry")),
        Num1(cfg->getAttributeIntDefault("para1",20)),
	Num2(cfg->getAttributeIntDefault("para2",1)),
    Num3(cfg->getAttributeIntDefault("para3",11)),
    Num4(cfg->getAttributeIntDefault("para4",20))

    {
    	adv10.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            adv10[di].resize(GLOBAL::Instruments.size(),nan);
        }

    	hl_ind.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            hl_ind[di].resize(GLOBAL::Instruments.size(),nan);
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
                adv10[di-delay][ii] = QL_Oputils::mean(c1);
            }
        }

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
            	hl_ind[di-delay][ii] = low[di-delay][ii]; 
            }
        }

        // for (int i = di-delay-Num4-Num3; i< di; ++i)
        // {       
        // 	QL_Oputils::industryNeutralize(close_ind[di-delay-i],indus[di-delay-i]);
        // }
     

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
		
                vector<float> c6(Num3,nan);
                vector<float> c7(Num4,nan);
				vector<float> c8(Num3,nan);
                for (int b = 0; b< Num4 ;++b){
				for (int c=0; c<Num3; ++c){
				c6[c]= hl_ind[di-delay-c-b][ii];	
				c8[c]=adv10[di-delay-c-b][ii];
			}
                c7[b] = QL_Oputils::corr(c6,c8);
            }
            QL_Oputils::rank(c7);
				
            rank2[ii] = c7[0];


			}
		}
        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
 				alpha[ii]= -1 * powf(float(rank1[ii]),float(rank2[ii]));	    		
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
            ar & adv10;
            ar & hl_ind;
        }
	const QL_MATRIX<QL_FLOAT> &vol;
    const QL_MATRIX<QL_FLOAT> &vwap;
    const QL_MATRIX<QL_FLOAT> &low;
	const QL_MATRIX<QL_INT> &indus;
    vector<vector<float> > adv10;
    vector<vector<float> > hl_ind;

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
        AlphaBase * str = new x1(cfg);
        return str;
    }
}

