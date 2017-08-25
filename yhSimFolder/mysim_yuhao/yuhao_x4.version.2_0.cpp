#include "include/qlSim.h"
namespace
{
class x4: public AlphaBase
{
    public:
    explicit x4(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        vwap(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_vwap")), 
        vol(dr.getData<QL_MATRIX<QL_FLOAT>>("volume")), 
        high(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_high")), 
        low(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_low")),
        Num1(cfg->getAttributeIntDefault("para1",20)),
	Num2(cfg->getAttributeIntDefault("para2",8)),
	Num3(cfg->getAttributeIntDefault("para3",10)),
	Num4(cfg->getAttributeIntDefault("para4",3)),
	Num5(cfg->getAttributeIntDefault("para5",18)),
	Num6(cfg->getAttributeIntDefault("para6",6))

    {
    	adv40.resize (GLOBAL::Dates.size());

    	for (int di = 0; di< GLOBAL::Dates.size(); ++di)
    	{
    		adv40[di].resize(GLOBAL::Instruments.size(),nan);
    	}
    }

    void generate(int di) override
    {
	    vector<float> rank2(GLOBAL::Instruments.size(),nan);
	    vector<float> rank1(GLOBAL::Instruments.size(),nan);

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {

                vector<float> c1(Num1,nan);
                for (int j = 0; j < Num1; ++ j)
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
            	rank1[ii] = 0.0;
            	vector<float> c1(Num2,nan);
                vector<float> c2(Num2,nan);
                vector<float> c3(Num3,nan);
                for (int j = 0 ; j< Num3;++j){
	    		for (int i =0; i<Num2; ++i){
	    			c1[i]=0.5*(high[di-delay-i-j][ii]+low[di-delay-i-j][ii]);
	    			c2[i]=adv40[di-delay-i-j][ii];
	    		}
                c3[j] = QL_Oputils::corr(c1,c2);}

	    		rank1[ii] = QL_Oputils::mean(c3);

	    		
            }
    	}
	
		// QL_Oputils::rank(rank1);

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
				vector<float> c5(Num4,nan);
				vector<float> c6(Num6,nan);
				vector<float> c7(Num5,nan);
				vector<float> c8(Num6,nan);
				
                    
				    	for (int j=0; j<Num6; ++j){
				    		for (int i=0;i<Num4;++i){
				    			c5[i] = vwap[di-delay-i-j][ii];
				    		}
				    		
				    		QL_Oputils::rank(c5);
				    		c6[j]= c5[0];

				    		for (int m=0;m<Num5;++m){
				    			c7[m] = vol[di-delay-m-j][ii];
				    		}
				    
				    		QL_Oputils::rank(c7);
				    		c8[j]= c7[0];
						}
					
						rank2[ii] = QL_Oputils::corr(c6,c8);
					
	    	}
	    }

	    // QL_Oputils::rank(rank2);

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
            alpha[ii] = -1*rank1[ii] * rank2[ii];
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
            ar & adv40;
        }
	const QL_MATRIX<QL_FLOAT> &vwap;
	const QL_MATRIX<QL_FLOAT> &vol;
	const QL_MATRIX<QL_FLOAT> &high;
	const QL_MATRIX<QL_FLOAT> &low;
	vector<vector<float> > adv40;

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
        AlphaBase * str = new x4(cfg);
        return str;
    }
}

