#include "include/qlSim.h"
namespace
{
class alpha64: public AlphaBase
{
    public:
    explicit alpha64(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        vwap(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_vwap")), 
        vol(dr.getData<QL_MATRIX<QL_FLOAT>>("volume")), 
        high(dr.getData<QL_MATRIX<QL_FLOAT>>("high")), 
        low(dr.getData<QL_MATRIX<QL_FLOAT>>("low")),
        open(dr.getData<QL_MATRIX<QL_FLOAT>>("open")),
        Num1(cfg->getAttributeIntDefault("para1",12)),
	Num2(cfg->getAttributeIntDefault("para2",16)),
	Num3(cfg->getAttributeIntDefault("para3",120)),
	Num4(cfg->getAttributeIntDefault("para4",3))
	
    {
        adv120.resize (GLOBAL::Dates.size());

        for (int di = 0; di< GLOBAL::Dates.size(); ++di)
        {
            adv120[di].resize(GLOBAL::Instruments.size(),nan);
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
                adv120[di-delay][ii] = QL_Oputils::mean(c1);
            }
        }

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                vector<float> c1(Num1,nan);
                vector<float> c2(Num2,nan);
                vector<float> c5(Num2,nan);
                vector<float> c4(Num1,nan);

                for (int j = 0; j< Num2; ++j)
        {
        for (int i = 0; i< Num1; ++i)
        {
          c1[i] = open[di-delay-i-j][ii] * 0.178404 + low[di-delay-i-j][ii] * (1.0-0.178404);
          c4[i] = adv120[di-delay-i][ii];
        }
        c2[j] = QL_Oputils::sum(c1);
        c5[j] = QL_Oputils::sum(c4);
        }
            rank1[ii] = QL_Oputils::corr(c2,c5);
            }
        }
       
        QL_Oputils::rank(rank1);

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                rank2[ii] = ((high[di-delay][ii]+low[di-delay][ii])/2.0*0.178404+(vwap[di-delay][ii])*(1.0-0.178404)) - ((high[di-delay-Num4][ii]+low[di-delay-Num4][ii])/2.0*0.178404+(vwap[di-delay-Num4][ii])*(1.0-0.178404));
            }
        }

        QL_Oputils::rank(rank2);

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
		
	           if (rank1[ii]<rank2[ii])
	               {alpha[ii]=-1;}
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
	const QL_MATRIX<QL_FLOAT> &high;
	const QL_MATRIX<QL_FLOAT> &low;
	const QL_MATRIX<QL_FLOAT> &open;
        vector<vector<float> > adv120;
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
        AlphaBase * str = new alpha64(cfg);
        return str;
    }
}

