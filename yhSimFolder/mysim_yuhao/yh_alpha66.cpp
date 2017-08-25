#include "include/qlSim.h"
namespace
{
class alpha66: public AlphaBase
{
    public:
    explicit alpha66(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        vwap(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_vwap")), 
        vol(dr.getData<QL_MATRIX<QL_FLOAT>>("volume")), 
        high(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_high")), 
        low(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_low")),
        open(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_open")),
        Num1(cfg->getAttributeIntDefault("para1",7)),
	Num2(cfg->getAttributeIntDefault("para2",11)),
	Num3(cfg->getAttributeIntDefault("para3",6)),
	Num4(cfg->getAttributeIntDefault("para4",3))
	
    {
    }
    void generate(int di) override
    {
		vector<float> l1(GLOBAL::Instruments.size(), nan);
		for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
	     
            {
            	vector<float> c1(Num1,nan);
            	float f2=0;

	    		for (int j = 0; j< Num1; ++j)
	    		{
	     		 c1[j] = vwap[di-delay-j][ii] - vwap[di-delay-j-Num4][ii];
	     		 f2 += float(c1[j]) * (Num1 - j) / ((Num1+1) * Num1 / 2.0);
	    		}
	    		l1[ii] = f2;
            }
    	}
    	QL_Oputils::rank(l1);
	
        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
	     
            {
                
		vector<float> c2(Num2,nan);
		vector<float> c3(Num3,nan);
		float f1;
		
		float f3=0;
		
	    
	    for (int j = 0; j<Num3; ++j)
	    {
	    for (int i = 0; i< Num2; ++i)
		{
		  f1 = open[di-delay-i-j][ii]- 0.5 * (high[di-delay-i-j][ii]+low[di-delay-i-j][ii]);
		  if (GLOBALFUNC::equal(f1,0)){alpha[ii]=0;}
		  else{
		  c2[i]=(low[di-delay-i-j][ii] * 0.96633 + low[di-delay-i-j][ii] * (1-0.96633) - vwap[di-delay-i-j][ii]) / f1;
		  }
		  f3 += c2[i] * (Num2 - i) / ((Num2+1) * Num2 / 2.0);
		}
		c3[j] = f3;
	    }  
		  
		 QL_Oputils::rank(c3);
		
	   alpha[ii] = (-1) * (float(l1[ii]) + float(c3[0]));
		
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
        AlphaBase * str = new alpha66(cfg);
        return str;
    }
}

