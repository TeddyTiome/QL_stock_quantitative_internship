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

                for (int i = 0 ; i < 48; ++i)
                {
                    if (vwap(di-delay,i+1,ii) == 0 || last_open(di-delay,i+1,ii) == 0 || last_close(di-delay,i+1,ii) == 0)
                    {
                        factor3[i] = 0;
                    }
                    else
                    {
                    factor1[i] = vwap(di-delay,i+1,ii) - last_close(di-delay,i+1,ii);
                    factor2[i] = abs(last_close(di-delay,i+1,ii) - last_open(di-delay,i+1,ii));
                        if (factor2[i] == 0)
                        {
                            factor3[i] = 0;
                        }
                        else
                        {
                            factor3[i] = factor1[i] / factor2[i];
                        }
                    }
                }
             
                // QL_Oputils::rank(factor3);  

                float temp1 = 0;
                // float temp2 = 0;
                float temp3 = 0;
                float temp4 = 0;
                for (int j = 0; j<48; ++j)
                {
                    if (factor3[j] > Num1)
                    {
                        temp1 += factor3[j];
                        // temp2 += (vwap(di-delay,j+1,ii) - last_close(di-delay,j+1,ii)) * vol_sum(di-delay,j+1,ii);
                        temp3 += factor3[j] * vwap(di-delay,j+1,ii) * vol_sum(di-delay,j+1,ii);
                    }
                    temp4 += vwap(di-delay,j+1,ii) * vol_sum(di-delay,j+1,ii);
                }

                if (temp4 == 0)
                {
                    alpha[ii] = 0;
                }                
                else
                {
                    alpha[ii] =  -1 * temp1 * temp3 / temp4 * QL_Oputils::mean(factor3);
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

