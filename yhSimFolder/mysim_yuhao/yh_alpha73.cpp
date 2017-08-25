#include "include/qlSim.h"
namespace
{
class alpha73: public AlphaBase
{
    public:
    explicit alpha73(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        vwap(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_vwap")), 
        low(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_low")),
        open(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_open")),
        Num1(cfg->getAttributeIntDefault("para1",4)),
	Num2(cfg->getAttributeIntDefault("para2",2)),
	Num3(cfg->getAttributeIntDefault("para3",3)),
	Num4(cfg->getAttributeIntDefault("para4",16))

    {
    }
    void generate(int di) override
    {	
    	vector<float> l1(GLOBAL::Instruments.size(), nan);

		 for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
            	vector<float> c1(Num2,nan);
            	float f1=0;

            	for (int a=0; a<Num2;++a){
	    			c1[a] = vwap[di-delay-a][ii]-vwap[di-delay-a-Num1][ii];
	    		}
	    		
	    		for (int a=0;a<Num2;++a){
	    			f1 += float(c1[a]) *(Num2-a) / ((Num2+1)*Num2/2.0);
	    		}

	    		l1[ii] = f1;
            }
        }
        QL_Oputils::rank(l1);
	
        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                
				vector<float> c4(Num4,nan);
				vector<float> c3(Num3,nan);
				float f2;
				float f3;
				
	    		for (int c=0; c<Num4;++c){	
                        c4[c] = 0.0;
	    			for (int b=0; b<Num3; ++b){
	    				f2 = open[di-delay-b-c][ii]*0.147155+low[di-delay-b-c][ii]*(1-0.147155);
	    				f3 = open[di-delay-b-c][ii]*0.147155+low[di-delay-b-c][ii]*(1-0.147155)-(open[di-delay-b-c-Num2][ii]*0.147155+low[di-delay-b-c-Num2][ii]*(1-0.147155));
	    					if (!(GLOBALFUNC::equal(f2,0))){
	    						c3[b]= (-1.0*(f3/f2));
	    					}
	    					else {c3[b]=0;}

						c4[c] += float(c3[b]) * (Num3-b) / ((Num3+1)*Num3/2.0);
	    			}
		
	    		}
	    
	    		QL_Oputils::rank(c4);
	    	
	    
	    		if (l1[ii]<c4[0]){
	    			alpha[ii] =  -1*float(c4[0]);
	    		}
	    		else {
	    			alpha[ii] =  -1 * float(l1[ii]);
	    		}



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
        AlphaBase * str = new alpha73(cfg);
        return str;
    }
}

