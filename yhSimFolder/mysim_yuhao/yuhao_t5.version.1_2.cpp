#include "include/qlSim.h"
namespace
{
class t5: public AlphaBase
{
    public:
    explicit t5(XMLCONFIG::Element *cfg):
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
                vector<float> factor5(48, 0);

                for (int i = 0 ; i < 48; ++i)
                {
                    if (last_close(di-delay,i+1,ii) > last_open(di-delay,i+1,ii))
                    {
                        factor1[i] = vwap(di-delay,i+1,ii) - last_open(di-delay,i+1,ii);
                        factor2[i] += 1;
                    }

                    factor3[i] = last_close(di-delay,i+1,ii) - last_open(di-delay,i+1,ii);
                    factor4[i] = vol_sum(di-delay,i+1,ii);
                    factor5[i] = (vwap(di-delay,i+1,ii)) * vol_sum(di-delay,i+1,ii);
                }

                for (int j = 0 ; j < 47; ++j)
                {
                    if (last_high(di-delay,j+2,ii) > last_high(di-delay,j+1,ii))
                    {
                        factor2[j] += 1;
                    }                    

                    if (last_low(di-delay,j+2,ii) > last_low(di-delay,j+1,ii))
                    {
                        factor2[j] += 1;
                    }
                }
                    if (QL_Oputils::sum(factor4) == 0 || QL_Oputils::sum(factor3) == 0 )
                     {
                         alpha[ii] = 0;
                     }
                    else 
                     {
                        alpha[ii] = - QL_Oputils::sum(factor5)/QL_Oputils::sum(factor4) * (QL_Oputils::sum(factor1) / QL_Oputils::sum(factor3)) * QL_Oputils::sum(factor2);
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
        AlphaBase * str = new t5(cfg);
        return str;
    }
}

