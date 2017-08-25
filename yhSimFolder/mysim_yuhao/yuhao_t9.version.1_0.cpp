#include "include/qlSim.h"
namespace
{
class t9: public AlphaBase
{
    public:
    explicit t9(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        vol_sum(dr.getData<QL_CUBE<QL_FLOAT>>("intervaldata_5min.vol_sum")),
        last_open(dr.getData<QL_CUBE<QL_FLOAT>>("intervaldata_5min.last_open")),
        last_close(dr.getData<QL_CUBE<QL_FLOAT>>("intervaldata_5min.last_close")),
        last_high(dr.getData<QL_CUBE<QL_FLOAT>>("intervaldata_5min.last_high")),
        last_low(dr.getData<QL_CUBE<QL_FLOAT>>("intervaldata_5min.last_low")),
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
                    factor1[i] = fabs(last_close(di-delay,i+1,ii) - last_open(di-delay,i+1,ii));
                    factor2[i] = min(fabs(last_high(di-delay,i+1,ii)-last_open(di-delay,i+1,ii)), fabs(last_high(di-delay,i+1,ii) - last_close(di-delay,i+1,ii)));
                    factor3[i] = min(fabs(last_low(di-delay,i+1,ii)-last_open(di-delay,i+1,ii)), fabs(last_low(di-delay,i+1,ii) - last_close(di-delay,i+1,ii)));

                    
               
                }

                // if (!GLOBALFUNC::equal(QL_Oputils::mean(factor1),0))
                //     {
                        alpha[ii] =  QL_Oputils::corr(factor1,factor3) * (QL_Oputils::mean(factor3) - QL_Oputils::mean(factor2)) * QL_Oputils::mean(factor1);
                    // }
                    // else
                    // {
                    //     alpha[ii] = 0;
                    // }

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
        const QL_CUBE<QL_FLOAT> &last_high;
        const QL_CUBE<QL_FLOAT> &last_low;
        const QL_CUBE<QL_FLOAT> &vwap;

    int Num1; 
};
}
extern "C"
{
    AlphaBase * createStrategy(XMLCONFIG::Element *cfg)
    {
        AlphaBase * str = new t9(cfg);
        return str;
    }
}

