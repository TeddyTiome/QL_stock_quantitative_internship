#include "include/qlSim.h"
namespace
{
class alpha84: public AlphaBase
{
    public:
    explicit alpha84(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        vwap(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_vwap")),
        close(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_close")),
        Num1(cfg->getAttributeIntDefault("para1",15)),
	Num2(cfg->getAttributeIntDefault("para2",20)),
	Num3(cfg->getAttributeIntDefault("para3",4))
    {
    }
    void generate(int di) override
    {

		
        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                vector<float> c1(Num1,nan);
		vector<float> c2(Num2,nan);
		
		float f1;
		float f2;
		float f3;

		for (int i=0; i<Num2;++i){
			for (int j=0; j<Num1;++j){
				c1[j]=vwap[di-delay-i-j][ii];
			}
			f1 = QL_Oputils::hhv(c1.begin(),c1.end());
			c2[i]=vwap[di-delay-i][ii]-f1;
		
	    }
	    QL_Oputils::rank(c2);
	    f2 = float(c2[0]);
	    f3 = close[di-delay][ii]-close[di-delay-Num3][ii];
	    if (!(GLOBALFUNC::equal(f2,0))){alpha[ii]= powf(f2,f3);}
	    else {alpha[ii]=0;}
	    
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
	const QL_MATRIX<QL_FLOAT> &close;

        int Num1; 
	int Num2;
	int Num3;

};
}
extern "C"
{
    AlphaBase * createStrategy(XMLCONFIG::Element *cfg)
    {
        AlphaBase * str = new alpha84(cfg);
        return str;
    }
}

