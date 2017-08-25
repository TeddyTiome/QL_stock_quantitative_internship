#include "include/qlSim.h"
namespace
{
class alpha61: public AlphaBase
{
    public:
    explicit alpha61(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        vwap(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_vwap")), 
        vol(dr.getData<QL_MATRIX<QL_FLOAT>>("volume")),
        Num1(cfg->getAttributeIntDefault("para1",16)),
	Num2(cfg->getAttributeIntDefault("para2",17)),
	Num3(cfg->getAttributeIntDefault("para3",180))
	
    {
        adv180.resize (GLOBAL::Dates.size());

        for (int di = 0; di< GLOBAL::Dates.size(); ++di)
        {
            adv180[di].resize(GLOBAL::Instruments.size(),nan);
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

                vector<float> c1(Num3,nan);
                for (int j = 0; j < Num3; ++ j)
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
             vector<float> c1(Num1,nan);  
                for (int i = 0; i< Num1; ++i)
             {
                    c1[i] = vwap[di-delay-i][ii];
             }
             rank1[ii] = vwap[di-delay][ii] - QL_Oputils::llv(c1.begin(),c1.end());
            }
        }
        QL_Oputils::rank(rank1);

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                vector<float> c2(Num2,nan);
                vector<float> c4(Num2,nan);
        
        
        for (int i = 0; i< Num2; ++i)
        {
          c2[i] = vwap[di-delay-i][ii];
          c4[i] = adv180[di-delay-i][ii];
         
        } 
        rank2[ii] = QL_Oputils::corr(c2,c4);
            }
        }
        QL_Oputils::rank(rank2);

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {

	           if (rank1[ii]<rank2[ii])
	           {alpha[ii]=1;}
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
        vector<vector<float> > adv180;
        int Num1; 
	int Num2;
	int Num3;
};
}
extern "C"
{
    AlphaBase * createStrategy(XMLCONFIG::Element *cfg)
    {
        AlphaBase * str = new alpha61(cfg);
        return str;
    }
}

