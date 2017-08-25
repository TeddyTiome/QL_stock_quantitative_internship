#include "include/qlSim.h"
namespace
{
class t8: public AlphaBase
{
    public:
    explicit t8(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        vol_sum(dr.getData<QL_CUBE<QL_FLOAT>>("intervaldata_5min.vol_sum")),
        last_open(dr.getData<QL_CUBE<QL_FLOAT>>("intervaldata_5min.last_open")),
        last_close(dr.getData<QL_CUBE<QL_FLOAT>>("intervaldata_5min.last_close")),
        vwap(dr.getData<QL_CUBE<QL_FLOAT>>("intervaldata_5min.sum_volume_price")),
        Num1(cfg->getAttributeFloatDefault("para1",0.5))

    {
    }
    void generate(int di) override
    {

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                vector<float> factor1(48, 0);
                vector<float> factor2(48, 0);
                vector<float> factor3(48, 0);
                vector<float> factor4(48, 0);

                for (int i = 0 ; i < 48; ++i)
                {
                   
                        factor1[i] = last_open(di-delay,i+1,ii);
                        factor2[i] = vol_sum(di-delay,i+1,ii);
                        factor3[i] = last_close(di-delay,i+1,ii);
                        factor4[i] = vwap(di-delay,i+1,ii);
                }

                        alpha[ii] = ( QL_Oputils::slop(factor3.begin(),factor3.end()) - QL_Oputils::slop(factor1.begin(),factor1.end()) ) * QL_Oputils::corr(factor4,factor2);

                   
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
        const QL_CUBE<QL_FLOAT> &vol_sum;
        const QL_CUBE<QL_FLOAT> &last_open;
        const QL_CUBE<QL_FLOAT> &last_close;
        const QL_CUBE<QL_FLOAT> &vwap;

    int Num1; 
};
}
extern "C"
{
    AlphaBase * createStrategy(XMLCONFIG::Element *cfg)
    {
        AlphaBase * str = new t8(cfg);
        return str;
    }
}

