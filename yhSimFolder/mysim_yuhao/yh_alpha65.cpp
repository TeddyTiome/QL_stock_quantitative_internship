#include "include/qlSim.h"
namespace
{
class alpha65: public AlphaBase
{
    public:
    explicit alpha65(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        vwap(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_vwap")), 
        vol(dr.getData<QL_MATRIX<QL_FLOAT>>("volume")), 
        high(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_high")), 
        low(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_low")),
        open(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_open")),
        Num1(cfg->getAttributeIntDefault("para1",8)),
	Num2(cfg->getAttributeIntDefault("para2",60)),
	Num3(cfg->getAttributeIntDefault("para3",6)),
	Num4(cfg->getAttributeIntDefault("para4",13))
	
    {
        adv60.resize (GLOBAL::Dates.size());

        for (int di = 0; di< GLOBAL::Dates.size(); ++di)
        {
            adv60[di].resize(GLOBAL::Instruments.size(),nan);
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

                vector<float> c1(Num2,nan);
                for (int j = 0; j < Num2; ++ j)
                {
                    c1[j] = vol[di - delay - j][ii];
                }
                adv60[di-delay][ii] = QL_Oputils::mean(c1);
            }
        }        

    	for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
            	vector<float> c4(Num3,nan);
				vector<float> c3(Num3,nan);

           for (int j = 0; j< Num3; ++j)
	    {
	      c3[j] = open[di-delay-j][ii] * 0.00817205 + vwap[di-delay-j][ii] * (1.0-0.00817205);
	      c4[j] = 0;
			
			for (int i = 0; i< Num1; ++i)
		{
		  c4[j] += adv60[di-delay-i][ii];
		}
		}
			rank1[ii] = QL_Oputils::corr(c3,c4);
            }
    	}

    	QL_Oputils::rank(rank1);

		for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
            	vector<float> c5(Num4,nan);

   				for (int i =0; i< Num4; ++i)
		{
		  c5[i] = open[di-delay-i][ii];
		}
			rank2[ii] = open[di-delay][ii]-QL_Oputils::llv(c5.begin(),c5.end());
		
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
    vector<vector<float> > adv60;
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
        AlphaBase * str = new alpha65(cfg);
        return str;
    }
}

